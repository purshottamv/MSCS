#include <iostream>
#include <string>
#include <bitset>
#include <unordered_set>
#include <vector>
#include <sys/time.h>

#define N 9
#define WINX	1
#define WIN0	2
#define DRAW	3
#define OTHER	4

using namespace std;
long count = 0;
long winx = 0;
long win0 = 0;
long draw = 0;
vector<string> winningSequence({"210","345","876","630","147","258","840","642"});

int winner(string& permuted)
{
	// PlayerX always plays first
	string playerX = "";	// Plays Odd position starting from 1st position
	string player0 = ""; // Plays Even Position starting from 2nd position
	string player = "";
	vector<string>::iterator iter;
	string seq;

	for(int i=0; i<= 8 ; i++)
	{
		if(i % 2 == 0)	// It is X's turn
		{
			playerX.push_back(permuted.at(i));
			player = playerX;
		}
		else
		{
			player0.push_back(permuted.at(i));
			player = player0;
		}
		
		if(player.length() >= 3)
		{
			for(iter = winningSequence.begin(); iter != winningSequence.end(); iter++)
			{
				seq = *iter;
				if(player.find(seq[0]) <= 8 && player.find(seq[1]) <= 8 && player.find(seq[2]) <= 8)
				{
					if(i % 2 == 0)
					{
						winx++;
						return WINX;
					}else
					{
						win0++;
						return WIN0;
					}
				}
			}
		}
	}
	draw++;
	return DRAW;
}
void permute(int level,string permuted,bitset<N>& mark,string& original)
{
	int length = original.size();
	if (level == length) {
		cout<<permuted<<" "<<winner(permuted)<<endl;
		count++;
	} else {
		for (int i = 0; i < length; i++) {
			if (!mark[i]) {
				mark.set(i);
				permute(level + 1, permuted + original[i],mark, original);
				mark.reset(i);
			}
		}
	}	
}

int main(int argc,char *argv[])
{
	string s = "012345678",permuted = "";
	bitset<N> mark;
	long seconds = 0, useconds = 0, mtime = 0;
	struct timeval start_time, end_time;

	mark.reset();

	// Start the timer
	gettimeofday(&start_time, NULL);	

	permute(0,permuted,mark,s);

	// End the timer
	gettimeofday(&end_time, NULL);
	seconds  = end_time.tv_sec  - start_time.tv_sec;
	useconds = end_time.tv_usec - start_time.tv_usec;
	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	cout<<"\nTotal Running Time : "<<mtime<<" ms\n";
	cout<<"\nTotal Permutations = "<<count<<endl;
	cout<<"\nWin X = "<<winx<<endl;
	cout<<"\nWin 0 = "<<win0<<endl;
	cout<<"\nDraws = "<<draw<<endl;
	return 0;
}