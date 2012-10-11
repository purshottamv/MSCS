/* ============================================================================
 Name        : Socket.h
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Socket Class Declaration
 Last modified : 9/25/2012
 ============================================================================
*/
#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

class Socket
{
	private:
		int sockfd;
		struct sockaddr_storage client_address; // connector's address information. defined in socket.h
		socklen_t addr_length;

	public:
		Socket();
		virtual ~Socket();
		
		/* Server function calls	*/
		bool create(struct addrinfo *p);
		//  bool bind ( const int port );
		bool listen() const;
		bool accept ( Socket& );

		/* Client function calls	*/
		bool connect ( const std::string host, const int port );

		/*	Communication Functions */
		bool send ( const std::string ) const;
		int recv ( std::string& ) const;
		void set_non_blocking ( const bool );
		bool is_valid() const { return sockfd != -1; }
		void setsockfd(int);
		int getsockfd();
		void closeSocket();
		sockaddr_storage getclient_address();
};


#endif
