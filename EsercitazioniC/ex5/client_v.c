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
	msg.type = TYPE_V;

	if (argc < 2) {
		printf("Inserire NumeroVisitatori\n");
		return 0;
	}
	else{
		msg.v = atoi(argv[1]);
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

	if(strcmp(msg.name, "") != 0){
		printf("Il gruppo verra guidato da %s\n", msg.name);
	} else {
		printf("Purtroppo non ci sono guide adatte al gruppo\n");
	}

	close(sockfd);

	return 0;
}