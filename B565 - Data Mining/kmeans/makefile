CC = g++
OPTS = -std=c++0x

EXECUTABLES = kmeans

all: $(EXECUTABLES)

kmeans: kmeans.o BreastCancerRecord.o
	$(CC) $(OPTS) -o kmeans kmeans.o BreastCancerRecord.o -g

kmeans.o: kmeans.cpp
	$(CC) $(OPTS) -c kmeans.cpp
#  For Trace mode Uncomment the below line and comment the above line
#	$(CC) $(OPTS) -c kmeans.cpp -D_TRACE -g

BreastCancerRecord.o: BreastCancerRecord.cpp
	$(CC) $(OPTS) -c BreastCancerRecord.cpp
#  For Trace mode Uncomment the below line and comment the above line
#	$(CC) $(OPTS) -c BreastCancerRecord.cpp -D_TRACE -g

clean:
	rm -f *.o kmeans