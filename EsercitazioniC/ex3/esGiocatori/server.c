#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "list.h"
#include <time.h>

#include <unistd.h>
#include <netdb.h>

#define BUF_SIZE 1000
#define N 4

int port = 8000;

// Return TRUE if is a new player, FALSE otherwise
BOOL isPresentIn(LIST l, ItemType p){
	return  Find(l,p) == NULL;
}

void updatePoint(LIST l, ItemType p, int pt){
	ItemType* player = Find(l, p);

	if(player != NULL){
		player->point += pt;
		player->num_match++;

	} else {
		perror("Error updating points");
		exit(1);
	}
}

int main() 
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	LIST history_list, waiting_list;

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
	
	char buf[BUF_SIZE];	


	while(1) 
	{	
		for (int i = 0; i < N; i++){

			printf("\nAspettando un nuovo giocatore...\n");
			
			// New connection acceptance		
			int newsockfd = accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );      
			if (newsockfd == -1) {
				perror("Error on accept");
				exit(1);
			}
			
			bzero(buf, BUF_SIZE);
			
			// Message reception
			if ( recv( newsockfd, buf, BUF_SIZE, 0 ) == -1) {
				perror("Error on receive");
				exit(1);
			}

			ItemType player;
			strcpy(player.name, buf);
			player.sockfd = newsockfd;

			printf("%s si è connesso al server\n", player.name);

			if (isPresentIn(history_list, player)){
				printf("%s è un nuovo giocatore\n", player.name);
				player.point = 0;
				player.num_match =0;
				history_list = EnqueueOrdered(history_list, player);				
			}
			
			waiting_list = EnqueueFirst(waiting_list, player);

			printf("\nLista giocatori:\n");
			PrintList(history_list);
			
			printf("\nIn attesa:\n");
			PrintList(waiting_list);	
		}

		printf("\n\nLimite giocatori per una partita raggiunto\n");

		int waiting_list_length = getLength(waiting_list);
		int random_index;
		int pt = 3;
		int position = 0;

		srand(time(NULL));

        while (waiting_list > 0){
			
			random_index = rand() % waiting_list_length;
			ItemType* random_player = GetItemAt(waiting_list, random_index);
			position++;
			updatePoint(history_list, *random_player, pt);

			char msg[50];
			
			sprintf(msg, "La tua posizione è %d e hai ottenuto %d\n",position, pt);

			if (send(random_player->sockfd, &msg, sizeof(msg), 0 ) == -1) {
				perror("Error on send");
				exit(1);
			}

			printf(" %s si è posizionato %d: chiudo la socket %d\n", random_player->name,position, random_player->sockfd);
			close(random_player->sockfd);

			if(pt>0)
				pt--;
			
			waiting_list = Dequeue(waiting_list, *random_player);
			waiting_list_length--; 
			
		}

		printf("\nLista aggiornata dopo la partita:\n");
			PrintList(history_list);

	}

	close(sockfd);


	return 0;
}



