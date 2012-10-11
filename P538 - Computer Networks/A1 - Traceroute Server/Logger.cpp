/* ============================================================================
 Name        : Logger.cpp
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Logger Class Definition
 Last modified : 9/25/2012
 ============================================================================
*/
#include "Logger.h"
#include <stdarg.h>
#include <unistd.h>

Logger::Logger ()
{
	bTraceMode = false;
	#ifdef _TRACE
		bTraceMode = true;
	#endif
	stdoutfd = dup(STDOUT_FILENO);
    pthread_mutex_init (&mLock, NULL);
}

Logger::Logger (const char* logFilename)
{
	bTraceMode = false;
	#ifdef _TRACE
		bTraceMode = true;
	#endif
	stdoutfd = dup(STDOUT_FILENO);
    pthread_mutex_init (&mLock, NULL);
    logfile.open (logFilename, fstream::app);
}

Logger::~Logger ()
{
    logfile.close ();
    pthread_mutex_destroy (&mLock);
}

void Logger::openFile(const char* logFilename)
{
    logfile.open (logFilename, fstream::app);	
}

void Logger::write (const char *type, const char* logline, ...)
{
    pthread_mutex_lock (&mLock);
    va_list argList;
    char    buffer [1024];
    char*   timestamp = new char [80];
    time_t  t       = time (0);
    struct  tm*     today   = localtime (&t);
    strftime    (timestamp,
                 80,
                 "%d/%m/%Y %H:%M:%S",
                 today);
    
    va_start (argList,
              logline);
    vsnprintf (buffer,
               1024,
               logline,
               argList);
    va_end (argList);
    logfile << timestamp <<" "<<type << buffer << endl;
    dprintf(stdoutfd,"%s %s %s\n",timestamp,type,buffer);
	//cout << timestamp <<" "<<type << buffer << endl;
    pthread_mutex_unlock (&mLock);
}

char* Logger::getTimeStamp ()
{
    char*           tString = new char [80];
    time_t          t       = time (0);
    struct  tm*     today   = localtime (&t);
    strftime    (tString,
                 80,
                 "%d/%m/%Y %H:%M:%S",
                 today);
    return tString;
}