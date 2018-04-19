all: server client

server: server.c
	gcc server.c -o server -lpthread -Wall -ansi
client: client.c
	gcc client.c -o client -lpthread -Wall -ansi

clean:
	rm server client