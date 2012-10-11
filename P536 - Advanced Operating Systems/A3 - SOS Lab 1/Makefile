CC = gcc
CFLAGS = -g 
SOSDIR = /usa/swany/sos
#GTYPE = 
GTYPE = $(shell $(SOSDIR)/bin/config.guess)
INCLUDES = -I$(SOSDIR)/include
LIB = $(SOSDIR)/lib/$(GTYPE)/libsim.a $(SOSDIR)/lib/$(GTYPE)/libmt.a $(SOSDIR)/lib/$(GTYPE)/libfdr.a -lnsl
MAIN = $(SOSDIR)/lib/$(GTYPE)/main_lab1.o

EXECUTABLES = sos
USER_OBJS   = exception.o sos.o

all: $(EXECUTABLES)

sos: $(USER_OBJS) $(MAIN) 
	$(CC) $(CFLAGS) -o sos $(MAIN) $(USER_OBJS) $(LIB)

.c.o: 
	$(CC) $(CFLAGS) $(INCLUDES) -c $*.c

clean:
	/bin/rm -f *.o core $(EXECUTABLES) 

