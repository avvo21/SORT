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


LIST greedySelectionF(LIST customers, ItemType* supplier) {
    LIST selected = NewList();
    int remaining = supplier->vax;
	int total_assigned = 0;
    
    LIST current = customers;
    while (!isEmpty(current)) {
        if (current->item.patients <= remaining) {
            selected = EnqueueLast(selected, current->item);
            remaining -= current->item.patients;
			total_assigned += current->item.patients;
        }
        current = current->next;
    }
    
    if (total_assigned >= supplier->min_req) {
        supplier->vax = remaining; 
        return selected;
    } else {
        DeleteList(selected);
        return NewList();
    }
}

ItemType* greedySelectionC(LIST suppliers, ItemType* customer) {
    ItemType* best_supplier = NULL;
    int max_vax = -1;

    LIST current = suppliers;
    while (!isEmpty(current)) {
        // Verifica i criteri di selezione
        if (current->item.vax >= customer->patients && 
            customer->patients >= current->item.min_req &&
            current->item.vax > max_vax) 
        {
            best_supplier = &(current->item); // Puntatore diretto all'elemento nella lista
            max_vax = current->item.vax;
        }
        current = current->next;
    }

    // Aggiorna lo stato del fornitore se trovato
    if (best_supplier != NULL) {
        best_supplier->vax -= customer->patients;
    }

    return best_supplier; // NULL se nessun fornitore valido
}


int main() 
{
	ItemType msg = {0};

	LIST customer_list, supplier_list;

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
		
		//bzero(buf, BUF_SIZE);
		
		// Message reception
		ssize_t received = recv( newsockfd, &msg, sizeof(msg), 0 );
		if (received == -1) {
			perror("Error on receive");
			exit(1);
		}

		msg.sokId = newsockfd;

		//Seleziono se l'utente è cliente o venditore
		switch (msg.type) {

			//per inviare la lista fare un ciclo e come ultimo messaggio mettere TYPE_END_COMMUNICATION

			case TYPE_F: {
				printf("Richiesta da Fornitore: %s - %d\n", msg.name, msg.vax);

				LIST matched_customer = greedySelectionF(customer_list, &msg);

				if(isEmpty(matched_customer)){
					printf("Nessun cliente può essere soddisfatto, fornitore messo in attesa\n");
					supplier_list = EnqueueLast(supplier_list, msg);

				} else {
					printf("Si è tovato un cliente disponibile\n");
					while (!isEmpty(matched_customer)){

						printf("Invio vaccini a %s\n", matched_customer->item.name);

						ItemType cus_msg;
						cus_msg.type = TYPE_C;
						sprintf(cus_msg.name, "Ordine fornito da %s", msg.name);

						if ( send(matched_customer->item.sokId, &cus_msg, sizeof(cus_msg), 0) == -1 ) {
							perror("Error on send: send TYPE_F customer\n");
							close(matched_customer->item.sokId);
							exit(1);
						}
						customer_list = Dequeue(customer_list, matched_customer->item);
						close(matched_customer->item.sokId);
						
						ItemType sup_msg;
						sup_msg.type = TYPE_F;
						sprintf(sup_msg.name, "Ordine servito a %s di quantità %d", matched_customer->item.name, matched_customer->item.patients);
						
						if(send(msg.sokId, &sup_msg, sizeof(sup_msg), 0) == -1){
							perror("Error on send: send TYPE_F supplier\n");
							close(msg.sokId);
							exit(1);
						}

						

						matched_customer = DequeueFirst(matched_customer);
					}

					//controllo se il fornitore può ancora fornire vaccini
					if(msg.vax < msg.min_req){	
						printf("Il fornitore non può più fornire vaccini, chiudo le comunicazioni\n");
						
						ItemType end_msg = {.type = TYPE_END_COMMUNICATION};
						if ( send(msg.sokId, &end_msg, sizeof(end_msg), 0) == -1 ) {
							perror("Error on send: send TYPE_F supplier closing\n");
							close(msg.sokId);
							exit(1);
						}
						close(msg.sokId);

					} else {
						printf("Il fornitore può ancora fornire vaccini, lo aggiungo alla lista\n");
						supplier_list = EnqueueLast(supplier_list, msg);
					}
				}
				break;	
			}

			
			case TYPE_C: { 
				printf("Richiesta da Cliente: %s - %d\n", msg.name, msg.patients);

				ItemType* matched_supplier = greedySelectionC(supplier_list, &msg);

				if(matched_supplier == NULL){
					printf("Non è stato trovato nessun fornitore, aggiungo il cliente alla coda\n");
					customer_list = EnqueueOrdered(customer_list, msg);
				} else {
					printf("È stato trovato un fornitore: %s\n", matched_supplier->name);

					ItemType cus_msg;
					cus_msg.type = TYPE_C;
					sprintf(cus_msg.name, "Ordine fornito da %s", matched_supplier->name);

					if(send(msg.sokId, &cus_msg, sizeof(cus_msg), 0) == -1){
						perror("Error on send: send TYPE_C customer\n");
						close(msg.sokId);
						exit(1);
					}
					close(msg.sokId);

					ItemType sup_msg;
					sup_msg.type = TYPE_F;
					sprintf(sup_msg.name, "Ordine servito a %s di quantità %d", msg.name, msg.patients);

					if(send(matched_supplier->sokId, &sup_msg, sizeof(sup_msg), 0) == -1){
						perror("Error on send: send TYPE_C supplier\n");
						close(matched_supplier->sokId);
						exit(1);
					}

					//controllo se il fornitore può ancora fornire vaccini
					if(matched_supplier->vax < matched_supplier->min_req){	
						printf("Il fornitore non può più fornire vaccini, chiudo le comunicazioni\n");
						
						ItemType end_msg = {.type = TYPE_END_COMMUNICATION};
						if ( send(matched_supplier->sokId, &end_msg, sizeof(end_msg), 0) == -1 ) {
							perror("Error on send: send TYPE_C supplier closing\n");
							close(matched_supplier->sokId);
							exit(1);
						}
						supplier_list = Dequeue(supplier_list, *matched_supplier);
						close(matched_supplier->sokId);
					}
				}
				break;
			}

			default:
				printf("Cliente di tipologia sconosciuta\n");
				close(newsockfd);
				break;
		}

		printf("\nFornitori in attesa:\n");
		PrintList(supplier_list);

		printf("\nClienti in attesa:\n");
		PrintList(customer_list);

	}	

	close(sockfd);

	return 0;
}
