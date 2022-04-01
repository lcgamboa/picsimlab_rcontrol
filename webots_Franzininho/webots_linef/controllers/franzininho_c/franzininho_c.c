/*
 * File:          linefc_picsimlab.c
 * Date:
 * Description:
 * Author:
 * Modifications:
 */

#ifndef _WIN32
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include <netdb.h>
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
#include <math.h>

#include <webots/robot.h>
#include <webots/gyro.h>
#include <webots/distance_sensor.h>
#include <webots/accelerometer.h>
#include <webots/motor.h>

int sockfd=-1;
char buff[2048];
char cmd[256];



int send_cmd(const char * message);
void setblock(int sock_descriptor);
void setnblock(int sock_descriptor);

const double max_speed = 6.28;

int main(int argc, char **argv) {
  struct sockaddr_in servaddr;

  wb_robot_init();
  int timestep = (int)wb_robot_get_basic_time_step();

  // Get and enable devices.
  WbDeviceTag left_ir = wb_robot_get_device("ir0");
  wb_distance_sensor_enable(left_ir, timestep);
  WbDeviceTag right_ir = wb_robot_get_device("ir1");
  wb_distance_sensor_enable(right_ir, timestep);

  // Get motors and set them to velocity mode.
  WbDeviceTag left_motor = wb_robot_get_device("left wheel motor");
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_velocity(left_motor, 0);
  WbDeviceTag right_motor = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(right_motor, INFINITY);
  wb_motor_set_velocity(right_motor, 0);
  

 if ((sockfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
     printf ("socket error : %s \n", strerror (errno));
     exit (1);
    }
   memset (&servaddr, 0, sizeof (servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = inet_addr ("127.0.0.1");
   servaddr.sin_port = htons (5000);

   
   if (connect (sockfd, (struct sockaddr *) & servaddr, sizeof (servaddr)) < 0)
    {
#ifdef _WIN32
     printf ("connect error number: %i \n", WSAGetLastError ());
#else         
     printf ("connect error : %s \n", strerror (errno));
#endif
     close (sockfd);
     exit(1);
    }
   else
    {
     recv (sockfd, buff, 200, 0);
     printf ("%s", buff);
     
     setnblock (sockfd); 
     send_cmd("reset"); 
    }
    
  // Display the welcome message.
  printf("Start...\n");

  // Main loop
  while (wb_robot_step(timestep) != -1) {
    //const double time = wb_robot_get_time();  // in seconds.

    // Retrieve robot position using the sensors.
    
       //read ir sensors
       double left_ir_value = wb_distance_sensor_get_value(left_ir);
       double right_ir_value = wb_distance_sensor_get_value(right_ir);
    
        //scaled  range 0 to 5 
        int left_ir_value_s = 0;
        int right_ir_value_s = 0;
        
        //printf("%7.4f %7.4f\n",left_ir_value,right_ir_value);
      
        if( left_ir_value > 4)
        {
            left_ir_value_s = 1;
        }
        
        if ( right_ir_value > 4 )
        {
            right_ir_value_s = 1;
        }
                                    
        sprintf(cmd,"set pin[07] %i ",left_ir_value_s);
        send_cmd(cmd);
        
        sprintf(cmd,"set pin[03] %i ",right_ir_value_s);
        send_cmd(cmd);
        
   
   
        send_cmd("get pin[02]");
        int m1_dir;
        sscanf(buff+9,"%i",&m1_dir);
  
        send_cmd("get pin[06]");
        int m2_dir;
        sscanf(buff+9,"%i",&m2_dir);
        
        send_cmd("get pinm[05]");
        int m_speed;
        sscanf(buff+9,"%3d",&m_speed);
       
        //printf("m1dir: %i  m2dir: %i  speed: %i  %8.2f %8.2f  %i  %i \n",m1_dir,m2_dir,m_speed,right_ir_value,left_ir_value,right_ir_value_s,left_ir_value_s);
          
       
        
        double left_speed  = ((m_speed * m1_dir * max_speed) /200.0)+0.0;
        double right_speed = ((m_speed * m2_dir * max_speed) /200.0)+0.0;

    wb_motor_set_velocity(left_motor, left_speed);
    wb_motor_set_velocity(right_motor, right_speed);
  }

  send_cmd("quit");
  close(sockfd);
  
  wb_robot_cleanup();

  return EXIT_SUCCESS;
}



int
send_cmd(const char * message)
{
 strcpy(buff,message);
 strcat(buff,"\r\n"); 
 //printf ("sending '%s'\n", message);
 int n = strlen (buff);
 if (send (sockfd, buff, n, MSG_NOSIGNAL) != n)
  {
   printf ("send error : %s \n", strerror (errno));
   close (sockfd);
   exit(-1);
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
     if (n < 0)
    {
#ifndef _WIN32         
     if (errno != EAGAIN)
#else
     if (WSAGetLastError () != WSAEWOULDBLOCK)
#endif   
      {
          printf ("send error : %s \n", strerror (errno));
          close (sockfd);
          exit(-1);
      }
     }
    }

  }
 while (((bp < 5) || (strcmp (&buff[bp - 5], "Ok\r\n>"))) && ((bp < 8) || strcmp (&buff[bp - 8], "ERROR\r\n>")));

 return bp;
}

#ifdef _WIN32
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


void
setnblock(int sock_descriptor)
{
#ifndef _WIN_
 int flags;
 /* Set socket to non-blocking */

 if ((flags = fcntl (sock_descriptor, F_GETFL, 0)) < 0)
  {
   /* Handle error */
   //printf("Error fcntl nblock !!!!!!!\n");  
  }


 if (fcntl (sock_descriptor, F_SETFL, flags | O_NONBLOCK) < 0)
  {
   /* Handle error */
   //printf("Error fcntl nblock !!!!!!!\n");  
  }
#else
 unsigned long iMode = 1;
 ioctlsocket (sock_descriptor, FIONBIO, &iMode);
#endif 
}

void
setblock(int sock_descriptor)
{
#ifndef _WIN_
 int flags;
 /* Set socket to blocking */

 if ((flags = fcntl (sock_descriptor, F_GETFL, 0)) < 0)
  {
   /* Handle error */
   //printf("Error fcntl block !!!!!!!\n");  
  }


 if (fcntl (sock_descriptor, F_SETFL, flags & (~O_NONBLOCK)) < 0)
  {
   /* Handle error */
   //printf("Error fcntl block !!!!!!!\n");  
  }
#else
 unsigned long iMode = 0;
 ioctlsocket (sock_descriptor, FIONBIO, &iMode);
#endif
}
