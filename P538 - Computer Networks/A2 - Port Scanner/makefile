CC = g++
OPTS = -std=c++0x -D_REENTRANT
DEBUG = -D_TRACE -g

EXECUTABLES = ps
EXECUTABLES_DEBUG = psd

all: $(EXECUTABLES)

$(EXECUTABLES): portscanner.o Logger.o
	$(CC) $(OPTS) -o $(EXECUTABLES) portscanner.o Logger.o -lpthread

$(EXECUTABLES_DEBUG): portscanner_d.o Logger_d.o
	$(CC) $(OPTS) -o $(EXECUTABLES_DEBUG) portscanner_d.o Logger_d.o -lpthread

portscanner_d.o: portscanner.cpp
	$(CC) $(OPTS) -c portscanner.cpp -o portscanner_d.o $(DEBUG)

portscanner.o: portscanner.cpp
	$(CC) $(OPTS) -c portscanner.cpp

Logger_d.o: Logger.cpp
	$(CC) $(OPTS) -c Logger.cpp -o Logger_d.o $(DEBUG)

Logger.o: Logger.cpp
	$(CC) $(OPTS) -c Logger.cpp

clean:
	rm -f *.o $(EXECUTABLES)
