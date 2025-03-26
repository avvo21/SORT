CC=gcc

main: main.o fast_trigo.o 
	$(CC) -o main main.o fast_trigo.o -lm

main.o: main.c fast_trigo.h
	$(CC) -c main.c 

fast_trigo.o: fast_trigo.c fast_trigo.h
	$(CC) -c fast_trigo.c

clean:
	rm -f *.o
	rm -f main