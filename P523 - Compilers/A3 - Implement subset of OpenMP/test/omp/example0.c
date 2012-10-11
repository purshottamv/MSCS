/* Adding two arrays  */
#define ELEMENTS 9
int main()
{
	int x;
	int a[ELEMENTS];
	int b[ELEMENTS];
	int c[ELEMENTS];
	int i,j;
	for(i=0;i<ELEMENTS;i=i+1)
	{
		a[i] = i*2;
		b[i] = i*3;
	}
	omp_set_num_threads(3);
	printf("\n***** Example: Adding two arrays using OMP parallel and OMP for **********\n\n");
	#pragma omp parallel
	{
		x = omp_get_thread_num();
		printf("Thread %d: Inside Parallel Block \n",omp_get_thread_num());
		#pragma omp for
		for(i=0;i<ELEMENTS;i=i+1)
		{
			printf("Thread %d : Adding element %d\n",omp_get_thread_num(),i);
			c[i] = a[i] + b[i];
		}
	}
	for(i=0;i<ELEMENTS;i=i+1)
	{
		printf("%d = %d + %d\n",c[i],a[i],b[i]);
	}
	return 1;
}
