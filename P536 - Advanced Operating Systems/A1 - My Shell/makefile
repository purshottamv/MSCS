CC = gcc
CFLAGS = -g
SOURCEFILE = myshell.c

all: myshell

OBJS = myshell.o

myshell: myshell.o
	$(CC) $(CFLAGS) -o myshell $(OBJS)

myshell.o: myshell.c
	$(CC) -c $(CFLAGS) $(SOURCEFILE)
clean:
	rm *.o
	rm myshell