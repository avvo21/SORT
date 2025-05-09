#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "list.h"
#include <time.h>

#include <unistd.h>
#include <netdb.h>

#define BUF_SIZE 1000

int port = 8000;

ItemType* findGude(int v, LIST guides){

	LIST tmp = guides;
	ItemType* bestGuide = NULL;
	int excess = 10000;

	while (!isEmpty(tmp)){
		/*Controllo se il numero minimo della guida soddisfa il criterio:
			- Il numero di visitatori minimi deve essere < v
			- Devo minimizzare il sovrannumero
		*/
		if(tmp->item.min_v <= v && tmp->item.max_v >= v && (tmp->item.max_v - v) < excess){
			excess = tmp->item.max_v - v;
			bestGuide = &tmp->item;
		}
		
		tmp = tmp->next;
	}

	return bestGuide;

}

int main() 
{
	ItemType msg = {0};

	LIST guides = NewList();

	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	// Socket opening
	int sockfd = socket( PF_INET, SOCK_STREAM, 0 );  
	if ( sockfd == -1 ) 
	{
		perror("Error opening socket");
		exit(1);
	}
	
	int options = 1;
	if(setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof (options)) < 0) {
		perror("Error on setsockopt");
		exit(1);
	}

	bzero( &serv_addr, sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	// Address bindind to socket
	if ( bind( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) == -1 ) 
	{
		perror("Error on binding");
		exit(1);
	}
	
	// Maximum number of connection kept in the socket queue
	if ( listen( sockfd, 20 ) == -1 ) 
	{
		perror("Error on listen");
		exit(1);
	}

	socklen_t address_size = sizeof( cli_addr );	
	
	while(1) {	

		printf("\nAspettando un nuovo utente...\n");
		
		// New connection acceptance		
		int newsockfd = accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );      
		if (newsockfd == -1) {
			perror("Error on accept");
			exit(1);
		}
		
		// Message reception
		ssize_t received = recv( newsockfd, &msg, sizeof(msg), 0 );
		if (received == -1) {
			perror("Error on receive");
			exit(1);
		}

		msg.sokId = newsockfd;

		switch (msg.type) {

			case TYPE_G: {

				printf("Connessione della guida \"%s\"\n", msg.name);
				guides = EnqueueFirst(guides, msg);

				printf("La guida è stata messa in attesa\n");

				break;	
			}


			case TYPE_V: { 
				printf("Gruppo di %d visitatori connesso\n", msg.v);

				ItemType* found_g;
				found_g = findGude(msg.v, guides);

				if(found_g != NULL){
					printf("Il visitatore è stato assegnato alla guida %s\n", found_g->name);

					strcpy(msg.name, found_g->name);

					if ( send(msg.sokId, &msg, sizeof(msg), 0) == -1 ) {
						perror("Error on send\n");
						exit(1);
					}

					int g_sockId = found_g->sokId;

					guides = Dequeue(guides, *found_g);
					
					if ( send(g_sockId, &msg, sizeof(msg), 0) == -1 ) {
						perror("Error on send\n");
						exit(1);
					}

					close(g_sockId);

				} else {
					printf("Non è stata trovata nessuna guida\n");

					printf("Invio la richiesta al server...\n");

					strcpy(msg.name, ""); // Messaggio per comunicare al client che non ci sono guide 

					if ( send(msg.sokId, &msg, sizeof(msg), 0) == -1 ) {
						perror("Error on send\n");
						exit(1);
					}
				}


				close(msg.sokId);
				
				break;
			}

			default:
				printf("Cliente di tipologia sconosciuta\n");
				close(newsockfd);
				break;
		}

		printf("\nLista guide disponibili:\n");
		PrintList(guides);
		printf("\n");


	}	

	close(sockfd);

	return 0;
}
