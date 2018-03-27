/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, newsockfd2, portno, clilen;
     char buffer[256], buffer2[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");

	
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
			  
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
	  
	  printf("Aqui");
	  
	 listen(sockfd,5);
	 newsockfd2 = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     
     n = write(newsockfd,"Chat iniciado client 1",30);
     if (n < 0) error("ERROR writing to socket");
	 
	 n = write(newsockfd2,"Chat iniciado client 2",30);
     if (n < 0) error("ERROR writing to socket");
	 
	 bzero(buffer,256);
     n = read(newsockfd,buffer,255);
     if (n < 0) error("ERROR reading from socket");
     printf("Here is the message from client 1: %s\n",buffer);
	 
	 
	 n = write(newsockfd2,buffer,18);
     if (n < 0) error("ERROR writing to socket");
	 
	 n = write(newsockfd,"Mensagem enviada", 20);
	 if (n < 0) error("ERROR writing to socket");
	 
	 close(newsockfd);
	 close(newsockfd2);
	 
     return 0; 
}
