#include <iostream>
#include <cstdlib>
#include "papi.h"
using namespace std;

#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define max(a,b)  (((a) > (b)) ? (a) : (b))

#define MATVAL 1
#define NUM_EVENTS 4
#define SIZE 6000
struct Sample1
{
	int x;
	double y[100];
};

struct Sample2
{
	int x[SIZE];
	double y[100*SIZE];
};


int main(int argc,char *argv[])
{
	int i=0,j=0,k=0,ii,jj,kk,r;
	int sum = 0,N,B;
	int retVal, EventSet = PAPI_NULL;
	int Events[NUM_EVENTS] = {PAPI_L1_DCM,PAPI_L1_ICM,PAPI_L2_DCM,PAPI_L2_ICM};
	long_long values[NUM_EVENTS],values1[NUM_EVENTS];

	Sample1 s[SIZE];
	Sample2 s2;

/*	cout<<"The example struct used is :"<<endl;
	cout<<"struct Sample\n";
	cout<<"{\n";
	cout<<"    int x;\n";
	cout<<"    double y[100];\n";
	cout<<"}\n";
*/
	cout<<"\nStarting Counters\n"<<endl;
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
	cout<<"Before Example 1 : Array Size :"<<SIZE<<":"<<"L1 Data Cache Misses        :"<<values[0]<<endl;
	cout<<"Before Example 1 : Array Size :"<<SIZE<<":"<<"L1 Instruction Cache Misses :"<<values[1]<<endl;
	cout<<"Before Example 1 : Array Size :"<<SIZE<<":"<<"L2 Data Cache Misses        :"<<values[2]<<endl;
	cout<<"Before Example 1 : Array Size :"<<SIZE<<":"<<"L2 Instruction Cache Misses :"<<values[3]<<endl;

	for(i=0;i<SIZE;i++)
	{
		s[i].x = 25;
	}

	if(PAPI_stop_counters(values,NUM_EVENTS) != PAPI_OK)
	{
		cout<<"\nPAPI : Error in Stopping Counters\n"<<endl;
		exit(1);
	}
	cout<<"Values after executing program : \n";
	cout<<"After Example 1 : Array Size :"<<SIZE<<":"<<"L1 Data Cache Misses        :"<<values[0]<<endl;
	cout<<"After Example 1 : Array Size :"<<SIZE<<":"<<"L1 Instruction Cache Misses :"<<values[1]<<endl;
	cout<<"After Example 1 : Array Size :"<<SIZE<<":"<<"L2 Data Cache Misses        :"<<values[2]<<endl;
	cout<<"After Example 1 : Array Size :"<<SIZE<<":"<<"L2 Instruction Cache Misses :"<<values[3]<<endl;
	
	sleep(2);
	cout<<"\nStarting Counters for Sample 2\n"<<endl;
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
	cout<<"Before Example 2 : Array Size :"<<SIZE<<":"<<"L1 Data Cache Misses        :"<<values[0]<<endl;
	cout<<"Before Example 2 : Array Size :"<<SIZE<<":"<<"L1 Instruction Cache Misses :"<<values[1]<<endl;
	cout<<"Before Example 2 : Array Size :"<<SIZE<<":"<<"L2 Data Cache Misses        :"<<values[2]<<endl;
	cout<<"Before Example 2 : Array Size :"<<SIZE<<":"<<"L2 Instruction Cache Misses :"<<values[3]<<endl;

	for(i=0;i<SIZE;i++)
	{
		s2.x[i] = 25;
	}

	if(PAPI_stop_counters(values,NUM_EVENTS) != PAPI_OK)
	{
		cout<<"\nPAPI : Error in Stopping Counters\n"<<endl;
		exit(1);
	}
	cout<<"Values after executing program : \n";
	cout<<"After Example 2 : Array Size :"<<SIZE<<":"<<"L1 Data Cache Misses        :"<<values[0]<<endl;
	cout<<"After Example 2 : Array Size :"<<SIZE<<":"<<"L1 Instruction Cache Misses :"<<values[1]<<endl;
	cout<<"After Example 2 : Array Size :"<<SIZE<<":"<<"L2 Data Cache Misses        :"<<values[2]<<endl;
	cout<<"After Example 2 : Array Size :"<<SIZE<<":"<<"L2 Instruction Cache Misses :"<<values[3]<<endl;
	
	return 1;
}