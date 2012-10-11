/* We don't (yet) support pre-processor directives. */

int main ()
{
	double some_function(int,int,int);
	double some_function(int *x,int);	/* Function declaration */
	double some_function(int *x,int a[]);	/* Function declaration. passing pointer and array */
	x = 10;
	y = 2.5 / x;
	z = f() + x[2][3];
	z[1][x];		/* this statement has no effect, but is valid PidginC statement */
	w = 2 + foo(x,10);
	printf("%d %d\n", x, w);
	some_function(&y);  /* Function call. Passing Address of a variable */
	while(x>3)
	{
		/* calling a function inside while condition */
		while(some_function(x,y))
		{
			/* Will this run */
		}
		/* Nesting if condition inside while */		
		if(y>6)
		{
			x = x + 67;
		}
		else
		{
			if(!x)
			{
				y =y + 20;
				printf("Else block");
			}
		}
	}
}