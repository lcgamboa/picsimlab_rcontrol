
#ifndef _WIN_
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <fcntl.h>
#else
#include<winsock2.h>
#include<ws2tcpip.h>
#define SHUT_RDWR SD_BOTH
#define MSG_NOSIGNAL 0
#endif
//system headers independent
#include<errno.h>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<limits.h>

#ifdef _WIN_
WORD wVersionRequested = 2;
WSADATA wsaData;

__attribute__ ((constructor))
static void
initialize_socket(void)
{
 WSAStartup (wVersionRequested, &wsaData);
 if (wsaData.wVersion != wVersionRequested)
  {
   fprintf (stderr, "\n Wrong version\n");
   return;
  }
}

__attribute__ ((destructor))
static void
finalize_socket(void)
{
 WSACleanup ();
}
#endif

static int sockfd = -1;
static char buff[4096];

static int send_cmd(const char * message);

int
main(int argc, char ** argv)
{

 struct sockaddr_in servaddr;
 char * fname;
 char *cmd;


 if (argc < 2)
  {
   printf ("Use:\n%s file.hex\n", argv[0]);
   return 0;
  }

 fname = (char *) malloc (PATH_MAX);
#ifdef _WIN_
 if (!_fullpath (fname, argv[argc - 1], PATH_MAX))
  {
   fname = NULL;
  }
#else
 realpath (argv[argc - 1], fname);
#endif

 if (fname)
  {
   printf ("Loading file:%s\n", fname);
  }
 else
  {
   printf ("File not found: %s\n", argv[argc - 1]);
   return -1;
  }



 if ((sockfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
  {
   printf ("Socket error : %s \n", strerror (errno));
   exit (1);
  }
 memset (&servaddr, 0, sizeof (servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr = inet_addr ("127.0.0.1");
 servaddr.sin_port = htons (5000);

 if (connect (sockfd, (sockaddr *) & servaddr, sizeof (servaddr)) < 0)
  {
#ifdef _WIN_
   printf ("Conn. error: %i \n", WSAGetLastError ());
#else
   printf ("Conn. error: %s \n", strerror (errno));
#endif
   close (sockfd);
   return -1;
  }
 else
  {
   recv (sockfd, buff, 200, 0);
   //printf ("%s", buff);

   if (send_cmd ("info") >= 0)
    {
     if ((buff[0] == 'B') &&(buff[1] == 'o'))
      {
       char * tok = strtok (buff, "\r\n");
       printf ("%s\n", tok);
       tok = strtok (NULL, "\r\n");
       printf ("%s\n", tok);
       tok = strtok (NULL, "\r\n");
       printf ("%s\n\n", tok);
      }
     else
      {
       printf ("Error: Getting board info !\n");
       send_cmd ("quit");
       return -1;
      }
    }
   else
    {
     printf ("Conn. error!\n");
     return -1;
    }

   cmd = (char *) malloc (strlen (fname) + 20);

   if (!cmd)
    {
     printf ("Malloc error!\n");
     return -1;
    }

   sprintf (cmd, "loadhex %s", fname);

   if (send_cmd (cmd) >= 0)
    {
     if ((buff[0] == 'O') &&(buff[1] == 'k'))
      {
       printf ("File loaded !\n");
      }
     else
      {
       printf ("Error: File not loaded !\n");
       send_cmd ("quit");
       return -1;
      }
    }
   else
    {
     printf ("Conn. error!\n");
     return -1;
    }

   send_cmd ("quit");

  }

 free (cmd);
 free (fname);

 return 0;
}

int
send_cmd(const char * message)
{
 //printf ("sending '%s'\n", message);
 int n = strlen (message);
 if (send (sockfd, message, n, MSG_NOSIGNAL) != n)
  {
   printf ("send error : %s \n", strerror (errno));
   close (sockfd);
   return -1;
  }
 n = 2;
 if (send (sockfd, "\r\n", n, MSG_NOSIGNAL) != n)
  {
   printf ("send error : %s \n", strerror (errno));
   close (sockfd);
   return -1;
  }

 int bp = 0;
 buff[0] = 0;
 do
  {

   if ((n = recv (sockfd, buff + bp, 1, 0)) > 0)
    {
     bp += n;
     buff[bp] = 0;
     //printf ("%c", buff[bp-1]);
    }
   else
    {
     printf ("recv error : %s \n", strerror (errno));
     close (sockfd);
     return -1;
    }

  }
 while (((bp < 5) || (strcmp (&buff[bp - 5], "Ok\r\n>"))) && ((bp < 8) || strcmp (&buff[bp - 8], "ERROR\r\n>")));

 return bp;
}

