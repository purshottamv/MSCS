/* ============================================================================
 Name        : seqpagerank.cpp
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Sequential algorithm to calculate page rank
 ============================================================================
*/
#include "WebURL.h"
#include <unordered_map>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sys/time.h>
#include <algorithm>
#include <set>

double danglingFactor = 0;

typedef std::pair<long,WebURL> MyPair;

struct Compare
{
    bool operator()(const MyPair& firstPair, const MyPair& secondPair) const
    {
        return firstPair.second.getOldPageRank() > secondPair.second.getOldPageRank();
    }
};

void ltrim(string& sLine)
{
	int i =0;
	while(sLine.compare(i,1," ") == 0 && i < sLine.length())
		i++;
	sLine.assign(sLine,i,sLine.length()-i);
}

void rtrim(string& sLine)
{
	int i = sLine.length();
	while(i > 0 && sLine.compare(i-1,1," ") == 0)
		i--;
	sLine.assign(sLine,0,i);
}

void initializeWebURLs(unordered_map<long,WebURL>& webURLs, long& totalURLs, string inputFile )
{
	double initialPageRankValue = (double)1/totalURLs;
	ifstream infile(inputFile,ios::in);
	string line, url;
	size_t pos;
	vector<long> urlArray;

	if(!infile.is_open())
	{
		string s = "Error : Cannot open the input file \"" + inputFile + "\"";
		throw s;
	}
	while(!infile.eof())
	{
		getline(infile,line);
		ltrim(line);
		rtrim(line);
		urlArray.clear();
		if(line.length() > 0)
		{
			pos = line.find(' ',0);
			if(pos > LARGEST_SIZE_T)
			{
				// It contains only the master URL and no outbound list of URLs
				long Webid = atoi(line.c_str());
				urlArray.push_back(Webid); 
			}
			else
			{
				while(pos < LARGEST_SIZE_T)
				{
					url = line.substr(0,pos);
					long Webid = atoi(url.c_str());
					urlArray.push_back(Webid);
					line = line.substr(pos + 1);
					pos = line.find(' ',0);
					if(pos > LARGEST_SIZE_T && line.length() >0 )
					{
						// This is the last url id on the line
						url = line;
						Webid = atoi(url.c_str());
						urlArray.push_back(Webid);
					}
				}
			}
			if(urlArray.size() > 0)
			{
				// Insert the URLs in the Hash Table i.e. unordered_map
				vector<long>::iterator it;
				long masterURLid;

				for ( it=urlArray.begin() ; it < urlArray.end(); it++ )
				{
					if(webURLs.find(*it) == webURLs.end())
					{
						WebURL *url = new WebURL((*it),initialPageRankValue);
						std::pair<long,WebURL> myURL((*it),*url);
						webURLs.insert(myURL);	// copy insertion
						delete url;
					}
					if(it == urlArray.begin())
					{
						masterURLid = *it;	// the first Web url id on the line is the master url id
					}
					else
					{
						WebURL *url = &webURLs.at(masterURLid);
						url->appendToOutboundList((webURLs.find(*it))->second);
					}
				}
			}
		}
	}
	infile.close();
}

int main(int argc, char *argv[])
{
	string inputFile, outputFile;
	int iterationCount;
	float dampingFactor;
	long getNumberOfURLs(string);
	long totalURLs;
	void verify(unordered_map<long,WebURL>& webURLs);
	void calculatePageRank(int& iterationNumber, long& totalURLs, unordered_map<long,WebURL>& webURLs,float& dampingFactor);
	long seconds = 0, useconds = 0, mtime = 0;
	struct timeval start_time, end_time;

	try
	{
		if(argc != 5)
		{
			cout<<"\nThe correct format for running the program is\nseqpagerank [input file path][output file path][iteration count][damping factor]\ne.g.: ./seqpagerank pagerank.input pagerank.output 10 0.85\n";
			exit(0);
		}
		else
		{
			inputFile = argv[1];
			outputFile = argv[2];
			iterationCount = atoi(argv[3]);
			if(iterationCount == 0)
			{
				throw "Error : Iteration Count has to be a valid integer.";
			}
			dampingFactor = atof(argv[4]);
			if(dampingFactor == 0 || dampingFactor > 1)
			{
				throw "Error : Damping factor should be greater than 0 and less than or equal to 1.";
			}
			// retrieve the number of web pages
			totalURLs = getNumberOfURLs(inputFile);
			#ifdef _TRACE
				cout<<"Total Number of URLs = "<<totalURLs<<endl;
			#endif

			// Create the Hash table of all the URLs and initialize each web url with the initial value
			unordered_map<long,WebURL> webURLs;
			initializeWebURLs(webURLs,totalURLs,inputFile);


			#ifdef _TRACE
				verify(webURLs);
			#endif

			// Start the timer
			gettimeofday(&start_time, NULL);

			// Iterate to calculate Page Rank
			// This algorithm takes 1 iteration more than the specified number of iterations.
			// This is because the final pagerank value of current iteration is calculated at the beginning of next iteration.
			for(int iteration = 1; iteration <= iterationCount + 1; iteration++)
			{
				calculatePageRank(iteration, totalURLs, webURLs, dampingFactor);
			}

			// End the timer
			gettimeofday(&end_time, NULL);
			seconds  = end_time.tv_sec  - start_time.tv_sec;
			useconds = end_time.tv_usec - start_time.tv_usec;
			mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
			printf("\nTotal Running Time : %ld ms\n", mtime);

			// sort the unordered_map using multiset
			multiset<MyPair,Compare> s;
			unordered_map<long,WebURL>::iterator iter = webURLs.begin();
			for(; iter != webURLs.end(); ++iter)
				s.insert(*iter);

			multiset<MyPair,Compare>::iterator setiter;
			#ifdef _TRACE
				cout<<"First 10 URLs :"<<endl;
			#endif
			cout<<"Top 10 URLS written to file \""<<outputFile<<"\""<<endl;
			int count = 1;
			ofstream outfile(outputFile,ios::out);
			for (setiter = s.begin(); setiter != s.end() && count <= 10; ++setiter)
			{
				outfile<<(*setiter).second.getId()<<"\t"<<setprecision(15)<<(*setiter).second.getOldPageRank()<< endl;
				count++;
				#ifdef _TRACE
					cout<<(*setiter).second.getId()<<"\t"<<(*setiter).second.getOldPageRank()<< endl;
				#endif
			}
			outfile.close();
		}
	}
	catch (char const* s)
	{
		cout<<s<<endl;
	}
	catch (exception& e)
	{
		cout<<"Standard Exception : "<<e.what();
	}
}

