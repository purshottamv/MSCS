#include <iostream>
#include <string>
#include <cstdlib>
#include <set>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

set< vector<int> > winningSequence;
vector<int> playerX, playerO;	// Save the Positions for Player X and O

bool winner(vector<int> &positions)
{
	// Determine if the given list of positions contains the winning sequence

	vector<int> winSeq;
	vector<int>::iterator end = positions.end(), viter = positions.begin();
	set< vector<int> >::iterator iter,iter_end = winningSequence.end();

	#ifdef _DEBUG
		cout<<"\nChecking the sequence : ";
		for (; viter != end ; viter++)
		{
			cout<<*viter<<" ";
		}
		cout<<endl;
	#endif
	
	// Take the list of all winning positions from the set and determine if positions is part of the winning sequence
	for(iter = winningSequence.begin(); iter != iter_end; iter++)	// Traverse through the set
	{
		winSeq = *iter;
		if(std::find(positions.begin(),positions.end(),winSeq[0]) != end && 
			std::find(positions.begin(),positions.end(),winSeq[1]) != end && 
			std::find(positions.begin(),positions.end(),winSeq[2]) != end && 
			std::find(positions.begin(),positions.end(),winSeq[3]) != end)
		{
			return true;
		}
	}
	return false;
}

void play(char *inputfile)
{
	// This function reads the file the input and processes each test case

	ifstream infile;
	string line;

	#ifdef _DEBUG
		cout<<"\nStart reading file\n";
	#endif

	infile.open(inputfile,ios::in);
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
	int lineCount = 0, rowNumber = 0;
	int column = 0, position = 0, emptyCount = 0;
	ofstream outfile("output.txt",ios::out);

	while(getline(infile,line))
	{
		if(lineCount % 5 == 0)
		{
			testCasesCount++;

			// Initialize the variables for the next test case
			rowNumber = 0;
			emptyCount = 0;
			playerX.clear();
			playerO.clear();
			#ifdef _DEBUG
				cout<<"\nTest Case # "<<testCasesCount<<endl;
			#endif

		}

		lineCount++;

		if(testCasesCount > nTestCases)
		{
			cout<<"Done with All the test cases\nExiting....\n";
			break;
		}

		if(line.length() > 0)
		{
			// Read the input line for the first 4 characters
			column = 0;
			while(column <= 4)
			{
				position = (4 * rowNumber) + (column + 1);
				switch(line[column])
				{
					case 'X':
							playerX.push_back(position);
							break;
					case 'O':
							playerO.push_back(position);
							break;
					case '.':
							emptyCount++;
							break;
					case 'T':
							playerX.push_back(position);
							playerO.push_back(position);
							break;
				}
				column++;
			}
			rowNumber++;
		
			if(rowNumber == 4)
			{
				// We have read all the four lines. Now render the decision.
				// Go though the positions
				if(winner(playerX))
				{
					// Player X wins
					outfile<<"Case #"<<testCasesCount<<": "<<"X won"<<endl;
				}
				else if(winner(playerO))
				{
					// Player O wins
					outfile<<"Case #"<<testCasesCount<<": "<<"O won"<<endl;
				}
				else if(emptyCount == 0)
				{
					// Draw
					outfile<<"Case #"<<testCasesCount<<": "<<"Draw"<<endl;			
				}
				else
				{
					// Game is still on
					outfile<<"Case #"<<testCasesCount<<": "<<"Game has not completed"<<endl;
				}
			}
		}
		#ifdef _DEBUG
			cout<<"End of Reading line "<<lineCount<<endl;
		#endif
	}	// end of While(getline())
	
	// Close the files
	infile.close();
	outfile.close();
}

void buildWinningSequence()
{
	vector<int> seq;
	int row,col;

	// Push the row sequences
	for(int row=0;row <= 3; row++)
	{
		for(int col = 1; col <= 4; col++)
		{
			seq.push_back(4*row+col);
		}
		winningSequence.insert(seq);
		seq.clear();
	}

	// push the column sequences
	for(int col=1;col <= 4; col++)
	{
		for(int row = 0; row <= 3; row++)
		{
			seq.push_back(4*row+col);
		}
		winningSequence.insert(seq);
		seq.clear();
	}

	// Push the diagonal sequences
	seq.push_back(1);seq.push_back(6);seq.push_back(11);seq.push_back(16);
	winningSequence.insert(seq);seq.clear();

	seq.push_back(4);seq.push_back(7);seq.push_back(10);seq.push_back(13);
	winningSequence.insert(seq);seq.clear();
}

int main(int argc,char *argv[])
{
	if(argc < 2)
	{
		cout<<"Please provide the name of the input file. Format : \n./tictactoetomek input.txt\n";
		exit(1);
	}
	buildWinningSequence();
	play(argv[1]);
	return 0;
}