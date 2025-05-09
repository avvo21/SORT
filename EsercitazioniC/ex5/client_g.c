#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"

#define BUF_SIZE 1000

char *host_name = "127.0.0.1"; /* local host */
int port = 8000;

int main(int argc, char *argv[]) 
{		
	ItemType msg = {0};
	msg.type = TYPE_G;

	if (argc < 4) {
		printf("Inserire Nome MinVisitatori MaxVisitatori\n");
		return 0;
	}
	else if(atoi(argv[2]) > atoi(argv[2])){
		printf("Il numero minimo deve essere più piccolo del massimo\n");
		return 0;
	} else {
		strcpy(msg.name, argv[1]);
		msg.min_v = atoi(argv[2]);
		msg.max_v = atoi(argv[3]);
	}
		
	struct sockaddr_in serv_addr;
	struct hostent* server;	
	
	if ( ( server = gethostbyname(host_name) ) == 0 ) 
	{
		perror("Error resolving local host\n");
		exit(1);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ((struct in_addr *)(server-> h_addr))->s_addr;
	serv_addr.sin_port = htons(port);
	
	int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
	if ( sockfd == -1 ) 
	{
		perror("Error opening socket\n");
		exit(1);
	}    

	if ( connect(sockfd, (void*)&serv_addr, sizeof(serv_addr) ) == -1 ) 
	{
		perror("Error connecting to socket\n");
		exit(1);
	}

	printf("Invio la richiesta al server...\n");

	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) {
		perror("Error on send\n");
		exit(1);
	}

	printf("Richiesta inviata, attendi per i risultati...\n");

	if ( recv(sockfd, &msg, sizeof(msg), 0) == -1 ) {
		perror("Error in receiving response from server\n");
		exit(1);
	}

	printf("A questa guida sono stati assegnati %d visitatori\n", msg.v);

	close(sockfd);

	return 0;
}