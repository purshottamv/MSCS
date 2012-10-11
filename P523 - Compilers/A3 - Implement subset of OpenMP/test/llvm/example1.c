int main ()
{
	int x,y;
	int z;
	
	x = 10;
	y = 2.5 / ( x + z / y ) - (45 * x);
	z = 2.5 + x / y * 5;
	{
		int x,z;	/* y is not redeclared */
		x = 10;
		x = 2.5 / ( x + z / y ) - (45 * x);
		{
			int y;	/* y redeclared */
			y = 4.5 * x + z;
		}
		z = 2.5 + x / y * 5;
	}
	return x;
}