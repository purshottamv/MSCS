/* ============================================================================
 File Name   : kmeans.cpp
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : KMeans Algorithm
 ============================================================================
The Algorithm (http://home.dei.polimi.it/matteucc/Clustering/tutorial_html/kmeans.html):
1. Place K points into the space represented by the objects that are being clustered. These points represent initial group centroids.
2. Assign each object to the group that has the closest centroid.
3. When all objects have been assigned, recalculate the positions of the K centroids.
4. Repeat Steps 2 and 3 until the centroids no longer move. This produces a separation of the objects into groups from which the metric to be minimized can be calculated.
*/

#include "BreastCancerRecord.h"
#include <fstream>
#include <vector>
#include <cstdlib>	// for rand
#include <unordered_map>
#include <set>

#define CURRENT_CENTROID 0
#define PREVIOUS_CENTROID 1

using namespace std;

vector<long> CentroidHistoryVector;		// Vector of Id, CurrentCentroid and PreviousCentroid
unordered_map<long,vector<long> >	CentroidHistoryTable;
unordered_map<long,BreastCancerRecord> CentroidData;

long lTotalNumberOfRecords;
bool converge = false;
long convergenceCount = 0;

int main(int argc, char *argv[])
{
	string inputFile, outputFile;
	unordered_map<long,BreastCancerRecord> records;
	vector<long> centroids;
	int iNumberOfCentroids, iterations = 0;

	long readInputFile(unordered_map<long,BreastCancerRecord>&, string&);
	void initializeCentroids(vector<long>&,long&,int&,unordered_map<long,BreastCancerRecord>&);
	void assignmentStep(unordered_map<long,BreastCancerRecord>&);
	void computeNewCentroid(vector<long>&,unordered_map<long,BreastCancerRecord>&);
	void calculatePPV(vector<long>&,unordered_map<long,BreastCancerRecord>&);

	string errorMessage = "\nThe correct format for running the program is\nkmeans [input file path] [Number of Centroids]\ne.g.: ./kmeans cancer_data.csv 2\n";
	
	try
	{
		if(argc < 2)
		{
			cout<<errorMessage;
			exit(0);
		}
		inputFile = argv[1];
		iNumberOfCentroids = atoi(argv[2]);
		if(iNumberOfCentroids == 0)
		{
			cout<<"\nError : Number of Centroids cannot be zero\n"<<errorMessage;
			exit(0);
		}
		outputFile = "output.txt";

		lTotalNumberOfRecords = readInputFile(records,inputFile);

		#ifdef _TRACE
			cout<<"Total Number of Centroids : "<<iNumberOfCentroids<<endl;
			cout<<"Total Number of Records : "<<lTotalNumberOfRecords<<endl;
			cout<<"Size of unordered_map : "<<records.size()<<endl;
		#endif
		// This is Step 1 : Place K points into the space represented by the objects that are being clustered. These points represent initial group centroids.
		initializeCentroids(centroids,lTotalNumberOfRecords,iNumberOfCentroids,records);

		while(!converge)
		{
			iterations++;

			assignmentStep(records);					// Step 2 : Assign each object to the group that has the closest centroid.
			computeNewCentroid(centroids,records);	// Step 3 : When all objects have been assigned, recalculate the positions of the K centroids.

			// Stopping Condition. This function checks for convergence. If the Current and Previous Centroids haven't changed for all the objects then quit.
			if (convergenceCount == lTotalNumberOfRecords)
			{
				converge = true;
				#ifdef _TRACE
					cout<<"Converging\n"<<endl;
				#endif
			}
		}
		cout<<"Total iterations : "<<iterations<<endl;

		calculatePPV(centroids,records);
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

void assignmentStep(unordered_map<long,BreastCancerRecord>& records)
{
	/*
		This function checks the 
	*/
	double minDistance = 0.0;
	double distance = 0.0;
	unordered_map<long,BreastCancerRecord>::iterator recordsIter;
	unordered_map<long,BreastCancerRecord>::iterator centroidIter;
	int clusterID;
	
	convergenceCount = 0;
	for(recordsIter = records.begin(); recordsIter != records.end(); ++recordsIter)
	{   
		BreastCancerRecord *rec = &(recordsIter->second);
		minDistance = LARGEST_SIZE_T;
		clusterID = 0;
		for(centroidIter = CentroidData.begin(); centroidIter != CentroidData.end(); centroidIter++)
		{
			BreastCancerRecord *CentroidRec = &(centroidIter->second);
			distance = (CentroidRec->getA1() - rec->getA1()) * (CentroidRec->getA1() - rec->getA1()) +
						(CentroidRec->getA2() - rec->getA2()) * (CentroidRec->getA2() - rec->getA2()) +
						(CentroidRec->getA3() - rec->getA3()) * (CentroidRec->getA3() - rec->getA3()) +
						(CentroidRec->getA4() - rec->getA4()) * (CentroidRec->getA4() - rec->getA4()) +
						(CentroidRec->getA5() - rec->getA5()) * (CentroidRec->getA5() - rec->getA5()) +
						(CentroidRec->getA6() - rec->getA6()) * (CentroidRec->getA6() - rec->getA6()) +
						(CentroidRec->getA7() - rec->getA7()) * (CentroidRec->getA7() - rec->getA7()) +
						(CentroidRec->getA8() - rec->getA8()) * (CentroidRec->getA8() - rec->getA8()) +
						(CentroidRec->getA9() - rec->getA9()) * (CentroidRec->getA9() - rec->getA9());

			if (minDistance >= distance)
			{
				minDistance = distance;
				clusterID = centroidIter->first;
			}
		}
		(CentroidHistoryTable[recordsIter->first])[PREVIOUS_CENTROID] = (CentroidHistoryTable[recordsIter->first])[CURRENT_CENTROID];
		(CentroidHistoryTable[recordsIter->first])[CURRENT_CENTROID] = clusterID;

		// Check if the centroids have changed and count the datapoints for which the centroids have not changed.
		if((CentroidHistoryTable[recordsIter->first])[PREVIOUS_CENTROID] == (CentroidHistoryTable[recordsIter->first])[CURRENT_CENTROID])
			convergenceCount++;

		#ifdef _TRACE
			cout<<"assignmentStep : recordsIter->first = "<<recordsIter->first<<" clusterID = "<<clusterID<<endl;
		#endif
	}
}

long readInputFile(unordered_map<long,BreastCancerRecord>& records,string& inputFile)
{
	// This function reads the csv file and inserts the data into "records" i.e. unordered_map data structure.
	
	ifstream infile(inputFile,ios::in);
	long lTotalNumberOfRecords = 0;
	size_t pos;
	int count;
	string element, line;
	char separator = ',';

	if(!infile.is_open())
	{
		throw "Error : Cannot open the input file \"" + inputFile + "\"";;
	}
	while(!infile.eof())
	{
		getline(infile,line);
		count = 0;
		if(line.length() > 0)
		{
			pos = line.find(separator,0);
			if(pos > LARGEST_SIZE_T)
			{
				// There is no separator
				throw "Error : Invalid input line\n";
			}
			else
			{
				BreastCancerRecord *rec = new BreastCancerRecord();
				while(pos < LARGEST_SIZE_T)
				{					
					element = line.substr(0,pos);
					switch(count)
					{
						case 0:	rec->setSCM(atoi(element.c_str()));	break;
						case 1:	rec->setA1(atoi(element.c_str()));	break;
						case 2:	rec->setA2(atoi(element.c_str()));	break;
						case 3:	rec->setA3(atoi(element.c_str()));	break;
						case 4:	rec->setA4(atoi(element.c_str()));	break;
						case 5:	rec->setA5(atoi(element.c_str()));	break;
						case 6:	rec->setA6(atoi(element.c_str()));	break;
						case 7:	rec->setA7(atoi(element.c_str()));	break;
						case 8:	rec->setA8(atoi(element.c_str()));	break;
						case 9:	rec->setA9(atoi(element.c_str()));	break;
					}
					line = line.substr(pos + 1);
					pos = line.find(separator,0);
					count++;
					if(pos > LARGEST_SIZE_T && line.length() >0 )
					{
						// This is the last element on the line
						element = line;
						rec->setA10(atoi(element.c_str()));
					}
				}
				lTotalNumberOfRecords++;
				std::pair<long,BreastCancerRecord> myrecord(lTotalNumberOfRecords,*rec);
				records.insert(myrecord);				// copy the record into unordered_map
				delete rec;
			}
		}
	}	// end of while loop
	infile.close();
	return lTotalNumberOfRecords;
}

void initializeCentroids(vector<long>& centroids,long& lTotalNumberOfRecords,int& iNumberOfCentroids,unordered_map<long,BreastCancerRecord>& records)
{
	if(iNumberOfCentroids > lTotalNumberOfRecords)
	{
		throw "The Number of Centroids cannot be greater than the Total nuumber of records in the data set\nQuitting the program!!\n";
	}
	/* initialize random seed: */
	srand ( time(NULL) );

	/* generate the centroids: */
	for(int i=0; i< iNumberOfCentroids; i++)
	{
		centroids.push_back((rand() % lTotalNumberOfRecords + 1));
		#ifdef _TRACE
			cout<<"Centroid "<<i+1<<" : "<<centroids[i]<<endl;
		#endif
	}

	// Create the Centroid Records in the table
	unordered_map<long,BreastCancerRecord>::iterator iter;
	for (int i = 0; i < centroids.size(); i++)
	{
		iter = records.find(centroids[i]);
		CentroidData.insert(*iter);
	}
	#ifdef _TRACE
		cout<<"initializeCentroids : CentroidData.size() = "<<CentroidData.size()<<endl;
		for(iter = CentroidData.begin(); iter != CentroidData.end(); iter++)
		{
			cout<<iter->first<<" "<<(iter->second).getSCM()<<endl;
		}
	#endif

	// Create the Centroid History Table to record the previous and current centroids. This is used to check for convergence
	for (int i = 1; i <= lTotalNumberOfRecords; i++) //Change to number of records
	{
		CentroidHistoryVector.push_back(0);		// Current Centroid
		CentroidHistoryVector.push_back(0);		// Previous Centroid
		std::pair<long,vector<long> > mypair(i,CentroidHistoryVector);
		CentroidHistoryTable.insert(mypair);	// copy the pair into unordered_map

		CentroidHistoryVector.clear();
	}
}

void computeNewCentroid(vector<long>& centroids,unordered_map<long,BreastCancerRecord>& records)
{
	unordered_map<long,BreastCancerRecord>::iterator centroidDataiter;
	unordered_map<long,vector<long> >::iterator historyIterator;
	for (centroidDataiter = CentroidData.begin(); centroidDataiter != CentroidData.end(); centroidDataiter++)
	{
		int cluster = centroidDataiter->first;					// Consider the first Centroid in the List of Centroids
		BreastCancerRecord *rec = &(centroidDataiter->second);

		vector<int> temp(NUMBER_OF_ATTRIBUTES,0);	// Create temporary variables for intermediate calculations
		int numberOfDataPoints = 0;					// Count the Number of datapoints that belong to a centroid

		for(historyIterator = CentroidHistoryTable.begin(); historyIterator != CentroidHistoryTable.end();historyIterator++)
		{
			vector<long> *currentDataPoint = &(historyIterator->second);

			if ((*currentDataPoint)[CURRENT_CENTROID] == cluster)
			{
				temp[0] = temp[0] + (records[historyIterator->first]).getA1();
				temp[1] = temp[1] + (records[historyIterator->first]).getA2();
				temp[2] = temp[2] + (records[historyIterator->first]).getA3();
				temp[3] = temp[3] + (records[historyIterator->first]).getA4();
				temp[4] = temp[4] + (records[historyIterator->first]).getA5();
				temp[5] = temp[5] + (records[historyIterator->first]).getA6();
				temp[6] = temp[6] + (records[historyIterator->first]).getA7();
				temp[7] = temp[7] + (records[historyIterator->first]).getA8();
				temp[8] = temp[8] + (records[historyIterator->first]).getA9();
				numberOfDataPoints++;
			}
		}
		
		// Update the Centroid Record
		// Value of each record is equal to the mean of all the attribute values that belong to this centroid.
		rec->setA1(temp[0] / numberOfDataPoints);
		rec->setA2(temp[1] / numberOfDataPoints);
		rec->setA3(temp[2] / numberOfDataPoints);
		rec->setA4(temp[3] / numberOfDataPoints);
		rec->setA5(temp[4] / numberOfDataPoints);
		rec->setA6(temp[5] / numberOfDataPoints);
		rec->setA7(temp[6] / numberOfDataPoints);
		rec->setA8(temp[7] / numberOfDataPoints);
		rec->setA9(temp[8] / numberOfDataPoints);
	}
}

void calculatePPV(vector<long>& centroids,unordered_map<long,BreastCancerRecord>& records)
{
	unordered_map<long,BreastCancerRecord>::iterator centroidDataiter;
	unordered_map<long,vector<long> >::iterator historyIterator;
	int clusterID,benignCount,malignantCount;
	double truePositive = 0, falsePositive = 0;
	for (centroidDataiter = CentroidData.begin(); centroidDataiter != CentroidData.end(); centroidDataiter++)
	{
		clusterID = centroidDataiter->first;					// Consider the first Centroid in the List of Centroids
		BreastCancerRecord *rec = &(centroidDataiter->second);
		benignCount = 0;
		malignantCount = 0;
		for(historyIterator = CentroidHistoryTable.begin(); historyIterator != CentroidHistoryTable.end();historyIterator++)
		{
			// Count the number of benign and malignant depending on the cluster.
			// If the current centroid in the centroid history table is same as the current centroid then increment the count.
			// i.e. Get the majority class for the current centroids.
			vector<long> *currentDataPoint = &(historyIterator->second);
			if ((*currentDataPoint)[CURRENT_CENTROID] == clusterID)
			{
				if((records[historyIterator->first]).getA10() == 2)		// benign
					benignCount++;
				if((records[historyIterator->first]).getA10() == 4)		// Malignant
					malignantCount++;
			}
		}
		if (benignCount >= malignantCount)
		{
			truePositive = truePositive + benignCount;
			falsePositive = falsePositive + malignantCount;
		}
		else
		{
			truePositive = truePositive + malignantCount;
			falsePositive = falsePositive + benignCount;
		}
	}
	cout<<"PPV = "<<(truePositive / (truePositive + falsePositive))<<endl;
}