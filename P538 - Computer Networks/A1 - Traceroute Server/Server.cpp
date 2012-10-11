/* ============================================================================
 Name        : Server.cpp
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Server Class Method definitions. All the show happens here.
 Last modified : 9/25/2012
 ============================================================================
*/

#include "Server.h"
#include <vector>

UserCount user_count;

Server::Server(parameters p)
{
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;		// use my IP
	para = p;
	log.openFile("server.log");
}

int Server::start()
{
	char PORT[10];
	char msg[LOGMESSAGE_SIZE];
	int retval;
	sprintf(PORT,"%ld",getparameters().port);
	long max_users = getmax_users();

	if((retval = getaddrinfo(NULL,PORT,&hints,&servinfo))!=0)
	{
		sprintf(msg,"getaddrinfo error: %s",gai_strerror(retval));
		throw  msg;
	}

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) 
	{
        if ((socket.create(p)) == false) 
            continue;
		else
			break;
    }

	if(p == NULL)	
	{
		cerr<<"server: failed to bind\n";
		return false;
	}

	freeaddrinfo(servinfo);

	if((socket.listen()) == false)
	{
		perror("server : Failed to listen ");
		return false;
	}
	sprintf(msg,"Listening for incoming connections on port %ld",getparameters().port);
	log.write(INFO,msg);
	
	while(true)
	{
		if(!socket.accept(socket_client))
		{
			perror("Server : Failed to listen ");
			log.write(ERROR,"Server : Error in accepting the connection.");
		}
		else
		{
			sprintf(msg,"Server : Accepted the connection from %s on port %d",getClientIPAddress().c_str(),socket_client.getsockfd());
			log.write(INFO,msg);
		}

		sprintf(msg,"Number of users logged on to the server: %d",(user_count.getcount() + 1));
		log.write(INFO,msg);
		if(user_count.getcount() > max_users - 1)
		{
			dprintf(socket_client.getsockfd(), "Connection refused. Reached maximum number of connections at the server. Please try again later.\n");
			socket_client.closeSocket();		// Closing the newly created socket

			sprintf(msg,"Server : Connection refused for %s. Reached maximum number of connections. Please try again later.",getClientIPAddress().c_str());
			log.write(WARNING,msg);
			continue;
		}
		threadargs args;
		//args.socket = &socket_client;
		args.socket = socket_client;
		sprintf(args.clientIP,"%s",getClientIPAddress().c_str());
		args.log = &log;
		args.r = getparameters().r;

		pthread_create(&threads[user_count.getcount()], NULL, handle_client_request, &args);
		++user_count;
	}
}

long Server::getmax_users()
{
	return para.max_users;
}

string Server::getClientIPAddress()
{
	char client[INET6_ADDRSTRLEN];
	struct sockaddr_storage client_addr = socket.getclient_address();

	// Convert IP addresses to human-readable form
	inet_ntop(client_addr.ss_family,get_in_addr((struct sockaddr*)(&client_addr)),client,INET6_ADDRSTRLEN);
	string str(client);
	return str;
}

void *Server::get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

parameters Server::getparameters()
{
	return para;
}

/* This function handles the request from the client. Called by pthread_create */
void* handle_client_request(void* arg)
{
	char receiveBuffer[BUFFER_SIZE];
	threadargs args = *((threadargs*)arg);
	int flags = 0;
	int receiveBufferSize;
	int sockfd = args.socket.getsockfd();
	char msg[LOGMESSAGE_SIZE];

	/*		Begin section for Rate Limit		*/
	long seconds = 0;
	struct timeval current_time;
	int commandcount = 0;
	vector<struct timeval> timestamps;
	/*		Begin section for Rate Limit		*/

	sprintf(msg,"handle_client_request : Received connection from %s on port %d",args.clientIP,sockfd);
	args.log->write(INFO,msg);

	struct timeval timeout;
	timeout.tv_sec = CLIENT_IDLE_TIME_OUT;
	timeout.tv_usec = 0;
	
	// Set options on sockets
	if(setsockopt(sockfd, SOL_SOCKET,SO_RCVTIMEO,&timeout, sizeof(timeout))< 0)
	{
		perror("handle_client_request : timeout");
	}

	while(true)
	{
		if((receiveBufferSize = recv(sockfd,(void*)receiveBuffer,BUFFER_SIZE-1,flags)) != -1)
		{
			/*		Begin section for Rate Limit		*/
			gettimeofday(&current_time, NULL);
			if(commandcount < args.r.num_requests)
			{
				timestamps.push_back(current_time);
			}
			seconds = current_time.tv_sec - timestamps[0].tv_sec;
			/*		End section for Rate Limit		*/

			if(errno == EWOULDBLOCK)
			{
				dprintf(sockfd, "Time out. You have been idle for more than %d seconds. Disconnecting...\n",CLIENT_IDLE_TIME_OUT);
				sprintf(msg,"Time out. Client %s is idle for more than %d seconds. Disconnecting. Port %d",args.clientIP, CLIENT_IDLE_TIME_OUT,args.socket.getsockfd());
				args.socket.closeSocket();
				args.log->write(INFO,msg);
				--user_count;
				pthread_exit(NULL);
			}
			else if(seconds < args.r.num_seconds && commandcount > args.r.num_requests)		// Added for Rate Limit
			{
				dprintf(sockfd, "Command cannot be processed as number of requests exceeded. %ld requests / %ld seconds\n",args.r.num_requests,args.r.num_seconds);
				sprintf(msg,"Command cannot be processed as number of requests exceeded. %ld requests / %ld seconds\n",args.r.num_requests,args.r.num_seconds);
				args.log->write(INFO,msg);
			}
			else if(receiveBufferSize >= 2)
			{
				receiveBuffer[receiveBufferSize-2] = '\0';
				commandcount++;
				sprintf(msg,"Received Command from %s : %s",args.clientIP, receiveBuffer);
				args.log->write(INFO,msg);
				/*		Begin section for Rate Limit	*/
				commandcount++;
				if(commandcount >= args.r.num_requests)
				{
					timestamps.erase(timestamps.begin());
					timestamps.push_back(current_time);
				}
				/*		End section for Rate Limit		*/

				// Execute the command
				if(execCommand(receiveBuffer, args.clientIP,  sockfd, args))
				{
					sprintf(msg,"Disconnecting %s on port %d",args.clientIP,sockfd);
					args.socket.closeSocket();
					args.log->write(INFO,msg);
					--user_count;
					pthread_exit(NULL);
				}
			}
		}
	}
}

