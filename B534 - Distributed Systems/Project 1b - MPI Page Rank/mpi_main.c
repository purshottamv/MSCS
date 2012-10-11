/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*   MPI PageRank															*/
/*   B534 project1 - Part 2 - Page Rank Algorithm Using MPI 				*/
/*   Group: 26       														*/
/*   Authors: Purshottam Vishwakarma										*/
/*   Emails: pvishwak@indiana.edu											*/
/*   Date:	2/7/2012														*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>             /* strtok() */
#include <sys/types.h>          /* open() */
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>             /* getopt() */
#include <mpi.h>

#include "pagerank.h"

int _debug,_help;

int main(int argc, char **argv)
{
    /* Definition of data structure and variables for MPI PageRank */ 
    int numUrls = 0, totalNumUrls = 0; 
	int _outputtimings = 1;
	char *filename = NULL; 
	char *outputfilename = "pagerank.output";
    char *format = "mpirun -np 10 ./mpi_main -i pagerank.input -t 0.001 -n 10 -o pagerank.output\n\nRun \"mpi_main -h\" for more information. \n ";
	int **am_index; /* int[numUrls][2] */ 
                    /* am_index[i][0]refers to second index for am,  am_index[i][1] refers to length of target urls list */ 
    int *adjacency_matrix;      /* [numTotalWebPages] */ 
    double *rank_values_table;  /* [numUrls] */ 
    double threshold = 0.0; 
	long seconds = 0, useconds = 0, mtime = 0, input_time = 0, output_time = 0;
	struct timeval start_time, end_time, start_file_io, end_file_io;
   
    int rank = 0, nproc, mpi_namelen,num_iterations = 0,i,j; 
    char mpi_name[MPI_MAX_PROCESSOR_NAME],*argument; 
	
	int mpi_pagerank(int *adjacency_matrix, 
		int **am_index, 
		int numUrls, 
		int totalNumUrls,
		int num_iterations, 
		double threshold, 
		double *rank_values_table,
		MPI_Comm comm);

	/* MPI Initialization */ 
    MPI_Init(&argc, &argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &nproc); 
    MPI_Get_processor_name(mpi_name, &mpi_namelen); 

	_debug = 0;
	_help = 0;
    /* Parse command line arguments */ 
	if(rank == 0)
		gettimeofday(&start_time, NULL);
	for(i=1;i<argc;i++)
	{
		if((strlen(argv[i]) == 2) && (strncmp(argv[i],"-",1) == 0))
		{
			j = (int)(argv[i][1]);
			switch(j)
			{
				case 105	: filename = argv[++i];			// '-i'
								if(_debug == 1 && rank == 0)
									printf("Input filename = %s\n",filename);
								break;
				case 110	: num_iterations = atoi(argv[++i]);	// '-n'
								if(_debug == 1 && rank == 0)
									printf("max_iterations = %d\n",num_iterations);
								break;
				case 116	: threshold = atof(argv[++i]);	// '-t'
								if(_debug == 1 && rank == 0)
									printf("threshold = %f\n",threshold);
								break;
				case 111	: _outputtimings = 1;	// '-o'
								if(_debug == 1 && rank == 0)
									printf("Output Timing results = Yes\n");
								break;
				case 100	: _debug = 1;					// '-d'
								if(_debug == 1 && rank == 0)
									printf("debug flag = %d\n",_debug);
								break;
				case 104	: _help = 1;				// '-h'
								if(_debug == 1 && rank == 0)
									printf("Help mode = %d\n",_help);
								break;
				default		: if(rank == 0) 
									printf("Invalid Command line argument : The format for running the program is\n%s\n",format);
								MPI_Finalize();
								exit(1);
			}
		}
	}

	if(_help ==1)
	{
		if(rank ==0)
		{
			printf("Use the below format to run the file\n%s\n",format);
			printf("-d : debug mode\n");
			printf("-n : Number of iterations. If not specified, default = 10\n");
			printf("-t : Threshold Value. If not specified default = 0.001\n.");
			printf("-i : Input File\n");
			printf("-o : Output Timing Results. (default = yes)\n");
			printf("-h : Help\n");
		}
        MPI_Finalize();
		exit(1);
	}
	if(filename == NULL)
	{
		if(rank==0)
			printf("Error!! Please provide the input file name.\nThe format for running the program is\n%s\n",format);
        MPI_Finalize();
		exit(1);
	}
	if(num_iterations == 0)
	{	
		if(rank ==0)
			printf("Using Default num_iterations = 10\nUse -n option to provide number of iterations\n");
		num_iterations = 10;
	}
	if(threshold == 0)
	{	
		if(rank ==0)
			printf("Using Default Threshold = 0.001\nUse -t option to provide threshold value\n");
		threshold = 0.001;
	}
	
	/* Read local adjacency matrix from file for each process */ 
	if(_debug == 1)
		printf("\nProc %d : Starting MPI Page Rank.\n",rank);
	
	if(rank == 0)
		gettimeofday(&start_file_io, NULL);
	mpi_read(filename, &numUrls, &am_index, &adjacency_matrix, MPI_COMM_WORLD);
	if(rank ==0)
	{
		gettimeofday(&end_file_io, NULL);
		seconds  = end_file_io.tv_sec  - start_file_io.tv_sec;
		useconds = end_file_io.tv_usec - start_file_io.tv_usec;
		input_time = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	}
    /* Set totalNumUrls */ 
	/* MPI_Allreduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm )  */
	MPI_Allreduce(&numUrls, &totalNumUrls, 1, MPI_INT, MPI_SUM,MPI_COMM_WORLD); 

	if(_debug == 1)
	{
		printf("Rank %d numUrls = %d\n",rank,numUrls);
		printf("Rank %d totalNumUrls = %d\n",rank,totalNumUrls);
	}
    /* Global page rank value table */ 
    rank_values_table = (double *) malloc(totalNumUrls * sizeof(double)); 
    assert(rank_values_table != NULL); 
 
    /* Root(rank 0) computes the initial rank value for each web page */ 
	if(rank == 0)
	{
		i = 0;
		while(i < totalNumUrls)
		{
			rank_values_table[i] = 1/(double)totalNumUrls;
			i++;
		}
	}

    /* Broadcast the initial rank values to all other compute nodes */ 
	/* MPI_Bcast( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm ) */
	MPI_Bcast(rank_values_table, totalNumUrls, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /* Start the core computation of MPI PageRank */ 
    mpi_pagerank(adjacency_matrix, am_index, numUrls, totalNumUrls, 
                 num_iterations, threshold, rank_values_table, 
                 MPI_COMM_WORLD);
 
    /* Save results to a file */ 
	if(rank == 0)
	{
		gettimeofday(&start_file_io, NULL);
		
		mpi_write(outputfilename, totalNumUrls,rank_values_table,MPI_COMM_WORLD);
		
		gettimeofday(&end_file_io, NULL);
		seconds  = end_file_io.tv_sec  - start_file_io.tv_sec;
		useconds = end_file_io.tv_usec - start_file_io.tv_usec;
		output_time = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	}

	/* Release resources */ 
	free(adjacency_matrix);

	if(_debug == 1)
		printf("Freeing am_index\n");
	free(am_index[0]);
	free(am_index);
	free(rank_values_table);
	if(_debug == 1)
		printf("Rank %d : Calling MPI_Finalize\n",rank);

	MPI_Finalize();
	if(rank == 0)
	{
		gettimeofday(&end_time, NULL);
		seconds  = end_time.tv_sec  - start_time.tv_sec;
		useconds = end_time.tv_usec - start_time.tv_usec;
		mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	//	printf("%ld ms\n", mtime);

		printf("  **** MPI PageRank **** \n");
		printf("Num of processes		= %d\n",nproc);
		printf("Input file			= %s\n",filename);
		printf("Output file			= %s\n",outputfilename);	
		printf("TotalNumUrls			= %d\n",totalNumUrls);
		printf("num_iterations			= %d\n",num_iterations);
		printf("threshold			= %f\n",threshold);
		if(_outputtimings)
		{
			printf("I/O time			= %ldms\n",input_time+output_time);
			printf("Computation time(including I/O time) = %ldms\n",mtime);
			printf("Computation time(excluding I/O time) = %ldms\n",mtime-input_time-output_time);
		}
	}

	return (0); 
}
