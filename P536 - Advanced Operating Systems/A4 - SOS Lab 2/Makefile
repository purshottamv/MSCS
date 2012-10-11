# Makefile for SOS 

CC = gcc
CFLAGS = -g 
SOSDIR = /usa/swany/sos
GTYPE = $(shell $(SOSDIR)/bin/config.guess)
INCLUDES = -I$(SOSDIR)/include
LIB = $(SOSDIR)/lib/$(GTYPE)/libsim.a $(SOSDIR)/lib/$(GTYPE)/libmt.a $(SOSDIR)/lib/$(GTYPE)/libfdr.a  
MAIN = $(SOSDIR)/lib/$(GTYPE)/main_lab2.o

EXECUTABLES = sos
USER_OBJS   = exception.o sos.o sched.o 

all: $(EXECUTABLES)

sos: $(USER_OBJS) $(MAIN) $(LIB) 
	$(CC) $(CFLAGS) -o sos $(MAIN) $(USER_OBJS) $(LIB) 

.c.o: 
	$(CC) $(CFLAGS) $(INCLUDES) -c $*.c

clean:
	/bin/rm -f *.o core $(EXECUTABLES) 

