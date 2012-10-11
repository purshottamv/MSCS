/* ============================================================================
 Name        : Socket.cpp
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Socket Class Method definitions
 Last modified : 9/25/2012
 ============================================================================
*/
#include "Socket.h"
#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>

Socket::Socket() :  sockfd ( -1 )
{

}

Socket::~Socket()
{
//  if ( is_valid() )
//	  ::close ( sockfd );
//  cout<<"Server : Closing the socket "<<sockfd<<endl;
}

bool Socket::create(struct addrinfo *p)
{
	int yes = 1;
	if((sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol))== -1)
	{
		perror("server: socket");
		return false;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) 
	{
		perror("Server : Error in setsockopt");
		return false;
	}

	if(bind(sockfd, p->ai_addr,p->ai_addrlen)== -1)
	{
		close(sockfd);
		perror("Server: Failed to bind ");
		return false;
	}
	return true;
}

/*bool Socket::bind ( const int port )
{

  if ( ! is_valid() )
    {
      return false;
    }



  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.s_addr = INADDR_ANY;
  m_addr.sin_port = htons ( port );

  int bind_return = ::bind ( sockfd,
			     ( struct sockaddr * ) &m_addr,
			     sizeof ( m_addr ) );


  if ( bind_return == -1 )
    {
      return false;
    }

  return true;
}

*/
bool Socket::listen() const
{
  if ( ! is_valid() )
    {
      return false;
    }

  int listen_return = ::listen ( sockfd, MAXCONNECTIONS );


  if ( listen_return == -1 )
    {
      return false;
    }

  return true;
}


bool Socket::accept ( Socket& new_socket )
{
	addr_length = sizeof(struct sockaddr_storage);
	int sockid;
	sockid = ::accept ( sockfd, (struct sockaddr*)&client_address ,&addr_length);
	if ( sockid <= 0 )
		return false;
	else
	{
		new_socket.setsockfd(sockid);
		return true;
	}
}


bool Socket::send ( const std::string s ) const
{
  int status = ::send ( sockfd, s.c_str(), s.size(), MSG_NOSIGNAL );
  if ( status == -1 )
    {
      return false;
    }
  else
    {
      return true;
    }
}


int Socket::recv ( std::string& s ) const
{
  char buf [ MAXRECV + 1 ];

  s = "";

  memset ( buf, 0, MAXRECV + 1 );

  int status = ::recv ( sockfd, buf, MAXRECV, 0 );

  if ( status == -1 )
    {
      std::cout<<"status == -1   errno == " << errno << "  in Socket::recv\n";
      return 0;
    }
  else if ( status == 0 )
    {
      return 0;
    }
  else
    {
      s = buf;
      return status;
    }
}



bool Socket::connect ( const std::string host, const int port )
{
//  if ( ! is_valid() ) return false;
//
//  m_addr.sin_family = AF_INET;
//  m_addr.sin_port = htons ( port );
//
//  int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );
//
//  if ( errno == EAFNOSUPPORT ) return false;
//
//  status = ::connect ( sockfd, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );
//
//  if ( status == 0 )
//    return true;
//  else
//    return false;
}

void Socket::set_non_blocking ( const bool b )
{

  int opts;

  opts = fcntl ( sockfd,
		 F_GETFL );

  if ( opts < 0 )
    {
      return;
    }

  if ( b )
    opts = ( opts | O_NONBLOCK );
  else
    opts = ( opts & ~O_NONBLOCK );

  fcntl ( sockfd,
	  F_SETFL,opts );

}

void Socket::setsockfd(int s)
{
	sockfd = s;
}

void Socket::closeSocket()
{
  if ( is_valid() )
	  ::close ( sockfd );
}

sockaddr_storage Socket::getclient_address()
{
	return client_address;
}

int Socket::getsockfd()
{
	return sockfd;
}