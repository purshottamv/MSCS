int main ()
{
	int x,*y;  /* Multiple Declarations */
	int func(int x, int y); /* Function Declaration */

	for(x=0;x<10;x=x+1)
	{
		printf("%s Hello World",string);
		x = x + 1;
		/* Here I have multiple statements inside for loop */
	}
	y = &h;   /* Initializing pointer variable */
	while (x<10 && y>10)
	{
		x = x + 2;
		func(x,y);  /* This is function call with 2 arguments */
	}
}
/* Below is the function definition */
int some_function(int a, int b)	
{
	printf("This is function definition");
	return 0;
}