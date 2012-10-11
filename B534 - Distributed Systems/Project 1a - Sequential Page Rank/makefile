CC = g++
OPTS = -std=c++0x

EXECUTABLES = seqpagerank

all: $(EXECUTABLES)

seqpagerank: seqpagerank.o WebURL.o
	$(CC) $(OPTS) -o seqpagerank seqpagerank.o WebURL.o

seqpagerank.o: seqpagerank.cpp
	$(CC) $(OPTS) -c seqpagerank.cpp
#  For Trace mode Uncomment the below line and comment the above line
#	$(CC) $(OPTS) -c seqpagerank.cpp -D_TRACE

WebURL.o: WebURL.cpp
	$(CC) $(OPTS) -c WebURL.cpp
#  For Trace mode Uncomment the below line and comment the above line
#	$(CC) $(OPTS) -c WebURL.cpp -D_TRACE

clean:
	rm -f *.o seqpagerank