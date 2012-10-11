

main()
{

  int i,j,p[2];
  char s[100];
  char s2[100];
  
  i=pipe(p);
  
  printf("pipe returned %d %d %d\n",i,p[0],p[1]);
  
  for (j=0;j<5;j++){
    sprintf(s,"I am a string %d\n",j);
    i=write(p[1],s,strlen(s));
    printf("Write completed i=%d\n",i);
    i=read(p[0],s2,100);
    printf("Read from the pipe completed i=%d\n\t%s\n",i,s2);
  }

}
