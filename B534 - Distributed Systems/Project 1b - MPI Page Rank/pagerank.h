#ifndef _H_PAGERANK
#define _H_PAGERANK

#include <assert.h>
#include <mpi.h>

int     file_read(char*, int*, int***, int**);
int     mpi_pagerank(int*, int**, int, int, int, double, double*, MPI_Comm);
int     mpi_read(char*/*filename*/, int*/*numUrls*/, int***/*am_index*/, int**/*adjacency_matrix*/,MPI_Comm);
int     mpi_write(char*, int, double*,  MPI_Comm);

extern int _debug;
extern int _help;

#endif
