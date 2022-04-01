from controller import Robot

import socket
import sys

def send_cmd(sock, message):
    #print >>sys.stderr, 'sending "%s"' % message
    sock.sendall(message + b'\r\n')
    data=b'' 
    while (data[-5:] != b'Ok\r\n>') and (data[-8:] != b'ERROR\r\n>'):
        try:
             parc = sock.recv(1)
        except socket.timeout:
             parc=b''

        data = data + parc
    #print >>sys.stderr, 'received "%s"' % data
    return data		

""" Controller to drive epuck to follow a line. """

def run_robot(robot):
    
    time_step = int(robot.getBasicTimeStep()) * 1 #96
    max_speed = 6.28
    
    # Motors
    left_motor = robot.getDevice('left wheel motor');
    right_motor = robot.getDevice('right wheel motor');
    left_motor.setPosition(float('inf')) 
    right_motor.setPosition(float('inf'))
    left_motor.setVelocity(0)  
    right_motor.setVelocity(0) 
    
    #Enable ir sensors
    left_ir = robot.getDevice('ir0');
    left_ir.enable(time_step)
    
    right_ir = robot.getDevice('ir1');
    right_ir.enable(time_step)
    
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Connect the socket to the port where the server is listening
    server_address = ('localhost', 5000)
    print ( 'connecting to %s port %s' % server_address)
    sock.connect(server_address)

   
    # Reading the PICSimLab header message
    data = sock.recv(100)
    print ('received "%s"' % data)
    
    sock.settimeout(0.0001)
    
    send_cmd(sock,b'reset')

    #Step simulation
    while robot.step(time_step) != -1:
        
        #read ir sensors
        left_ir_value = left_ir.getValue()
        right_ir_value = right_ir.getValue();
        
        print("1 left: {} right: {}".format(left_ir_value,right_ir_value))
        
        #scaled  range 0 to 200 
        left_ir_value_s =  (left_ir_value / 1.8);
        right_ir_value_s = (right_ir_value / 1.8);
      
        if (left_ir_value_s > 5):
            left_ir_value_s = 5.0
            
        if (left_ir_value_s < 0):
            left_ir_value_s = 0
            
        if (right_ir_value_s > 5):
            right_ir_value_s = 5.0
            
        if (right_ir_value_s < 0):
            right_ir_value_s = 0

                                    
        print("2 left: {} right: {}".format(left_ir_value_s,right_ir_value_s))
      
        
        send_cmd(sock,b'set apin[23] '+bytes(str(float(left_ir_value_s)), encoding='ascii'))
        
        send_cmd(sock,b'set apin[24] '+bytes(str(float(right_ir_value_s)), encoding='ascii'))

        #data=send_cmd(sock,"get part[00].out[04]")
        #m1_dir = int(data[29:30])
        #m1_speed = int(data[38:41])

        #data=send_cmd(sock,"get part[01].out[04]")
        #m2_dir = int(data[29:30])
        #m2_speed = int(data[38:41])
        
        
        data=send_cmd(sock,b'dumpr B3');
        m1_speed= int(data[6:8], 16)/2.5
        data=send_cmd(sock,b'get pin[15]');
        m1_dir= int(data[9:10])  
        
        data=send_cmd(sock,b'dumpr B3'); #fixme
        m2_speed= int(data[6:8], 16)/2.5
        data=send_cmd(sock,b'get pin[18]');
        m2_dir= int(data[9:10])  
        
        #print("m3 dir: {} speed: {}".format(m3_dir,m3_speed))
    
        #print("m1 dir: {} speed: {}".format(m1_dir,m1_speed))
        #print("m2 dir: {} speed: {}".format(m2_dir,m2_speed))
        
       
        
        left_speed  = (m1_speed * -(2*m1_dir-1) * max_speed) /100.0
        right_speed = (m2_speed * -(2*m2_dir-1) * max_speed) /100.0
        
        #print("speed left: {} right: {}".format(left_speed,right_speed))
      
      
        left_motor.setVelocity(left_speed)
        right_motor.setVelocity(right_speed)
        
        #send_cmd(sock,"sync")
    
    try:
        send_cmd(sock,b'quit')
    finally:
        #print >>sys.stderr, 'closing socket'
        sock.close()        
    
        
if __name__ == "__main__":
    my_robot = Robot()
    run_robot(my_robot)
   


    
   
