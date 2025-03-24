all:	server clientV clientG 

server:	server.o list.o
	gcc -Wall -o server server.o list.o

clientV:	client_v.o list.o
	gcc -Wall -o client_v client_v.o list.o

clientG:	client_g.o list.o
	gcc -Wall -o client_g client_g.o list.o

server.o: server.c list.h
	gcc -Wall -c server.c

client_v.o: client_v.c list.h
	gcc -Wall -c client_v.c

client_g.o: client_g.c list.h
	gcc -Wall -c client_g.c

list.o: list.c list.h
	gcc -Wall -c list.c

clean:
	rm -f client_v client_g server *~
	rm -f *.o
