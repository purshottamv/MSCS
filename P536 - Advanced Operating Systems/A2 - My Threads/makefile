CC = gcc
CFLAGS = -g -D _DEBUG
SOURCEFILE = mythread.c

all: dphil test_mt test_mtd dphild

dphil: mythread.o dphil_skeleton.o
	$(CC) mythread.o dphil_skeleton.o -o dphil

test_mt: mythread.o test_mt.o
	$(CC) mythread.o test_mt.o -o test_mt

test_mtd: mythreadd.o test_mtd.o
	$(CC) $(CFLAGS) mythreadd.o test_mtd.o -o test_mtd

dphild: mythreadd.o dphil_skeletond.o
	$(CC) $(CFLAGS) mythreadd.o dphil_skeletond.o -o dphild

mythread.o: mythread.c
	$(CC) -c  mythread.c

test_mt.o: test_mt.c
	$(CC) -c  test_mt.c

dphil_skeleton.o: dphil_skeleton.c
	$(CC) -c  dphil_skeleton.c

dphil_skeletond.o: dphil_skeleton.c
	$(CC) -c $(CFLAGS) dphil_skeleton.c -o dphil_skeletond.o

mythreadd.o: mythread.c
	$(CC) -c $(CFLAGS) mythread.c -o mythreadd.o

test_mtd.o: test_mt.c
	$(CC) -c $(CFLAGS) test_mt.c -o test_mtd.o

clean:
	rm *.o
	rm dphil
	rm test_mt
	rm dphild
	rm test_mtd