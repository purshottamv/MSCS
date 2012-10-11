/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*   MPI PageRank															*/
/*   B534 project1 - Part 2 - Page Rank Algorithm in Parallel				*/
/*   Group: 26       														*/
/*   Authors: Purshottam Vishwakarma										*/
/*   Emails: pvishwak@indiana.edu											*/
/*   Date:	2/7/2012														*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "pagerank.h"

int mpi_pagerank(int *adjacency_matrix, int **am_index, int numUrls, int total_num_urls,int num_iterations, double threshold, double *rank_values_table,MPI_Comm comm)
{
    /* Definitions of variables */ 
	double *local_rank_values_table,*old_rank_values_table,first_term, old;
	double dangling,sum_dangling;
	int rank, loop = 0,i=0,j,outbound_node,number_of_outbounds,root_node, index_val = 0, primary_node;
	double delta = 0.0,diff = 0.0;

    /* Get MPI rank */ 
    MPI_Comm_rank(comm, &rank); 
	first_term = (double)(1-0.85)/(double)total_num_urls;
    /* Allocate memory and initialize values for local_rank_values_table */ 
    local_rank_values_table = (double *) malloc(total_num_urls * sizeof(double)); 
	old_rank_values_table = (double *) malloc(total_num_urls * sizeof(double)); 
    assert(local_rank_values_table != NULL); 

	if(rank != 0)  // Adjust the am_index[i][0] values
	{
		for(i=0; i < numUrls ; i++)
		{
			am_index[i][0] = index_val;
			index_val += am_index[i][1];
		}
	}

    /* Start computation loop */ 
    do 
    { 
 		if(_debug == 1 && rank !=0)
			printf("Rank %d : Computing PageRank for iteration %d\n",rank,loop);
		/* Compute pagerank and dangling values */
		dangling = 0.0;
		sum_dangling = 0.0;
		i=0;
		while(i < total_num_urls)
		{
			old_rank_values_table[i] = rank_values_table[i];
			local_rank_values_table[i++] = 0;
		}

		for(primary_node=0; primary_node < numUrls ; primary_node++)
		{
			root_node = adjacency_matrix[am_index[primary_node][0]];
			number_of_outbounds =  am_index[primary_node][1];
	 		if(_debug == 1 && rank !=0)
				printf("Rank %d : src_url:%d cur_val : %.3f with %d outbound urls\n",rank,root_node,rank_values_table[root_node],number_of_outbounds-1);
			if(number_of_outbounds == 1)
			{
				dangling = dangling + rank_values_table[root_node]; 
	 			if(_debug == 1)
					printf("Rank %d : dangling = %f After adding %f\n",rank,dangling,rank_values_table[root_node]);
			}
			else
			{
				for(j = 1;j < number_of_outbounds;j++)
				{
					outbound_node = adjacency_matrix[am_index[primary_node][0] + j];
					old = local_rank_values_table[outbound_node];
					local_rank_values_table[outbound_node] = local_rank_values_table[outbound_node] +
						(double)(rank_values_table[root_node] / (double)(number_of_outbounds - 1));
		 			if(_debug == 1)
						printf("\tRank %d : Target_url : %d. current_iteration_val: %f  Added %f\n",rank,outbound_node, old,
						(double)(rank_values_table[root_node] / (double)(number_of_outbounds - 1)));
				}
			}
 			if(_debug == 1)
				printf("\n");
		}

        /* Distribute pagerank values */ 
	    /* MPI_Allreduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm )  */
		MPI_Allreduce(local_rank_values_table, rank_values_table, total_num_urls, MPI_DOUBLE, MPI_SUM, comm); 

        /* Distribute dangling values */ 
        MPI_Allreduce(&dangling, &sum_dangling, 1, MPI_DOUBLE, MPI_SUM, comm); 
         

		/* Recalculate the page rank values with damping factor 0.85 */ 
		sum_dangling = sum_dangling / (double)total_num_urls;

		if(_debug == 1 && rank !=0)
			printf("Rank %d : Addition of Page Ranks of all Dangliing Nodes = %f\n",rank,sum_dangling);

		delta = 0.0;
		diff = 0.0;
		for(i=0;i<total_num_urls;i++)
		{
			rank_values_table[i] += sum_dangling;
			rank_values_table[i] = first_term + 0.85 *(rank_values_table[i]);
			if(rank ==0)
			{
				/* Root(process 0) computes delta to determine to stop or continue */ 
				diff = old_rank_values_table[i] -  rank_values_table[i];
				delta = delta + (diff*diff);
				if(_debug == 1)
					printf("Rank %d : After Iteration %d : Final rank_values_table[%d] = %f\n",rank,loop,i,rank_values_table[i]);
			}
		}
		/* Root broadcasts delta */ 
		MPI_Bcast(&delta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}
	while (delta > threshold && ++loop < num_iterations); 
	free(local_rank_values_table);
	free(old_rank_values_table);
	if(_debug)
		printf("Rank %d : Returning to mpi_main\n",rank);
	return 1;   
}