/**************  Matrix Multiplication using MPI ************/

#include "mpi.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define ROWS	1500
#define COLUMNS	1500
#define MASTER_PROCESS 0
#define ELEMENT_VALUE 1
#define WORKER_beginRow 1
#define WORKER_END 4

int mat1 [ROWS][COLUMNS]; // Parent Matrix
int mat2 [ROWS][COLUMNS]; // Parent Matrix
int res_mat [ROWS][COLUMNS]; // Resultant matrix
MPI_Status status;
MPI_Request request;

long seconds, useconds, mtime;
struct timeval start, end;
MPI_Status status;

// Initialize mat1 and mat2
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

int main(int argc,char **argv)
{
    int processID; // Holds The Process ID
    int totalProcesses; // Holds Number Of Processes
    int noOfRows; // Rows Alloted To Each Worker For Computation
    int beginRow, endRow, i,j,k;

    int offset = 0;
   
    MPI_Init(&argc, &argv); // Initialize the MPI execution environment
    MPI_Comm_rank(MPI_COMM_WORLD, &processID); // Determine the rank of the calling process in the communicator
    MPI_Comm_size(MPI_COMM_WORLD, &totalProcesses); // Return the size of the group associated with a communicator

    /**MASTER Process - 1. Initializes The Parent and Resultant Matrices,
                         2. Calculates Matrix Multiplication Time By a Single Process,
                        3. Dispatch JOBS to all other Non-Master Processes,
                        4. Waits For All Other Non-Master Processes To Finish The Job.**/
    if(processID == MASTER_PROCESS)
    {
        // Initializing mat1 and mat2
        fillmatrix();
		// -----------------------------------------------------------
		printf("Sequential Execution :");
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
			printf(" %ld milliseconds\n", mtime);
		// -----------------------------------------------------------
		printf("Execution in OpenMPI : ");
		// -----------------------------------------------------------
		fillmatrix();		// Initialize again
		gettimeofday(&start, NULL);
       
        // Assigning jobs to all the worker processes.
        for(i = 1; i<totalProcesses; i++)
        {
            noOfRows = ROWS/(totalProcesses-1); // Rows to Each Process
            beginRow = (i - 1) * noOfRows;
            if (((i + 1) == totalProcesses) && ((ROWS % (totalProcesses - 1)) != 0)) {
                endRow = ROWS;
            } else {
                endRow = beginRow + noOfRows;
            }
            MPI_Isend(&beginRow, 1, MPI_INT, i, WORKER_beginRow, MPI_COMM_WORLD, &request);  //Starts a standard-mode, nonblocking send. 
            MPI_Isend(&endRow, 1, MPI_INT, i, WORKER_beginRow + 1, MPI_COMM_WORLD, &request);  // 
            MPI_Isend(&mat1[beginRow][0], (endRow - beginRow) * COLUMNS, MPI_INT, i, WORKER_beginRow + 2, MPI_COMM_WORLD, &request);
        }
    }
    MPI_Bcast(&mat2, ROWS*COLUMNS, MPI_INT, 0, MPI_COMM_WORLD);
    // Each Worker Process does its assigned job
    if (processID > 0)
    {
        MPI_Recv(&beginRow, 1, MPI_INT, 0, WORKER_beginRow, MPI_COMM_WORLD, &status);
        MPI_Recv(&endRow, 1, MPI_INT, 0, WORKER_beginRow + 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&mat1[beginRow][0], (endRow - beginRow) * COLUMNS, MPI_INT, 0, WORKER_beginRow + 2, MPI_COMM_WORLD, &status);
        for (i = beginRow; i < endRow; i++)
        {
            for (j = 0; j < COLUMNS; j++)
            {
                for (k = 0; k < ROWS; k++)
                {
                    res_mat[i][j] += mat1[i][k] * mat2[k][j];
                }
            }
        }
        MPI_Isend(&beginRow, 1, MPI_INT, 0, WORKER_END, MPI_COMM_WORLD, &request);
        MPI_Isend(&endRow, 1, MPI_INT, 0, WORKER_END + 1, MPI_COMM_WORLD, &request);
        MPI_Isend(&res_mat[beginRow][0], (endRow - beginRow) * COLUMNS, MPI_INT, 0, WORKER_END + 2, MPI_COMM_WORLD, &request);
    }

    // Master process is waiting for all other worker processes
    if (processID == 0)
    {
        for (i = 1; i < totalProcesses; i++)
        {
            MPI_Recv(&beginRow, 1, MPI_INT, i, WORKER_END, MPI_COMM_WORLD, &status);
            MPI_Recv(&endRow, 1, MPI_INT, i, WORKER_END + 1, MPI_COMM_WORLD, &status);
            MPI_Recv(&res_mat[beginRow][0], (endRow - beginRow) * COLUMNS, MPI_INT, i, WORKER_END + 2, MPI_COMM_WORLD, &status);
        }
        gettimeofday(&end, NULL);
		seconds  = end.tv_sec  - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;
		mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
        printf("%ld ms\n", mtime);
    }

    MPI_Finalize();
}