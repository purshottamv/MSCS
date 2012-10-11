/* Example exemplifying OMP sections and critical */
#define ELEMENTS 9
int main()
{
	int x,y1,y2;
	int a[ELEMENTS];
	int b[ELEMENTS];
	int c[ELEMENTS];
	int i,j;
	for(i=0;i<ELEMENTS;i=i+1)
	{
		a[i] = i*2;
		b[i] = i*3;
	}
	y1 = 0;
	y2 = 0;
	omp_set_num_threads(8);
	
	printf("\n************ Example exemplifying OMP sections and critical ************\n\n");
	#pragma omp parallel
	{
		x = omp_get_thread_num();
		#pragma omp critical
		{
			y1 = y1 + 45;
		}
		y2 = y2 + 45;
		printf("Thread %d: Hello World %d\n",omp_get_thread_num(),x);
		#pragma omp for
		for(i=0;i<ELEMENTS;i=i+1)
		{
			c[i] = a[i] + b[i];
		}
		#pragma omp sections
		{
			#pragma omp section
			{
				printf("Thread %d doing section 1\n",omp_get_thread_num());
			}
			#pragma omp section
			{
				printf("Thread %d doing section 2\n",omp_get_thread_num());
			}
			#pragma omp section
			{
				printf("Thread %d doing section 3\n",omp_get_thread_num());
			}
			#pragma omp section
			{
				printf("Thread %d doing section 4\n",omp_get_thread_num());
			}
			printf("Thread %d doing sections\n",omp_get_thread_num());
		}/* End of OMP Sections */
	}
/*	for(i=0;i<ELEMENTS;i=i+1)
	{
		printf("%d = %d + %d\n",c[i],a[i],b[i]);
	} */
	printf("\ny1 = %d\n",y1);
	printf("\ny2 = %d\n",y2);
	return 1;
}
