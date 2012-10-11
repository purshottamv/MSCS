CC = g++
CFLAGS =

all: sim_t sim_p sim_seq createmem

OBJS = sim_t.o InstructionDecode.o LoadReservationStation.o WriteBack.o

sim_t: sim_t.o InstructionDecode.o LoadReservationStation.o WriteBack.o
	$(CC) $(CFLAGS) $(OBJS) -o sim_t

sim_t.o: sim_t.cpp
	$(CC) -c $(CFLAGS) sim_t.cpp -o sim_t.o

InstructionDecode.o: InstructionDecode.cpp
	$(CC) -c $(CFLAGS) InstructionDecode.cpp -o InstructionDecode.o

LoadReservationStation.o: LoadReservationStation.cpp
	$(CC) -c $(CFLAGS) LoadReservationStation.cpp -o LoadReservationStation.o

WriteBack.o: WriteBack.cpp
	$(CC) -c $(CFLAGS) WriteBack.cpp -o WriteBack.o

sim_p: sim_p.o
	$(CC) $(CFLAGS) sim_p.o -o sim_p

sim_p.o: sim_p.cpp
	$(CC) -c $(CFLAGS) sim_p.cpp -o sim_p.o

sim_seq: sim_seq.o
	$(CC) $(CFLAGS) sim_seq.o -o sim_seq

sim_seq.o: sim_seq.cpp
	$(CC) -c $(CFLAGS) sim_seq.cpp -o sim_seq.o

createmem: createmem.o
	$(CC) $(CFLAGS) createmem.o -o createmem

createmem.o: createmem.cpp
	$(CC) -c $(CFLAGS) createmem.cpp -o createmem.o

clean:
	rm *.o
	rm sim_t
	rm sim_p
	rm sim_seq
	rm createmem