/*
	This is only for the large input set 1. We know that there are only 19 fair and square numbers between 1 and 10e+14.
	Therefore, build the set of all these 19 numbers and check if each of these number fall within the given range.
*/

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <fstream>
#include <set>

using namespace std;

bool ispalindrome(unsigned long num)
{
	string str = static_cast<ostringstream*>( &(ostringstream() << num) )->str();
	long len = str.length();
	for(long j = 0 ; j < (len / 2); j++)
	{
		if(str[j] != str[len - 1 -j])
			return false;
	}
	return true;
}

int main(int argc, char *argv[])
{
	ifstream infile;
	string line;
	
	infile.open(argv[1],ios::in);
	if(!infile.is_open())
	{
		cout<<"Unable to open input file\n";
		exit(1);
	}

	// Read the first line containing the number of test cases
	if(!getline(infile,line))
	{
		cout<<"Cannot read the first line containing the number of test cases\n";
		infile.close();
		exit(1);
	}

	int testCasesCount = 0, nTestCases = atoi(line.c_str());
	unsigned long lowerlimit,upperlimit, count;
	long double dnum;
	char *pch;
	string slowerlimit, supperlimit;
	set<unsigned long> palindromes;
	set<unsigned long>::iterator iter, end;
	ofstream outfile("output.txt",ios::out);
	
	//Build the set
	
	lowerlimit = 1;
	upperlimit = pow(10,14);

	for( ; lowerlimit <= upperlimit ; lowerlimit++)
	{
		dnum = sqrt(lowerlimit);
		if( std::floor(dnum) == dnum && ispalindrome(lowerlimit) && ispalindrome((unsigned long)dnum))
		{
			palindromes.insert(lowerlimit);
			outfile<<lowerlimit<<endl;
		}
//		if(lowerlimit % 10000)
//			cout<<"Reached "<<lowerlimit<<endl;
	}

	outfile<<"\nThere are "<<palindromes.size()<<" palindromes"<<endl;
	end = palindromes.end();
	// Now run the test cases
	while(getline(infile,line))
	{
		testCasesCount++;
		count = 0;

		stringstream ss(line);
		getline(ss, slowerlimit, ' ');
		getline(ss, supperlimit, ' ');
		lowerlimit = atoi(slowerlimit.c_str());
		upperlimit = atoi(supperlimit.c_str());

		for( iter = palindromes.begin(); lowerlimit <= upperlimit ; lowerlimit++)
		{
			if(*iter >= lowerlimit && *iter <= upperlimit)
				count++;
		}

		outfile<<"Case #"<<testCasesCount<<": "<<count<<endl;
		if(testCasesCount == nTestCases)
			break;
	}
	return 0;
}