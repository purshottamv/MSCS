#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define SIZE 3

double L[SIZE][SIZE];
int i =0;
int j=0;
int k = 0;
void cholesky(double A[SIZE][SIZE])
{
	double s;
	omp_set_num_threads(3);
	#pragma omp parallel private(i,j,k,s)
	{
		#pragma omp for
		for (i = 0; i < SIZE; i++)
		{
			printf("Thread %d: Inside Parallel Block \n",omp_get_thread_num());
			for (j = 0; j < (i+1); j++)
			{
				s = 0;
				for (k = 0; k < j; k++)
				{
					s = s + L[i][k] * L[j][k];
				} 
				if(i==j)
				{
					L[i][j] = sqrt(A[i][i] - s);
				}
				else
				{
					L[i][j] = (1.0 / L[j][j] * (A[i][j] - s));
				}
			}
		}
	}
}
 
void show_matrix(double A[SIZE][SIZE]) 
{
    for (i = 0; i < SIZE; i++) 
	{
        for (j = 0; j < SIZE; j++)
		{
            printf("%2.5f ", A[i][j]);
		}
        printf("\n");
    }
}

void initialize(double mat[SIZE][SIZE])
{
    for (i = 0; i < SIZE; i++)
	{    
		for (j = 0; j < SIZE; j++)
		{
			mat[i][j] = 0;
		}
    }
}

int main() {
	double m1[3][3] = {{4, -2, -6},
						{-2, 10,  9},
						{-6,  9, 14}};
	initialize(L);
    show_matrix(L);
	cholesky(m1);
    show_matrix(L);
    return 0;
}
