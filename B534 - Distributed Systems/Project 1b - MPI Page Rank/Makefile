
.KEEP_STATE:

all: mpi

DFLAGS      =
OPTFLAGS    = -O -NDEBUG
OPTFLAGS    = -g
INCFLAGS    = -I.
CFLAGS      = $(OPTFLAGS) $(DFLAGS) $(INCFLAGS)
LDFLAGS     = $(OPTFLAGS)
LIBS        =


CC          = gcc
MPICC       = mpicc

.c.o:
	$(CC) $(CFLAGS) -c $<

H_FILES     = pagerank.h

MPI_SRC     = mpi_main.c   \
              mpi_pagerank.c \
              mpi_io.c

MPI_OBJ     = $(MPI_SRC:%.c=%.o)

mpi_main.o: mpi_main.c $(H_FILES)
	$(MPICC) $(CFLAGS) -c $*.c

mpi_pagerank.o: mpi_pagerank.c $(H_FILES)
	$(MPICC) $(CFLAGS) -c $*.c

mpi_io.o: mpi_io.c $(H_FILES)
	$(MPICC) $(CFLAGS) -c $*.c

mpi: mpi_main
mpi_main: $(MPI_OBJ) $(H_FILES)
	$(MPICC) $(LDFLAGS) -o mpi_main $(MPI_OBJ) $(LIBS)

clean:
	rm -rf *.o mpi_main
