/* ============================================================================
 Name        : WebURL.h
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Sequential algorithm to calculate page rank
 ============================================================================
*/

#include "WebURL.h"

WebURL::WebURL(long WebURLId, double initialPageRank)
{
	id = WebURLId;
	oldPageRank = initialPageRank;
	newPageRank = initialPageRank;
	currentIterationNumber = 0;
}

void WebURL::appendToOutboundList(WebURL& weburl)
{
	outboundWebURLs.push_back(&weburl);	
	#ifdef _TRACE
		cout<<weburl.getId()<<" added to the outbound list of "<<id<<endl;
	#endif
}
long WebURL::getId() const
{
	return id;
}
std::list<WebURL *> WebURL::getOutboundWebURLs()
{
	return outboundWebURLs;
}

double WebURL::getNewPageRank()
{  return newPageRank;	}

double WebURL::getOldPageRank() const
{  return oldPageRank;	}

void WebURL::setNewPageRank(double val)
{	
	newPageRank = val;	
}

void WebURL::setOldPageRank(double val)
{	oldPageRank = val;	}

void WebURL::setCurrentIterationNumber(long val)
{	currentIterationNumber = val;	}

void WebURL::evaluate(int iterationNumber, double dampingFactor, long totalURLs)
{
	if(currentIterationNumber == 0)
	{
		// This is the special case. Will be evaluated only on the first iteration
		currentIterationNumber = iterationNumber;
		oldPageRank = newPageRank;
		newPageRank = 0.0;
	}
	else if(currentIterationNumber < iterationNumber)
	{
		currentIterationNumber = iterationNumber;
		oldPageRank = dampingFactor * (newPageRank + (double)(danglingFactor/totalURLs)) + ((1.0 - dampingFactor) * (double)(1.0/totalURLs));
		newPageRank = 0.0;
	}
}