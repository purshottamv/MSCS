/* ============================================================================
 Name        : Server.h
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Header file for Server Class
 Last modified : 9/25/2012
 ============================================================================
*/

#ifndef _SERVER_H_
#define _SERVER_H_

#include "Socket.h"
#include "SocketException.h"
#include "Logger.h"

#include <signal.h>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include<errno.h>

#define BUFFER_SIZE 1000
#define MAX_USER	10
#define CLIENT_IDLE_TIME_OUT	30

struct rate {
	long num_requests;	
	long num_seconds;
};

struct parameters {
	long port;
	long max_users;
	bool strict_dest;
	rate r;
};

struct threadargs {
	char clientIP[INET6_ADDRSTRLEN];
	Socket socket;
	Logger *log;
	rate r;
};

void* handle_client_request(void* arg);
int execCommand(char command[], char dest[], int new_fd,threadargs& args);
bool isValidDestination(char *dest);
void print_help_guide(int new_fd);

extern bool strict_destination;
extern char separator[];

class Server
{
	private :
		parameters para;
		Socket socket,socket_client;
		struct addrinfo hints, *servinfo, *p;	// defined in netdb.h
		struct sigaction sa;
		char s[INET6_ADDRSTRLEN];
		int rv;
		Logger log;
		pthread_t threads[100];

	public :
		Server(parameters p);
		int start();
		long getmax_users();
		string getClientIPAddress();
		void *get_in_addr(struct sockaddr *sa);
		parameters getparameters();
};

class UserCount
{
    private:
        pthread_mutex_t mLock;
        int usercount;

	public:
        UserCount ()
		{
			usercount = 0;    
			pthread_mutex_init (&mLock, NULL);	
		}
        virtual ~UserCount()
		{
			pthread_mutex_destroy (&mLock);
		}
		void operator++()
		{
			pthread_mutex_lock (&mLock);
			usercount++;
			pthread_mutex_unlock (&mLock);
		}
		void operator--()
		{
			pthread_mutex_lock (&mLock);
			usercount--;
			pthread_mutex_unlock (&mLock);
		}
		int getcount()
		{	return usercount;	}
};

#endif