/* executes the command issued by the client */
int execCommand(char command[], char dest_addr[], int new_fd, threadargs& args)
{
	int commandLength = strlen(command);
	char* token1 = strtok(command," ");
	char* token2 = strtok(NULL," ");
	char msg[LOGMESSAGE_SIZE];
	char buffer[BUFFER_SIZE-1];
	bool valid_command = false;
	bool valid_dest = false;
	bool is_traceroute_me = false; // check if command issued is traceroute me

	if(token1!=NULL)
	{
		if(strcmp("traceroute",token1) == 0)
		{
			valid_command = true;

			if(token2!=NULL)
			{
				if(isValidDestination(token2))
				{
					valid_dest = true;
					strcpy(buffer, "traceroute ");
					strcat(buffer, token2);

					if(strcmp("traceroute me", buffer)== 0)
					{
						strcpy(buffer, "traceroute ");
						strcat(buffer, dest_addr);
						is_traceroute_me = true;
					}

					if(strcmp(dest_addr, token2) == 0)
					{
						is_traceroute_me = true;
					}

					if((dup2(new_fd,STDOUT_FILENO)>=0) && (dup2(new_fd, STDERR_FILENO)>= 0))
					{
						char time_buffer[80];

						/* checks if file is present on the sever local dir */
						if(!access(token2, F_OK))
						{
							string str;
							ifstream ifs(token2);

							if(ifs.is_open())
							{
								ofstream logFile;
								logFile.open("logfile.txt",ios::app);

								while(ifs.good())
								{
									getline(ifs,str);
									if(str.length() > 11)
									{
										const char* ch = str.c_str();
										sprintf(msg,"Client %s : Executing from file : %s",args.clientIP,ch);
										args.log->write(INFO,msg);
										cout << endl << "Tracing......" << endl;
										system(ch);
										cout << separator << endl;
									}
								}
								logFile.close();
							}
						} 
						else 
						{	
							if(strict_destination == false || (strict_destination == true && is_traceroute_me == true))					
							{
								sprintf(msg,"Client %s : Executing %s",args.clientIP, buffer);
								args.log->write(INFO,msg);
								cout << endl << "Tracing......" << endl;
								system(buffer);			// Executes the command
								cout << separator << endl;
							}
							else
							{
								dprintf(new_fd, "Sorry, the 'strict_dest' flag has been enabled.\nYou can only traceroute your own IP address.\nFor e.g. traceroute me\n");
								sprintf(msg,"Client %s : Sorry, the 'strict_dest' flag has been enabled.\nYou can only traceroute your own IP address.\nFor e.g. traceroute me\n",args.clientIP);
								args.log->write(INFO,msg);
							}
						}
					}
				}
			}
		}
		else if(strcmp("help",token1)== 0)
		{
			print_help_guide(new_fd);
			return EXIT_SUCCESS;
		}
		else if(strcmp("quit", token1) == 0)
		{
			return EXIT_FAILURE;
		}
	}

	if(valid_command && (!valid_dest))
	{
		// Logging 
		dprintf(new_fd, "Invalid dest: dest should only consist of letters, numbers and the characters '.' and '-'\n");
		sprintf(msg,"Client %s : Invalid Destination \"%s\".",args.clientIP, buffer);
		args.log->write(INFO,msg);
		return EXIT_SUCCESS;
	}

	if(!valid_command)
	{
		sprintf(msg,"Client %s : Invalid Command \"%s\"",args.clientIP, command);
		args.log->write(INFO,msg);
		dprintf(new_fd,"The command you entered is not valid. Try help for more information\n\n");
	}

	return EXIT_SUCCESS;
}

/* checks whether the destination entered by the user is valid */
bool isValidDestination(char *dest)
{		
	int i;
	int len = strlen(dest);
	bool valid = true;

	for(i = 0; i < len; i++)
	{
		if((!isalnum(dest[i])) && (dest[i]!= '.') && (dest[i]!='-'))
		{
			valid = false;
			break;
		}	
	}

	return valid;
}

/* prints the usage guide for the client */
void print_help_guide(int new_fd)
{
	dprintf(new_fd, "\n");
	dprintf(new_fd,"\tThe following commands are supported by this server:\n\n");
	dprintf(new_fd, "\ttraceroute [destination machine]:\tPerforms traceroute for the destination machine.\n");
	dprintf(new_fd,"\ttraceroute [file name]:\t\t\tLoops through the file and displays traceroute results for each traceroute command in file.\n");
	dprintf(new_fd,"\ttraceroute me:\t\t\t\tPerforms traceroute using your hostname as target.\n");
	dprintf(new_fd, "\thelp:\t\t\t\t\tDisplay help options.\n");
	dprintf(new_fd, "\tquit:\t\t\t\t\tWill close the session.\n");
	dprintf(new_fd,"\n");
}