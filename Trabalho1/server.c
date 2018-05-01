/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
   gcc server2.c -lsocket
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>

#include <signal.h>

#define MAX_CLIENTS	5

void dostuff(int); /* function prototype */
int inicializaSocket(int);
void* trataCliente(int);
void enviarMensagem(char*, int);
void enviarMensagemTodos(char*);
void deletarClient(int);
void adicionarClient(int);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int newsockfd[5] = { -1, -1, -1, -1, -1};
int sockfd;

int main(int argc, char *argv[])
{
    
    pthread_t thread_listen;
    int newsock, portno, clilen;
    struct sockaddr_in cli_addr;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    portno = atoi(argv[1]);

    sockfd = inicializaSocket(portno);

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    printf("----------------SERVER INICIADO----------------");

    while (1) {
        clilen = sizeof(cli_addr);

        newsock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsock < 0) 
            error("ERROR on accept");

        adicionarClient(newsock);

        pthread_create(&thread_listen, NULL, &trataCliente, newsock);

        sleep(1);
    } /* end of while */
    return 0; /* we never get here */
}

void adicionarClient(int newsock){
    int i;
    
    for(i=0;i<MAX_CLIENTS;i++){
        if(newsockfd[i] == -1) {
            newsockfd[i] = newsock;
            return;
        }
    }
}

void deletarClient(int sock){
    int i;
	for(i=0;i<MAX_CLIENTS;i++){
		if(newsockfd[i] == sock){
            newsockfd[i] = -1;
            return;
		}
	}
    
}

int inicializaSocket(int portno){
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    return sockfd;
}

void *trataCliente(int client){
    char buffer_in[256];
    char buffer_out[256];
    int n;

    sprintf(buffer_out, "Cliente %d entrou na sala.\r\n", client);
    enviarMensagemTodos(buffer_out);
    n = read(client,buffer_in,255);    
    if (n < 0) 
        error("ERROR reading from socket");

    while(memcmp(buffer_in,"bye",strlen("bye"))!=0){
        buffer_in[n] = '\0'; /* Limpar cache das mensagens. */
        buffer_out[0] = '\0';

        sprintf(buffer_out, "[%d] %s\r\n", client, buffer_in);
        enviarMensagem(buffer_out, client);

        n = read(client,buffer_in,255);
        if (n < 0) 
            error("ERROR reading from socket");
    }

    write(client, "bye", strlen("bye"));
    close(client);
    sprintf(buffer_out, "Cliente %d saiu.\r\n", client);
    enviarMensagem(buffer_out, client);
    deletarClient(client);

    pthread_detach(pthread_self());

    return NULL;    
}

/* Envia mensagem para todos os clientes exceto o que está enviando. */
void enviarMensagem(char *s, int client){
	int i;
	for(i=0;i<MAX_CLIENTS;i++){
		if(newsockfd[i] != - 1){
			if(newsockfd[i] != client){
				write(newsockfd[i], s, strlen(s));
			}
		}
	}
}

/* Envia mensagem para todos os clientes. */
void enviarMensagemTodos(char *s){
    int i;
	for(i=0;i<MAX_CLIENTS;i++){
		if(newsockfd[i] != -1){
			write(newsockfd[i], s, strlen(s));
		}
	}
}