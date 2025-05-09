all:	server clientC clientL 

server:	server.o list.o
	gcc -Wall -o server server.o list.o

clientC:	client_c.o list.o
	gcc -Wall -o client_c client_c.o list.o

clientL:	client_l.o list.o
	gcc -Wall -o client_l client_l.o list.o

server.o: server.c list.h
	gcc -Wall -c server.c

client_c.o: client_c.c list.h
	gcc -Wall -c client_c.c

client_l.o: client_l.c list.h
	gcc -Wall -c client_l.c

list.o: list.c list.h
	gcc -Wall -c list.c

clean:
	rm -f client_l client_c server *~
	rm -f *.o

