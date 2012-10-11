# Makefile for P573 Assignment 1

CC = g++
OPTS = -O0

go: elapsedtime.o timing.o
	$(CC) $(OPTS) -o go elapsedtime.o timing.o -L/usr/lib -lrt -lm
elapsedtime.o: elapsedtime.c
	$(CC) $(OPTS) -c elapsedtime.c
timing.o: timing.cpp
	$(CC) $(OPTS) -c timing.cpp
#  For Trace mode Uncomment the below line and comment the above line
#	$(CC) $(OPTS) -c timing.cpp -D_TRACE
clean:
	rm -f elapsedtime.o timing.o go