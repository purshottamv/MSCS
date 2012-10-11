/* We don't (yet) support pre-processor directives. */

int main ()
{
  x = 10;
  y = 2.5 / x;
  z = f() + x[2][3];
  z[1][x]; /* this statement has no effect, but is valid PidginC statement */
  w = 2 + foo(x,10);
  printf("%d %d\n", x, w);
}
