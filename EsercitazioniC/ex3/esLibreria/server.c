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

void remove_first_char(char *str) {
    if (str == NULL || str[0] == '\0') return; // Controllo per stringa vuota
    int i = 0;
    while (str[i] != '\0') {
        str[i] = str[i + 1]; // Sposta i caratteri a sinistra
        i++;
    }
}

int main() 
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	LIST warehouse, waiting_list;

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
	
	Message buf;
	ItemType buf_item;	


	while(1) {	

		printf("\nAspettando un nuovo utente...\n");
		
		// New connection acceptance		
		int newsockfd = accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );      
		if (newsockfd == -1) {
			perror("Error on accept");
			exit(1);
		} 
		
		//bzero(buf, BUF_SIZE);
		
		// Message reception
		ssize_t received = recv( newsockfd, &buf, sizeof(Message), 0 );
		if (received == -1) {
			perror("Error on receive");
			exit(1);
		} 
		/* else if (received != sizeof(ItemType)) {
			perror("Incomplete data received\n");
			exit(1);
		}*/

		//Seleziono se l'utente è cliente o venditore
		switch (buf.name[0]) {

			case 'C': // Il client è una casa editrice
				remove_first_char(buf.name);
				strcpy(buf_item.name, buf.name);
				buf_item.amount = buf.amount;
		
				ItemType* book_w = Find(warehouse, buf_item);
		
				if (book_w == NULL) {
					// Nuovo libro
					printf("Aggiungo un nuovo libro\n");
					warehouse = EnqueueOrdered(warehouse, buf_item);  
				} else {
					// Il libro è già presente in magazzino
					printf("Modifico la quantità del libro\n");
					book_w->amount += buf_item.amount;
				}
		
				Message rep;
				strcpy(rep.name, "Libro Ricevuto");
				send(newsockfd, &rep, sizeof(Message), 0);
				close(newsockfd);

				book_w = Find(warehouse, buf_item);
				// Controllo che il libro esista prima di usarlo
				ItemType* book_wl = Find(waiting_list, buf_item);
				while (book_wl != NULL && book_w->amount > 0) {

					printf("Il libro appena inserito era già richiesto, lo invio\n");
					strcpy(rep.name, "Libro Presente e Spedito");
					send(book_wl->sockId, &rep, sizeof(Message), 0);
					close(book_wl->sockId);

					book_w->amount--;

					if(book_w->amount <= 0){
						printf("Libro termintato");
						warehouse = Dequeue(warehouse, *book_w);
					}
	
					waiting_list = Dequeue(waiting_list, *book_wl);
					book_wl = Find(waiting_list, buf_item);
					
				}
				
				break;
		
			case 'L': // Il client è un lettore
				remove_first_char(buf.name);
				strcpy(buf_item.name, buf.name);
		
				ItemType* book_f = Find(warehouse, buf_item);
		
				if (book_f == NULL || book_f->amount <= 0) {
					// Il libro non è disponibile, metto in lista d'attesa
					printf("Il libro richiesto non è presente, metto il cliente in attesa\n");
					buf_item.sockId = newsockfd;
					waiting_list = EnqueueLast(waiting_list, buf_item);
				} else {
					// Il libro è disponibile, lo spedisco al cliente
					Message rep;
					printf("Il libro richiesto è presente, lo invio\n");
					strcpy(rep.name, "Libro Presente e Spedito");
					send(newsockfd, &rep, sizeof(Message), 0);
					close(newsockfd);
		
					book_f->amount--;  // Decremento corretto della quantità
					if(book_f->amount <= 0){
						printf("Libro termintato");
						warehouse = Dequeue(warehouse, *book_f);
					}
				}
				break;
		
			default:
				printf("Cliente di tipologia sconosciuta\n");
				close(newsockfd);
				break;
		}
		PrintList(warehouse);

	}

	close(sockfd);

	return 0;
}



