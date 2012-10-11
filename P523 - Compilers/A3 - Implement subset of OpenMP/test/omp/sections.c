/* Adding and Multiplying two arrays using sections */
#define ELEMENTS 9
int main()
{
	int x,y1,y2,n;
	int a[ELEMENTS];
	int b[ELEMENTS];
	int c[ELEMENTS];
	int d[ELEMENTS];
	int i,j;
	for(i=0;i<ELEMENTS;i=i+1)
	{
		a[i] = i*2;
		b[i] = i*3;
	}
	omp_set_num_threads(3);
	printf("\n************ Example exemplifying OMP sections  ************\n\n");
	#pragma omp parallel
	{
		x = omp_get_thread_num();
		printf("Thread %d: Hello World %d. I am outside sections\n",omp_get_thread_num(),x);
		#pragma omp sections
		{
			#pragma omp section
			{
				printf("Thread %d doing section 1\n",omp_get_thread_num());
				for(i=0;i<ELEMENTS;i=i+1)
				{
					c[i] = a[i] * b[i];
				}
			}
			#pragma omp section
			{
				for(i=0;i<ELEMENTS;i=i+1)
				{
					d[i] = a[i] * b[i];
				}
			}
			#pragma omp section
			{
				printf("Thread %d doing section 3\n",omp_get_thread_num());
			}
			#pragma omp section
			{
				printf("Thread %d doing section 4\n",omp_get_thread_num());
			}
			printf("Thread %d. I am inside sections but not part of any section.\n",omp_get_thread_num());
		}/* End of OMP Sections */
	}
	printf("Addition of Arrays\n");
	for(i=0;i<ELEMENTS;i=i+1)
	{
		printf("%d = %d + %d\n",c[i],a[i],b[i]);
	}
	printf("Multiplication of Arrays\n");
	for(i=0;i<ELEMENTS;i=i+1)
	{
		printf("%d = %d + %d\n",d[i],a[i],b[i]);
	}
	return 1;
}