/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*   MPI PageRank							     */
/*   CGL Indiana University						     */
/*   Author: Hui Li							     */
/*   Email: lihui@indiana.edu					 	     */
/*     								             */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          /* open() */
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>             /* read(), close() */

#include <mpi.h>
#include "pagerank.h"

#define MAX_CHAR_PER_LINE 128

/* 
 * Read an adjacency matrix file and assign a local matrix to each process
 * Note
 * -. After this call, each process will have its own adjacency matrix
 * -. Each process will have roughly eqaul amount of number of urls
 */
int mpi_read(char *filename,    /* (IN) file name */
             int *numUrls,      /* (OUT) number of urls assigned to local machine */
			 int ***am_index,   /* (OUT) index array for adjacency matrix, assigned to local machine */
             int **adjacency_matrix,    /* (OUT) adjacency matrix assigned to local machine */
             MPI_Comm comm)     /* (IN) MPI communicator */
{

    int i, j, k, len, divide, rem;
    int rank, nproc;
    int total_num_urls;
    MPI_Status status;

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &nproc);

    if (rank == 0)
    {

        int result = file_read(filename, &total_num_urls, am_index,
                               adjacency_matrix);

        if (result == -1)
        {
            MPI_Finalize();
            exit(1);
        }
    }

    /* broadcast global numUrls to the rest proc */
    MPI_Bcast(&total_num_urls, 1, MPI_INT, 0, comm);

    divide = (total_num_urls) / nproc;
    rem = (total_num_urls) % nproc;

    if (rank == 0)
    {
        int msg_size;
        int block_size;
        int cur_index = (rem > 0) ? divide + 1 : divide;
        int send_start_index;   // = (*am_index)[cur_index][0];

        (*numUrls) = cur_index;

        /* scatter adjacency_matrix[total_num_urls] to other processes */
        for (i = 1; i < nproc; i++)
        {
            send_start_index = (*am_index)[cur_index][0];
            block_size = (i < rem) ? (divide + 1) : divide;
            msg_size = 0;
            for (k = 0; k < block_size; k++)
            {
                msg_size += (*am_index)[cur_index][1];
                cur_index++;
            }
			/* MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) */
			MPI_Send(&msg_size, 1, MPI_INT, i, 0, comm);
            MPI_Send((*adjacency_matrix) + send_start_index, msg_size,
                     MPI_INT, i, 1, comm);
        }

        /* scatter am_index[total_num_urls][2] to other processes */
        block_size = (rem > 0) ? divide + 1 : divide;
        int *ptr = (*am_index)[0] + block_size * 2;
        for (i = 1; i < nproc; i++)
        {
            block_size = (i < rem) ? divide + 1 : divide;
            MPI_Send(ptr, block_size * 2, MPI_INT, i, 2, comm);
            ptr += block_size * 2;
        }

        /* reduce the adjacency_matrix[total_num_urls] to adjacency_matrix[local_num_urls] */
        block_size = (rem > 0) ? divide + 1 : divide;
        msg_size = 0;
        for (i = 0; i < block_size; i++)
            msg_size += (*am_index)[i][1];

        (*adjacency_matrix) =
            realloc(*adjacency_matrix, msg_size * sizeof(int));
        assert(*adjacency_matrix != NULL);

		/* reduce the am_index[total_num_urls][2] to am_index[local_num_urls][2] */
        (*am_index)[0] =
            realloc((*am_index)[0], block_size * 2 * sizeof(int));
        assert((*am_index)[0] != NULL);
    }
    else
    {
        /* local numUrls */
        (*numUrls) = (rank < rem) ? divide + 1 : divide;

        int recv_msg_size;
		
		/*MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)  */
		MPI_Recv(&recv_msg_size, 1, MPI_INT, 0, 0, comm, &status);
        (*adjacency_matrix) = (int *) malloc(recv_msg_size * sizeof(int));

		MPI_Recv((*adjacency_matrix), recv_msg_size, MPI_INT, 0, 1, comm,
                 &status);

        (*am_index) = (int **) malloc((*numUrls) * sizeof(int *));
        assert((*am_index) != NULL);
        (*am_index)[0] = (int *) malloc((*numUrls) * 2 * sizeof(int));
        assert((*am_index)[0] != NULL);
        for (i = 1; i < *numUrls; i++)
            (*am_index)[i] = (*am_index)[i - 1] + 2;

        MPI_Recv((*am_index)[0], (*numUrls) * 2, MPI_INT, 0, 2, comm,
                 &status);
    }
    return 1;
}

