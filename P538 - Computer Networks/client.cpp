/* Client program for P538 first project */

#include<iostream>
#include<sstream>
#include<cstdlib>
#include<stdio.h>
#include<string.h>
#include<getopt.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<sys/stat.h>
#include<arpa/inet.h>
#include<errno.h>
#include<sys/wait.h>
#include<signal.h>
#include<vector>

using namespace std;

#define MAXDATASIZE 200

char port[10];

void help_usage()
{
	cout << "To use the client: ./client <hostname> <port>" << endl;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char* argv[])
{
	int sockfd, numbytes;  
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char client[INET6_ADDRSTRLEN];
	char server_name[100];
	char *end_ptr;
	int base = 10;
	void* request_handler(void* arg);

	if(argc == 3)
	{
		strcpy(server_name, argv[1]);
		strcpy(port, argv[2]);
		cout << "hostname: " << server_name << " port: " << port << endl;
	}
	else
	{
		help_usage();
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}
	
		//cout << "socket : " << sockfd << endl;

		break;
	}

	//cout << "connect executed successfully" << endl;

	if(p == NULL)	
	{
		cout << "Client failed to connect!" << endl;
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),client, sizeof client);
	cout << "client connecting to " << client << endl;

	char input[200];
	int buffersize = 1500;
	char buffer[buffersize];
	int flags = 0;
	pthread_t threads[2];

	pthread_create(&threads[0], NULL, request_handler, &sockfd);
	pthread_detach(threads[0]);		// let thread do its own work and clean up after work is done

	while(true)
	{
		//cout << "Inside true loop" << endl;
		//cin >> input;
		numbytes = recv(sockfd, buffer, buffersize-1, flags);
//		cout<<"numbytes = "<<numbytes<<"\n\n";
		buffer[numbytes] = '\0';
//		cout<<"Hey I received this : numbytes = "<<numbytes<<"\n\n";
		cout << buffer;
		if(strcmp(buffer,"Inactivity timeout!") == 0 || numbytes == 0)
		{
			exit(1);
			close(sockfd);
		}
		memset(buffer, '\0', sizeof(buffer));
	}
}

void* request_handler(void* arg)
{
	int sockfd = *((int *)arg);
//	cout<<"Reading the command on port "<<sockfd<<"\n";
	string str_in;
	char input[200];
	int flags = 0;

	while(1)
	{
//		cout << "Please enter the next command:\n"<< endl;
		getline(cin, str_in);
		strcpy(input, str_in.c_str());

		if (send(sockfd,input , strlen(input), flags) == -1)
					 perror("send");
	}
}