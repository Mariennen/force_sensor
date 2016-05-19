# Communication

import socket
import sys
from thread import *

HOST = '10.0.1.3'   # Symbolic name meaning all available interfaces
PORT = 5432

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
print 'Socket created'

try:
    s.bind((HOST, PORT))
except socket.error , msg:
    print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
    sys.exit()

print 'Socket bind complete'
s.listen(10)
print 'Socket now listening'

#Function for handling connections
def clientthread(conn):
    #Sending message to connected client
    # conn.send('Welcome to the server. Receving Data...\n') #send only takes string
    file = open("Force_Marina_2.txt", "w")
    # file1= open ("Force_RP.txt","w")
    # file2= open ("Force_RP.txt","w")

    #infinite loop so that function do not terminate and thread do not end.
    while True:
        #Receiving from client (force values from arduino)
        force = conn.recv(16)
        # reply = 'Message Received at the server!\n'
        print 'Received: ' + force
        file.write(force + "\n")

        # cont=1
        # for i in range (2,force.size,2)
        #     leftforce[cont,1] = force [i,1]
        #     cont+=1
        # file1.write(leftforce+ "\n")

        # conta=1
        # for i in range (2,force.size,2)
        #     rightforce[cont,1] = force [i,1]
        #     conta+=1
        # file2.write(rigthforce+ "\n")

                # if not data:
        #     break
        # conn.sendall(data)

    file.close()
    # file1.close()
    # file2.close()

    conn.close()


#now keep talking with the client
while 1:
    #wait to accept a connection
    conn, addr = s.accept()
    print 'Connected with ' + addr[0] + ':' + str(addr[1])
    #start new thread
    start_new_thread(clientthread ,(conn,))

    # print s.getsockname()
    # print s.getpeername()

s.close()

