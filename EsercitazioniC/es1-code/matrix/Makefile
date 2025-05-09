CC = g++
CFLAGS = -O3 -Wall -pthread -std=c++11
LFLAGS = -pthread

OUT = matrix_serial matrix_threads

all : $(OUT)

matrix_%: matrix_%.o
	$(CC) -o $@ $< $(LFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	rm -f *.o *~ $(OUT)


