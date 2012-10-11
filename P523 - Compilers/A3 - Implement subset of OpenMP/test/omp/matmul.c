/* Matrix Multiplication  */
#define ROWS 3
#define COLUMNS 3
int main()
{
	int x,y1,y2;
	int a[ROWS][COLUMNS];
	int b[ROWS][COLUMNS];
	int c[ROWS][COLUMNS];
	int i,j,k;
	printf("\n************ Example exemplifying Matrix Multiplication ************\n\n");
	printf("Printing Matrix A\n");
	for(i=0;i<ROWS;i=i+1)
	{
		for(j=0;j<COLUMNS;j=j+1)
		{
			a[i][j] = (i+1)*(j+2)*2;
			b[i][j] = (i+1)*(j+2)*3;
			c[i][j] = 0;
			printf("%5d",a[i][j]);
		}
		printf("\n");
	}
	printf("Printing Matrix B\n");
	for(i=0;i<ROWS;i=i+1)
	{
		for(j=0;j<COLUMNS;j=j+1)
		{
			printf("%5d",b[i][j]);
		}
		printf("\n");
	}
	omp_set_num_threads(3);
	#pragma omp parallel
	{
		#pragma omp for
		for(i=0;i<ROWS;i=i+1)
		{
			printf("\nThis is thread %d\n",omp_get_thread_num());
			for(j=0;j<COLUMNS;j=j+1)
			{
				x = 0;
				for(k=0;k<COLUMNS;k=k+1)
				{
					x = x + (a[i][k] * b[k][j]);
				}
				c[i][j] = x;
			}
		}
	}
	printf("Printing Matrix C\n");
	for(i=0;i<ROWS;i=i+1)
	{
		for(j=0;j<COLUMNS;j=j+1)
		{
			printf("%5d",c[i][j]);
		}
		printf("\n");
	}
	return 1;
}
