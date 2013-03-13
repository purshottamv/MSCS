CC = g++
OPTS = -std=c++0x -lpcap
DEBUG = -D_TRACE -g

EXECUTABLES = wt
EXECUTABLES_DEBUG = wtd

all: $(EXECUTABLES) $(EXECUTABLES_DEBUG)

$(EXECUTABLES): wiretap.o Logger.o PacketInfoCollector.o
	$(CC) $(OPTS) -o $(EXECUTABLES) wiretap.o Logger.o PacketInfoCollector.o

$(EXECUTABLES_DEBUG): wiretap_d.o Logger_d.o PacketInfoCollector_d.o
	$(CC) $(OPTS) -o $(EXECUTABLES_DEBUG) wiretap_d.o Logger_d.o PacketInfoCollector_d.o

wiretap_d.o: wiretap.cpp
	$(CC) $(OPTS) -c wiretap.cpp -o wiretap_d.o $(DEBUG)

wiretap.o: wiretap.cpp
	$(CC) $(OPTS) -c wiretap.cpp -o wiretap.o

PacketInfoCollector_d.o: PacketInfoCollector.cpp
	$(CC) $(OPTS) -c PacketInfoCollector.cpp -o PacketInfoCollector_d.o $(DEBUG)

PacketInfoCollector.o: PacketInfoCollector.cpp
	$(CC) $(OPTS) -c PacketInfoCollector.cpp

Logger_d.o: Logger.cpp
	$(CC) $(OPTS) -c Logger.cpp -o Logger_d.o $(DEBUG)

Logger.o: Logger.cpp
	$(CC) $(OPTS) -c Logger.cpp

clean:
	rm -f *.o $(EXECUTABLES) output
