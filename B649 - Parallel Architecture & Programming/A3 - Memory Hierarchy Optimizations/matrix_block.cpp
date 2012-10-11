#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include "papi.h"
using namespace std;

#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define max(a,b)  (((a) > (b)) ? (a) : (b))

#define MATVAL 1
#define NUM_EVENTS 4

int main(int argc,char *argv[])
{
	int i=0,j=0,k=0,ii,jj,kk,r;
	int sum = 0,N,B;
	int retVal, EventSet = PAPI_NULL;
	int Events[NUM_EVENTS] = {PAPI_L1_DCM,PAPI_L1_ICM,PAPI_L2_DCM,PAPI_L2_ICM};
	long_long values[NUM_EVENTS],values1[NUM_EVENTS];

	/* Initialize PAPI library */
/*	retVal = PAPI_library_init(PAPI_VER_CURRENT);
	if(retVal != PAPI_VER_CURRENT)
	{
		cout<<"\nError initializing PAPI Library\n";
		exit(1);
	}

	//  Create the event set and Add the events 
	if(PAPI_create_eventset(&EventSet) != PAPI_OK)
		handle_error(1);

	if(PAPI_add_event(&EventSet,PAPI_L1_DCM) != PAPI_OK)
		handle_error(1);
	if(PAPI_add_event(&EventSet,PAPI_L1_ICM) != PAPI_OK)
		handle_error(1);
	if(PAPI_add_event(&EventSet,PAPI_L2_DCM) != PAPI_OK)
		handle_error(1);
	if(PAPI_add_event(&EventSet,PAPI_L2_ICM) != PAPI_OK)
		handle_error(1);
*/
	if(argc == 3)
	{
		N = atoi(argv[1]);
		B = atoi(argv[2]);
		if(B > N)
		{
			cout<<"\nError: Required N >= B\n";
			return 1;
		}
	}
	else
	{
		cout<<"\nPlease specify the Matrix size and block size\nFor example : ./matrix_block.out 6 2\n"<<endl;
		return 1;
	}
	char matsize[20],blocksize[20];
	memset(matsize,'\0',20);
	memset(blocksize,'\0',20);
	sprintf(matsize,"Mat size - %d X %d : ",N,N);
	sprintf(blocksize,"Block size - %d : ",B);
//	cout<<"\nMatrix Size : "<<N<<" x "<<N<<endl;
//	cout<<"Block Size : "<<B<<endl;
	short mat1[N][N];
	short mat2[N][N];
	short mat3[N][N];

	for(j=0;j<N;j++)
	{
	   for(k=0;k<N;k++)
		{
			mat1[j][k] = MATVAL;
			mat2[j][k] = MATVAL;
			mat3[j][k] = 0;
//			cout<<mat1[j][k]<<" ";
		}
//		cout<<endl;
	}

	cout<<"\nStarting Counters\n"<<endl;
/*	for(i =0;i<N;i++)
		for(j=0;j<N;j++)
		   for(k=0;k<N;k++)
		      mat3[i][j] = mat3[i][j] + (mat1[i][k] * mat2[k][j]);
*/
	if(PAPI_start_counters(Events,NUM_EVENTS) != PAPI_OK)
	{
		cout<<"\nPAPI : Error in Starting Counters\n"<<endl;
		exit(1);
	}

	if(PAPI_read_counters(values,NUM_EVENTS) != PAPI_OK)
	{
		cout<<"\nPAPI : Error in Reading Counters\n"<<endl;
		exit(1);
	}

	cout<<"Values before executing program : \n";
	cout<<matsize<<blocksize<<"L1 Data Cache Misses        :"<<values[0]<<endl;
	cout<<matsize<<blocksize<<"L1 Instruction Cache Misses :"<<values[1]<<endl;
	cout<<matsize<<blocksize<<"L2 Data Cache Misses        :"<<values[2]<<endl;
	cout<<matsize<<blocksize<<"L2 Instruction Cache Misses :"<<values[3]<<endl;

	for (jj = 0; jj < N; jj = jj+B)
		for (kk = 0; kk < N; kk = kk+B)
			for (i = 0; i < N; i = i+1)
				for (j = jj; j < min(jj+B,N); j = j+1)
				{
					r = 0;
					for (k = kk; k < min(kk+B,N); k = k + 1)
						r = r + mat1[i][k]*mat2[k][j];
					mat3[i][j] = mat3[i][j] + r;
				};

	if(PAPI_stop_counters(values,NUM_EVENTS) != PAPI_OK)
	{
		cout<<"\nPAPI : Error in Stopping Counters\n"<<endl;
		exit(1);
	}
	cout<<"Values after executing program : \n";
	cout<<matsize<<blocksize<<"L1 Data Cache Misses        :"<<values[0]<<endl;
	cout<<matsize<<blocksize<<"L1 Instruction Cache Misses :"<<values[1]<<endl;
	cout<<matsize<<blocksize<<"L2 Data Cache Misses        :"<<values[2]<<endl;
	cout<<matsize<<blocksize<<"L2 Instruction Cache Misses :"<<values[3]<<endl;
	
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
//			cout<<mat3[i][j]<<" ";
		}
//		cout<<endl;
	}
	return 1;
}
