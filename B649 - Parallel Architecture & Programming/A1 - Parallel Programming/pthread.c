/******************************************************************************
* FILE: matrix.c
* DESCRIPTION:
*   Comparing the sequential programming and Multithreaded Parallel Programming Model
*   using Matrix Multiplication
* AUTHOR: Purshottam Vishwakarma
* LAST REVISED: 09/22/11
******************************************************************************/
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define ROWS	1500
#define COLUMNS	1500
#define ELEMENT_VALUE 1

int mat1[ROWS][COLUMNS],mat2[ROWS][COLUMNS],res_mat[ROWS][COLUMNS];

void fillmatrix()
{
   int i,j;
	for(i=0;i<ROWS;i++)
		for(j = 0;j<COLUMNS;j++)
		{
			mat1[i][j] = ELEMENT_VALUE;
			mat2[i][j] = ELEMENT_VALUE;
			res_mat[i][j] = 0;
		}
}

void *multiply(void *r)
{
	long row = (long)r;
	int j=0,k=0;
	for(j=0;j<COLUMNS;j++)
		for(k=0;k<COLUMNS;k++)
			res_mat[row][j] = res_mat[row][j] + (mat1[row][k]*mat2[k][j]);
   pthread_exit((void *)r);
}
int main(int argc, char *argv[])
{
	pthread_t threads[ROWS];
   
	int rc = 0;
	int i,j,k;
	long t;
	long row;
	pthread_attr_t attr;
	void *status;
	size_t stacksize;
	time_t t1;
	time_t t2;
	
    struct timeval start, end;
	long mtime, seconds, useconds;

	fillmatrix();

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	gettimeofday(&start, NULL);

	for(t=0;t<ROWS;t++)
	{
		//printf("In main: creating thread %ld\n", t);
		rc = pthread_create (&threads[t], &attr, multiply, (void *)t);
		if (rc)
		{
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

  /* Free attribute and wait for the other threads */
   pthread_attr_destroy(&attr);
   for(t=0; t<ROWS; t++) {
      rc = pthread_join(threads[t], &status);
      if (rc) {
         printf("ERROR; return code from pthread_join() is %d\n", rc);
         exit(-1);
         }
      //printf("Main: completed join with thread %ld having a status of %ld\n",t,(long)status);
      }

	gettimeofday(&end, NULL);
	seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("pthread Execution time: %ld milliseconds\n", mtime);

// -----------------------------------------------------------
	gettimeofday(&start, NULL);
	for(i=0;i<ROWS;i++)
	{
		for(j=0;j<COLUMNS;j++)
		{
			for(k=0;k<COLUMNS;k++)
				res_mat[i][j] = res_mat[i][j] + (mat1[i][k]*mat2[k][j]);
		}
	}

	gettimeofday(&end, NULL);
	seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Sequential Execution time: %ld milliseconds\n", mtime);
//	-------------------------------------------------------

   /* Last thing that main() should do */
   pthread_exit(NULL);
}