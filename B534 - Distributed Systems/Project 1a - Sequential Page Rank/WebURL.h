/* ============================================================================
 Name        : WebURL.h
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Sequential algorithm to calculate page rank
 ============================================================================
*/

#ifndef WEBURL_H
#define WEBURL_H

#include <iostream>
#include <string>
#include <list>

#define LARGEST_SIZE_T 18446744073709

using namespace std;

extern double danglingFactor;

class WebURL
{
	private :
		long id;								// Unique identifier for each WebURL
		double oldPageRank;					// Page Rank value from the previous iteration.
		double newPageRank;					// Page Rank value in the current iteration.
		int currentIterationNumber;				// To match the iteration state
		list<WebURL *> outboundWebURLs;		// List of OutBound web pages
	public :
		WebURL(long WebURLId, double initialPageRank);
		void appendToOutboundList(WebURL& weburl);
		long getId() const;
		list<WebURL *> getOutboundWebURLs();
		double getNewPageRank();
		double getOldPageRank() const;
		void setNewPageRank(double val);
		void setOldPageRank(double val);
		void setCurrentIterationNumber(long val);
		void evaluate(int iterationNumber, double dampingFactor, long totalURLs);
};

#endif