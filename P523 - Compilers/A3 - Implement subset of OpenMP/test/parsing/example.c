#include <stdio.h>
// This is the driver code to check the llvm assembly
//double somefunction(int);
double somefunction(int,int,double);
int main()
{
	// printf("The value returned is %d\n",somefunction(100));
	printf("The value returned is %f\n",somefunction(100,200,3.5));
	return 1;
}