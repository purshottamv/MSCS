CC = g++
CFLAGS = -lpapi
SOURCEFILE = 

all: matrix_block datalayout 

OBJS = 

matrix_block: matrix_block.o
	$(CC) $(CFLAGS) -o matrix_block matrix_block.o

matrix_block.o: matrix_block.cpp
	$(CC) -c $(CFLAGS) matrix_block.cpp

datalayout: datalayout.o
	$(CC) $(CFLAGS) -o datalayout datalayout.o

datalayout.o: datalayout.cpp
	$(CC) -c $(CFLAGS) datalayout.cpp

clean:
	rm *.o
	rm matrix_block
	rm datalayout