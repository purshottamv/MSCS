#include <iostream>
#include <cstdlib>
#include <fstream>
#include <exception>
#include <iomanip>

#define nREPETITIONS	100
#define VECTOR_SIZE		10000
#define nOPERATIONS		100

using namespace std;

double elapsedtime();

int main(int argv,char *argc[])
{
	ifstream file;
	char line[80];
	int nsamples = 0;

	void overhead(int *nsamples);
	void resolution(int *nsamples);

	#ifdef _TRACE
	cout<<"Running Program in Trace mode\n";
	#endif
	try
	{
		file.open("sizes");
		if(!file)
		{
			throw "Error reading file sizes. Halting execution\n";
			exit(0);
		}
		else
		{
			file.getline(line,80);
			nsamples = atoi(line);
			#ifdef _TRACE
			cout<<"nsamples = "<<nsamples<<endl;
			cout.precision(17);
			#endif
			overhead(&nsamples);
			resolution(&nsamples);
		}
	}
	catch(const char* c)
	{	
		cout << c << endl;   
	}
	catch(exception& e)
	{
		cout << "Standard exception: " << e.what() << endl;  
	}
	return 1;
}

void overhead(int *nsamples)
{
    double time_used, timer_overhead, time_start, time_end, t1, t2, x[VECTOR_SIZE], y[VECTOR_SIZE];
	double cost_in_flops, Gflops_per_sec, time1;
	double alpha = 7.232;
	long sum;
	const long max_rand = 1000000L;

	ofstream file("data_overheads",ios::out);
	file.precision(17);
	try
	{
		srandom(time(NULL));
		// Initialize the vectors
		for(int i=0;i< VECTOR_SIZE;i++)
		{
			x[i] = (double)(random() % max_rand) / max_rand;
			y[i] = (double)(random() % max_rand) / max_rand;
		}
		#ifdef _TRACE
		cout<<"Inside Overhead\n";
		#endif
		for(int i=0;i< *nsamples;i++)
		{
			//Block 1:
			time_start = elapsedtime();
			for(int k = 1; k < nREPETITIONS; k++)
			{
				for(int j = 1;j < VECTOR_SIZE ; j++)
				{
					y[j] = y[j] + alpha * x[j];
				}
			}
			t1 = elapsedtime() - time_start;
			Gflops_per_sec = (2.0e-9) * VECTOR_SIZE * nREPETITIONS/t1;

			#ifdef _TRACE
			cout<<"End of Block 1 : Gflops_per_sec = "<<Gflops_per_sec<<endl;
			#endif
			//Block 2:
			time_start = elapsedtime();
			for(int k = 1 ;k < nREPETITIONS; k++)
			{
				time1 = elapsedtime();
				for(int j = 1;j < VECTOR_SIZE ; j++)
				{
					y[j] = y[j] + alpha * x[j];
				}
			}
			t2 = elapsedtime() - time_start;

			timer_overhead = (t2 - t1)/nREPETITIONS;

			cost_in_flops = 1.0e9 * Gflops_per_sec * timer_overhead;

			file<<timer_overhead<<endl;

			#ifdef _TRACE
			cout<<"End of Block 2 : timer_overhead = "<<timer_overhead<<" cost_in_flops = "<<cost_in_flops<<endl;
			#endif
		}
		file.close();
	}
	catch (exception& e)
	{
		cout << "Exception Occured in Resolution: " << e.what() << endl;  
		file.close();
	}
}

void resolution(int *nsamples)
{
    double time_used, time_start, time_end;
	long sum;
	ofstream file("data_resolution",ios::out);

	#ifdef _TRACE
	cout<<"Inside Resolution\n";
	#endif
	file.precision(17);
	try
	{
		for(int i=0;i< *nsamples;i++)
		{
			sum = 0;
			time_start = elapsedtime();
			for(int j=0; j < nOPERATIONS; j++)
			{
				 sum = sum + j;
			}
			time_used = elapsedtime() - time_start;
			file<<time_used<<endl;
			#ifdef _TRACE
			cout<<"End of Resolution Loop "<< i <<" : timer_used = "<<time_used<<endl;
			#endif
		}

		file.close();
	}
	catch (exception& e)
	{
		cout << "Exception Occured in Resolution: " << e.what() << endl;  
		file.close();
	}
}