CC = g++
OPTS = -std=c++0x
DEBUG = -D_TRACE -g

EXECUTABLES = server client

all: $(EXECUTABLES)

client: client.o
	$(CC) $(OPTS) -o client client.o -lpthread

server: Socket.o Server.o traceroute_server.o Logger.o
	$(CC) $(OPTS) -o server Socket.o Server.o traceroute_server.o Logger.o -lpthread

client.o: client.cpp
	$(CC) $(OPTS) -c client.cpp
#  For Trace mode Uncomment the below line and comment the above line
#	$(CC) $(OPTS) -c client.cpp $(DEBUG)

Socket.o: Socket.cpp
	$(CC) $(OPTS) -c Socket.cpp
#  For Trace mode Uncomment the below line and comment the above line
#	$(CC) $(OPTS) -c Socket.cpp $(DEBUG)

Server.o: Server.cpp
	$(CC) $(OPTS) -c Server.cpp
#  For Trace mode Uncomment the below line and comment the above line
#	$(CC) $(OPTS) -c Server.cpp $(DEBUG)

traceroute_server.o: traceroute_server.cpp
	$(CC) $(OPTS) -c traceroute_server.cpp
#  For Trace mode Uncomment the below line and comment the above line
#	$(CC) $(OPTS) -c traceroute_server.cpp $(DEBUG)

Logger.o: Logger.cpp
	$(CC) $(OPTS) -c Logger.cpp
#  For Trace mode Uncomment the below line and comment the above line
#	$(CC) $(OPTS) -c Logger.cpp $(DEBUG)

clean:
	rm -f *.o $(EXECUTABLES)