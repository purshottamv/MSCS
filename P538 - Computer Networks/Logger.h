/* ============================================================================
 Name        : Logger.h
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Logger Header file
 Last modified : 9/25/2012
 ============================================================================
*/
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <fstream>
#include <stdarg.h>
#include <iostream>
#include <pthread.h>

#define INFO    "Info :"
#define WARNING "Warn :"
#define ERROR   "Error:"
#define DEBUG   "Debug:"

#define LOGMESSAGE_SIZE 100

using namespace std;

class Logger
{
    private:
        pthread_mutex_t mLock;
        char*           getTimeStamp ();
        ofstream        logfile;
        bool            bTraceMode;
		int	stdoutfd;

	public:
        Logger ();
		Logger (const char* logFilename);
        void openFile (const char* logFilename);
        virtual ~Logger ();
		void write (const char *type, const char* logline, ...);
};

#endif