/* Helper function for mpi_read */
int file_read(char *filename,   /* (IN) file name */
              int *total_num_urls,      /* (OUT) num of total urls */
              int ***am_index,  /* (OUT) adjacency matrix index */
              int **adjacency_matrix)   /* (OUT) adjacency matrix */
{

    int i, j, len;
    ssize_t numBytesRead;

    FILE *infile;
    char *line, *ret;
    int lineLen;
    int total_num_web_pages;

    if ((infile = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "Error: no such file (%s)\n", filename);
        return -1;
    }

    /* first find the number of adjacency_matrix */
    lineLen = MAX_CHAR_PER_LINE;
    line = (char *) malloc(lineLen);
    assert(line != NULL);

    (*total_num_urls) = 0;
    total_num_web_pages = 0;

    while (fgets(line, lineLen, infile) != NULL)
    {
        /* check each line to find the max line length */
        while (strlen(line) == lineLen - 1)
        {
            /* this line read is not complete */
            len = strlen(line);
            fseek(infile, -len, SEEK_CUR);

            /* increase lineLen */
            lineLen += MAX_CHAR_PER_LINE;
            line = (char *) realloc(line, lineLen);
            assert(line != NULL);

            ret = fgets(line, lineLen, infile);
            assert(ret != NULL);
        }

        if (strtok(line, " \t\n") != 0)
        {
            (*total_num_urls)++;
            total_num_web_pages++;
            while (strtok(NULL, " \t\n") != 0)
                total_num_web_pages++;
        }
    } //while

    if (_debug)
    {
        printf("File %s total_num_urls: %d\n", filename, *total_num_urls);
        printf("File %s total_num_web_pages: %d\n", filename,
               total_num_web_pages);
    }

    /* allocate memory for adjacency_matrix[] and read all adjacency_matrix items */
    (*adjacency_matrix) =
        (int *) malloc(total_num_web_pages * sizeof(int));
    if ((*adjacency_matrix) == NULL)
    {
        fprintf(stderr,
                "Error: failed to allocate memory for adjacency_matrix\n");
        return -1;
    }

    (*am_index) = (int **) malloc((*total_num_urls) * sizeof(int *));
    assert((*am_index) != NULL);
    (*am_index)[0] = (int *) malloc((*total_num_urls) * 2 * sizeof(int));

    if ((*am_index)[0] == NULL)
    {
        fprintf(stderr, "Error: failed to allocate memory for am_index\n");
        return -1;
    }

    for (i = 1; i < *total_num_urls; i++)
        (*am_index)[i] = (*am_index)[i - 1] + 2;

    i = 0;
    j = 0;
    int position;
    char *pch;
    rewind(infile);

    /* read all adjacency_matrix items, and construct am_index */
    while (fgets(line, lineLen, infile) != NULL)
    {
        pch = strtok(line, " \t\n");
        assert(pch != NULL);
        (*am_index)[j][0] = i;
        len = 0;
        while (pch != 0)
        {
            (*adjacency_matrix)[i] = atof(pch);
            i++;
            len++;
            pch = strtok(NULL, " \t\n");
        }
        (*am_index)[j][1] = len;
        j++;
    } //while

    fclose(infile);
    free(line);
    return 1;
}

/* 
 * Save rank_values_table[] to a file
 * Note
 * -. Only root (rank 0) will save rank values to a file
 * -. Assumed rank_values_table[] are the same across all proc 
 */
int mpi_write(char *filename,   /* (IN) file name */
              int totalNumUrls, /* (IN) number of total urls */
              double *rank_values_table,        /* (IN) array of rank values. double[total_num_urls] */
              MPI_Comm comm)    /* (IN) MPI communicator */
{

    int divide, rem, len, err;
    int i, j, k, rank, nproc;
    char outFileName[1024], fs_type[32], str[32];
    MPI_File fh,fhall;
    MPI_Status status;
	int *index;
	void quickSort(double arr[], int index[],int left, int right);

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &nproc);

    /* only proc 0 do this, because rank_values_table[] are the same across all proc */
    if (rank == 0)
    {
		index = (int *)malloc(totalNumUrls * sizeof(int));
        
		sprintf(outFileName, "%s.all",filename);
		printf("\nProc:%d is writing rank values of %d urls to file %s\n",
             rank, totalNumUrls,outFileName);
		printf("Proc:%d is writing top 10 page rank values to file %s\n",
             rank,filename);
        err = MPI_File_open(MPI_COMM_SELF, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
        if (err != MPI_SUCCESS)
        {
            char errstr[MPI_MAX_ERROR_STRING];
            int errlen;
            MPI_Error_string(err, errstr, &errlen);
            printf("Error at opening file %s (%s)\n", filename, errstr);
            MPI_Finalize();
            exit(1);
        }
        err = MPI_File_open(MPI_COMM_SELF, outFileName, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fhall);
        if (err != MPI_SUCCESS)
        {
            char errstr[MPI_MAX_ERROR_STRING];
            int errlen;
            MPI_Error_string(err, errstr, &errlen);
            printf("Error at opening file %s (%s)\n", outFileName, errstr);
            MPI_Finalize();
            exit(1);
        }

		for (i = 0; i < totalNumUrls; i++)
        {
            index[i] = i;
			memset(str,'\0',32);
			sprintf(str, "%d ", i);
            MPI_File_write(fhall, str, strlen(str), MPI_CHAR, &status);
            sprintf(str, "%f\n", rank_values_table[i]);
            MPI_File_write(fhall, str, strlen(str), MPI_CHAR, &status);
        }
        MPI_File_close(&fhall);
		// Sort the urls
		printf("Using quicksort to sort the top 10 Urls\n");
		quickSort(rank_values_table,index,0,totalNumUrls-1);
        for (i = totalNumUrls-1; i >= totalNumUrls-10; i--)
        {
			memset(str,'\0',32);
			sprintf(str, "%d\t", index[i]);
            MPI_File_write(fh, str, strlen(str), MPI_CHAR, &status);
            sprintf(str, "%f\n", rank_values_table[i]);
            MPI_File_write(fh, str, strlen(str), MPI_CHAR, &status);
        }
		MPI_File_close(&fh);
		free(index);
    }
    return 1;
}

void quickSort(double arr[], int index[],int left, int right) {
      int i = left, j = right;
      int tmpindex;
	  double tmp;
      double pivot = arr[(left + right) / 2];

	  /* partition */
      while (i <= j) {
            while (arr[i] < pivot)
                  i++;
            while (arr[j] > pivot)
                  j--;
            if (i <= j) {
                  tmp = arr[i];
                  arr[i] = arr[j];
                  arr[j] = tmp;

				  tmpindex = index[i];
				  index[i] = index[j];
				  index[j] = tmpindex;

                  i++;
                  j--;
            }
      }
 	  /* recursion */
      if (left < j)
            quickSort(arr, index, left, j);
      if (i < right)
            quickSort(arr, index, i, right);
}