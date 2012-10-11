#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <cstdio>

using namespace std;
char *mem;

int main()
{
	char filename[20];;
	bool createMemoryFile(char *filename);
	bool initializememory();
	bool readmemory();
	bool copyMemoryFile(char *);
	int choice;
	// Allocate the memory
	mem = new char[1024*1024];
	memset(filename,'\0',20);

	cout<<"Enter your Choice : \n1. Create a new Memory File.\n2. Read an existing memory file\n";
	cin>>choice;
	if(choice < 1 || choice > 2)
	{
		cout<<"Invalid Input. Terminating Program.\n";
		return 1;
	}
	cout<<"Enter the file name : ";
	cin>>filename;
	if(choice == 1)
	{
		if(!initializememory())
		{
			cout<<"Error in Initializing the memory\n";
			return 1;
		}
		if(!readmemory())
		{
			cout<<"Error in reading the memory file"<<endl;
			return 1;
		}
		if(!createMemoryFile(filename))
		{
			cout<<"Error in Creating memory File\n";
			return 1;
		}
	}
	else if(choice == 2)
	{
		if(!copyMemoryFile(filename))
		{
			cout<<"Error in Creating memory File\n";
			return 1;
		}
		if(!readmemory())
		{
			cout<<"Error in reading the memory file"<<endl;
			return 1;
		}
	}
	delete mem;
}

bool initializememory()
{
	bool bRetVal = true;
	
	int ival,*iptr;
	iptr = &ival;
	double dval = 0.0;

	cout<<"Initializing the memory\n";
	ival = 1;
	sprintf(&mem[0],"%d",ival);
	ival = 3;
	sprintf(&mem[4],"%d",ival);
	ival = 8;
	sprintf(&mem[4+4],"%d",ival);
	ival = 24;
	sprintf(&mem[12],"%d",ival);
	ival = 9;
	sprintf(&mem[16],"%d",ival);

	int i=0;
	dval = 1.4;
    for(i =0;i<20;i++)
	{
		dval = dval + i;
		sprintf(&mem[100+(i*8)],"%g",dval);
		//memcpy(&mem[100+i],&dval,sizeof(double));
	}
	dval = 2.6;
    for(i =0;i<20;i++)
	{
		dval = dval + i;
		//memcpy(&mem[800+i],&dval,sizeof(double));
		sprintf(&mem[800+(i*8)],"%g",dval);
	}
	
	return bRetVal;	
}

bool createMemoryFile(char *outputFile)
{
	ofstream out;
	long count = 0;
	char cdata;
	bool bRetVal = true;

	cout<<"Creating the memory file : "<<outputFile<<endl;
	out.open(outputFile,ios::out);
	if(!out)
	{
		cout<<"Error Opening File "<<outputFile<<endl;
		return false;
	}
	while(count<(1024*1024))
	{	
		memcpy(&cdata,&mem[count],sizeof(char));
		out.write(&cdata,sizeof(char));
		count++;
	}
	cout<<"\nMemory Dump created successfully in \""<<outputFile<<"\""<<endl;
	out.close();
	return bRetVal;	
}

bool readmemory()
{
	bool bRetVal = true;
	char ch[20];
	int ival;
	double dval;
	cout<<"Reading the memory file\n";

	memset(ch,'\0',20);
	memcpy(&ch,&mem[0],sizeof(int));
	cout<<"mem[0] = "<<atoi(ch)<<endl;

	memset(ch,'\0',20);
	memcpy(&ch,&mem[4],sizeof(int));
	cout<<"mem[4] = "<<atoi(ch)<<endl;

	memset(ch,'\0',20);
	memcpy(&ch,&mem[8],sizeof(int));
	cout<<"mem[8] = "<<atoi(ch)<<endl;

	memset(ch,'\0',20);
	memcpy(&ch,&mem[12],sizeof(int));
	cout<<"mem[12] = "<<atoi(ch)<<endl;

	memset(ch,'\0',20);
	memcpy(&ch,&mem[16],sizeof(int));
	cout<<"mem[16] = "<<atoi(ch)<<endl;

	int i=0;
    for(i =0;i<10;i++)
	{
		memset(ch,'\0',20);
		memcpy(&ch,&mem[100+(i*8)],sizeof(double));
		cout<<"mem["<<(100+(i*8))<<"] = "<<strtod(ch,NULL)<<endl;
	}

    for(i =0;i<10;i++)
	{
		memset(ch,'\0',12);
		memcpy(&ch,&mem[800+(i*8)],sizeof(double));
		cout<<"mem["<<(800+(i*8))<<"] = "<<strtod(ch,NULL)<<endl;
	}

    for(i =0;i<10;i++)
	{
		memset(ch,'\0',12);
		memcpy(&ch,&mem[1600+(i*8)],sizeof(double));
		cout<<"mem["<<(1600+(i*8))<<"] = "<<strtod(ch,NULL)<<endl;
	}

	return bRetVal;	
}
bool copyMemoryFile(char *initMemFile)
{
	ifstream in;
	long count = 0;
	char cdata;
	bool bRetVal = true;
	// Allocate the memory
	mem = new char[1024*1024];

	in.open(initMemFile,ios::in);
	if(!in)
	{
		cout<<"Error Opening File "<<initMemFile<<endl;
		return false;
	}
	while(!in.eof() && count <= (1024*1024))
	{	
		in.read(&cdata,sizeof(char)); 
		memcpy(&mem[count],&cdata,sizeof(char));
		count++;
	}
	cout<<count<<" bytes read from file"<<initMemFile<<endl;
	in.close();
	return bRetVal;	
}