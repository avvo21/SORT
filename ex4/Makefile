all:	server clientC clientF 

server:	server.o list.o
	gcc -Wall -o server server.o list.o

clientC:	client_c.o list.o
	gcc -Wall -o client_c client_c.o list.o

clientF:	client_f.o list.o
	gcc -Wall -o client_f client_f.o list.o

server.o: server.c list.h
	gcc -Wall -c server.c

client_c.o: client_c.c list.h
	gcc -Wall -c client_c.c

client_l.o: client_f.c list.h
	gcc -Wall -c client_f.c

list.o: list.c list.h
	gcc -Wall -c list.c

clean:
	rm -f client_c client_f server *~
	rm -f *.o

