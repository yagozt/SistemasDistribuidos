#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include <pthread.h>

#include <signal.h>

int conectarSocket(int, char*);
void* enviarMsg(int);
void* lerMsg(int);
void saidaForcada(int);

void error(char *msg)
{
    perror(msg);
    exit(0);
}



int sockfd;

int main(int argc, char *argv[])
{
    int portno;
    pthread_t thread_a, thread_b;
    void* retval;

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);

    sockfd = conectarSocket(portno, argv[1]);

    signal(2, saidaForcada);

    while (1){
        printf("Inicializando client.\n");
        pthread_create(&thread_a, NULL, enviarMsg, sockfd);
        pthread_create(&thread_b, NULL, lerMsg, sockfd);

        pthread_join(thread_a, &retval);
        pthread_join(thread_b, &retval);
        close(sockfd);
        printf("Terminou.\n");
        return 0;      
        
    }
}

void* enviarMsg(int sock){
    char buffer[256];
    int n;

    while (1){
        /*if (signal(SIGINT, saidaForcada) != SIG_ERR) {
            n = write(sock, "bye", strlen("bye"));
            if (n < 0) 
                error("ERROR writing to socket");
            return 0;
        }*/
        memset(buffer, 0, 256);
        fgets(buffer, 255, stdin);
        n = write(sock, buffer, strlen(buffer));
        if (n < 0) 
            error("ERROR writing to socket");
        if(memcmp(buffer, "bye", strlen("bye")) == 0)
            return 0;
            
    }
}

void* lerMsg(int sock){
    char buffer[256];
    int n;
    
    while(1){
        memset(buffer, 0, 256);
        n = read(sock, buffer, 255);
        if (n < 0) 
            error("ERROR reading from socket");

        printf("%s\n",buffer);
        if(memcmp(buffer, "bye", strlen("bye")) == 0)
            return 0;
    }
}

int conectarSocket(int portno, char *argvServer){
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argvServer);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    return sockfd;
}

void saidaForcada(int signo) {
    write(sockfd, "bye", strlen("bye"));
}