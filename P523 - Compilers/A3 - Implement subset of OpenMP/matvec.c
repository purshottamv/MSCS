#include <stdio.h>
#include <stdlib.h>
#define MAX 5
double *matvec(double **A,double *B, double *C,int);
double *matvec2 (double **A, double *B, double *C, int n)	
{
	// This will not get called in main. 
	// It is the equivalent of matvec to verify the results
   int i, j;
   for (i = 0; i < n; i++) {
     C[i] = 0.0;
     for (j = 0; j < n; j++) {
		C[i] = C[i] + A[i][j] * B[j];
     }
   }
   return C;
}
int main()
{
	double **mat;
	double vec1[MAX];
	double vec2[MAX];
	double *vec3;
	int i=0,j=0;

	mat = (double **)malloc(sizeof(double *) * MAX);
	for(i=0;i<MAX;i++)
	{
		// Initializing the vector and matrix with some arbitrary values
		vec1[i] = 2*i + 3;
		mat[i] = (double *)malloc(MAX * sizeof(double));
		for(j=0;j<MAX;j++)
			mat[i][j] = i*j + 3;
	}
	vec3 = matvec(mat,vec1,vec2,MAX);
	for(i=0;i<MAX;i++)
	{
		printf("C[%d] = %f\n",i,vec3[i]);
		free(mat[i]);
	}
	free(mat);
	return 1;
}