long getNumberOfURLs(string inputFile)
{
	// This is the efficient way of getting the total number of URLs.
	// Just read the first URL id in the last line to get the total number of URLs.
	// No need of reading all the lines from the beginning.

	ifstream infile(inputFile,ios::in);
	int iNumberOfURLs = 0, icode, num;

	if(!infile.is_open())
	{
		string s = "Error : Cannot open the input file \"" + inputFile + "\"";
		throw s;
	}
	
	infile.seekg(-1,ios::end);
	icode = (int)infile.peek();

	while(icode == 13 || icode == 10)
	{
		infile.seekg(-1,ios::cur);
		icode = (int)infile.peek();
	}

	while(icode != 13 && icode != 10)
	{
		infile.seekg(-1,ios::cur);
		icode = (int)infile.peek();
	}

	infile.seekg(1,ios::cur);
	infile>>num;
	infile.close();
	return num + 1;
}

void verify(unordered_map<long,WebURL>& webURLs)
{
	unordered_map<long,WebURL>::iterator it;

	cout<<"\nVERIFYING THE UNORDERED MAP :"<<endl;
	cout<<"The size of unordered_map = "<<webURLs.size()<<endl;
	for(it = webURLs.begin(); it != webURLs.end(); it++)
	{
		cout<<"Web URL = "<<it->first<<":"<<endl;
		WebURL myurl = it->second;
		cout<<"id = "<<myurl.getId()<<"\t"<<"Page Rank = "<<setprecision(15)<<myurl.getNewPageRank()<<"\tOutbound WebURL ids : ";
		list<WebURL *> listofurls = myurl.getOutboundWebURLs();
		list<WebURL *>::iterator listite;
		for(listite = listofurls.begin(); listite != listofurls.end(); listite++)
		{
			WebURL *url = *listite;
			cout<<url->getId()<<" ";
		}
		cout<<endl<<endl;
	}
}

void calculatePageRank(int& iterationNumber, long& totalURLs, unordered_map<long,WebURL>& webURLs,float& dampingFactor)
{
	#ifdef _TRACE
		cout<<"\nIteration : "<<iterationNumber - 1<<" Dangling Factor = "<<danglingFactor<<endl;
	#endif	
	double tmpDanglingFactor = 0;
	unordered_map<long,WebURL>::iterator it;

	// Iterating through each Key (Representing Page) in the Map and calculating the Page Rank of the pages associated with it.
	for(it = webURLs.begin(); it != webURLs.end(); it++)
	{
		WebURL *masterURL = &(it->second);
		list<WebURL *> listofOutboundURLs = masterURL->getOutboundWebURLs();
		list<WebURL *>::iterator listite;

		double val;

		// First time call the evaluate function for the master url to change the iteration number and 
		// calculate the pagerank of the previous iteration
		masterURL->evaluate(iterationNumber,dampingFactor,totalURLs);
		#ifdef _TRACE
			cout<<"src_url: "<<masterURL->getId()<<" cur_val:["<<masterURL->getOldPageRank()<<"]"<<endl;
		#endif
		if(!listofOutboundURLs.empty())
		{
			for(listite = listofOutboundURLs.begin(); listite != listofOutboundURLs.end(); listite++)
			{
				WebURL *url = *listite;
				url->evaluate(iterationNumber,dampingFactor,totalURLs);
				val =  (masterURL->getOldPageRank() / (double)(listofOutboundURLs.size()));
				url->setNewPageRank(val + url->getNewPageRank());
				#ifdef _TRACE
					cout<<" ->tar_url:"<<url->getId()<<" cur_val:"<<url->getOldPageRank()<<" added_val:"<<val<<endl;
				#endif
			}
		}
		else
		{
			tmpDanglingFactor += masterURL->getOldPageRank();
		}
	}
	// Saving the dangling factor to use it in the next cycle
	danglingFactor = tmpDanglingFactor;
}