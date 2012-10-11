/*
 ============================================================================
 Name        : MyShell.c
 Author      : Purshottam Vishwakarma
 Version     :
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : This is a shell with bare minimum functionalities
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>

#define MAXPIPES 100
#define MAXARGS 10
#define MAXCHAR	300

typedef void (*sighandler_t)(int);

void handle_signal(int signalno)
{
	printf("\n");
	fflush(stdout);
	return;
}

int main(int argc,char *argv[],char *envp[])
{

	char command[MAXCHAR];
	char **cmd;
	int i = 0;
	pid_t childpid, status;
	char **ParseCommand(char *);
	char **ParseCommand2(char *,int *);
	int ExecuteCommand(int,char **);

	while(1)
	{
		signal(SIGINT, handle_signal);	
		printf("pshell$");
		for(i=0;i < MAXCHAR;i++)
		{
			command[i] = getchar();
			if(command[i] == '\n')
			{
				command[i] = '\0';
				break;
			}else if(command[i] == EOF)
			{
				printf("\n");
				exit(1);
			}
		}
		if(strcmp(command,"exit") == 0 || strcmp(command,"logout") == 0)
			exit(1);

		cmd = ParseCommand(command);
		for(i=0; cmd[i] != NULL && strlen(cmd[i]) > 0; i++)
		{
			int ntokens = 0,j=0,result;
			char **subcommands;
			subcommands = ParseCommand2(cmd[i],&ntokens);
			result = ExecuteCommand(ntokens,subcommands);
			free (subcommands);
		}
		free (cmd);
	}

	return EXIT_SUCCESS;
}

char **ParseCommand(char *command)
{

	int i = 0;
	char **cmd;
	char *cmd_tmp;

	if ((cmd = (char **)malloc((i + 1) * sizeof(char *))) == NULL) {
		perror("malloc failed");
	}

	cmd_tmp = command;

	for (;cmd[i] = strsep(&cmd_tmp, ";");)
	{
		i++;
		if ((cmd = (char **)realloc(cmd, (i + 1)*sizeof(char *)))
			== NULL) {
			perror("realloc failed");
		}
	}
	return cmd;
}

char **ParseCommand2(char *command, int *count)
{
	int i = 0;
	char delim[6]={' ','|','<','>','&','\0'};
	char **cmd, *cmd_tmp, *index, *end = NULL;
	char *getnexttoken(char **,char *);

	if ((cmd = (char **)malloc((i + 1) * sizeof(char *))) == NULL) {
		perror("malloc failed");
	}

	cmd_tmp = command;
	end = cmd_tmp + strlen(cmd_tmp);
	index = getnexttoken(&cmd_tmp,&delim);
	cmd[i] = (char *)malloc(index-cmd_tmp+1);
	strncpy(cmd[i],cmd_tmp,index-cmd_tmp);
	cmd[i][index-cmd_tmp] = '\0';
	cmd_tmp = index;
	i++;
	while(index != end)
	{
		index = getnexttoken(&cmd_tmp,&delim);
		if(cmd_tmp == end)		// If the user types spaces after the command
			break;
		if ((cmd = (char **)realloc(cmd, (i + 1)*sizeof(char *)))
			== NULL)
		{
			perror("realloc failed");
		}
		cmd[i] = (char *)malloc(index-cmd_tmp+1);
		strncpy(cmd[i],cmd_tmp,index-cmd_tmp);
		cmd[i][index-cmd_tmp] = '\0';
		cmd_tmp = index;
		i++;
	}
	*count = i;
	return cmd;
}

char *getnexttoken(char **cmd_tmp,char *delim)
{
	char *index;
	char *space = " ";
	for(;strncmp(*cmd_tmp,space,1) == 0; )
		(*cmd_tmp)++;
	index = strpbrk(*cmd_tmp,delim);
	if(index == NULL)  //it is the last token
		index = (*cmd_tmp) + strlen(*cmd_tmp);
	else if(index == *cmd_tmp)
	{
		if(strncmp(index,">",1) == 0 && strncmp(index+1,">",1) == 0)
			index = index + 2;
		else if((strncmp(index,">",1) == 0) || (strncmp(index,"<",1) == 0) || (strncmp(index,"|",1) == 0) || (strncmp(index,"&",1) == 0))
			index++;
	}
	return index;
}

int ExecuteCommand(int nargc,char *sargv[])
{
	int token = 0,j=0,x=0,i = 0,argiterator = 0, backgroundflag = 0;
	char *command = NULL;
	char *args[10];
	int fdwrite = -1, fdread = -1, readpipe = 0,writepipe = 0;
	pid_t pid;
	int status;
	int pipelocation[100],pipeiterator = 0, totalpipes = 0, nextpipelocation; // This program will not support more than 100 pipes
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	int	commpipe[2][2];
	int readflag = 0;
	
	// Inititalize the arrays
	for(x=0;x<MAXARGS;x++)
		args[x] = NULL;
	for(x=0;x<MAXPIPES;x++)
		pipelocation[x] = -1;

	// search for all the pipe locations
	for(x=0;x<nargc;x++)
	{
		if(strcmp(sargv[x],"|") == 0) // there is a pipe at this location. Mark it.
			pipelocation[pipeiterator++] = x;
	}
	totalpipes = pipeiterator;
	pipeiterator = 0;

	if(strpbrk(sargv[token],"<>>!@#%^&*()_+:?") != NULL)   // First character of the command should not be a special character
	{
		printf("Error Unexpected Token\n");
		return (1);
	}
	
	while(token < nargc)
	{
		// Check if there is a pipe in the immediate future. In that case we need to change the file descriptor using dup2
		if(pipelocation[pipeiterator] >= 0)
		{
			writepipe = 1;
			nextpipelocation = pipelocation[pipeiterator];
		}
		else
			nextpipelocation = nargc;

		if(pipeiterator > 0 && pipelocation[pipeiterator - 1] >= 0)
			readpipe = 1;			

		if((pipeiterator%2) == 0)
			pipe(commpipe[0]);	// Create Pipe
		else
			pipe(commpipe[1]);	// Create Pipe
		
		command = (char *)malloc(strlen(sargv[token])+1);
		strcpy(command,sargv[token]);
		command[strlen(sargv[token])] = '\0';

		token++;
		while(token < nargc && token < nextpipelocation)
		{
			if(strcmp(sargv[token],"<") == 0 || strcmp(sargv[token],">") == 0 || strcmp(sargv[token],">>") == 0 || strcmp(sargv[token],"&") == 0)
			{
				//create file discriptor
				if(strcmp(sargv[token],"<") == 0)
				{
					// initialize the input file descriptor
					if((token+1)<nargc)
					{
						fdread = open(sargv[++token],O_RDONLY);
						if(fdread == -1)
						{
							perror("Error Opening file ");
							return 1;
						}
					}
					else
					{
						printf("Syntax error near unexpected token 'nwline'\n");
						return (1);
					}
				}
				else if(strcmp(sargv[token],">") == 0)
				{
					// Initialize the output file descriptor in truncate mode
					if((token+1)<nargc)
					{
						fdwrite = open(sargv[++token],O_RDWR | O_CREAT | O_TRUNC, mode);
						if(fdwrite == -1)
						{
							perror("Error Opening file ");
							return 1;
						}
					}
					else
					{
						printf("Syntax error near unexpected token 'nwline'\n");
						return (1);
					}
				}
				else if(strcmp(sargv[token],">>") == 0)
				{
					// Initialize the output file descriptor in append mode
					if((token+1)<nargc)
					{
						fdwrite = open(sargv[++token],O_RDWR | O_CREAT |O_APPEND, mode);
						if(fdwrite == -1)
						{
							perror("Error Opening file ");
							return 1;
						}
					}
					else
					{
						printf("Syntax error near unexpected token 'nwline'\n");
						return (1);
					}
				}
				else if(strcmp(sargv[token],"&") == 0)
				{
					if((token+1) < nargc && strcmp(sargv[token+1],"|") == 0)
					{
							printf("Syntax error near unexpected token '|'\n");
							return (1);
					}
					backgroundflag = 1;
				}
				token++;
				continue;
			}
			args[argiterator] = (char *)malloc(strlen(sargv[token])+1);
			strcpy(args[argiterator],sargv[token]);
			args[argiterator][strlen(sargv[token])] = '\0';
			token++;
			argiterator++;
		}  // end of while loop

		// Process the cd command before forking. In the fork ignore the cd command
		//if((strncmp(command,"cd",2) == 0))
		if((strcmp(command,"cd") == 0))
		{
			printf("%s %d\n",command,nargc);
			if(totalpipes == 0 && nargc <= 2)
			{
				if(nargc == 1)
				{
					const char* home = getenv("HOME");
					chdir(home);
				}
				else
				{
					if(chdir(sargv[1]) == -1)
						perror("Invalid path ");
				}
			}
			else
			{
				printf("Syntax Error \n");
			}
			free(command);
			command = (char *)malloc(strlen("pwd")+1);
			strcpy(command,"pwd");
			command[strlen("pwd")] = '\0';
			for(x=0;x<argiterator;x++)
			{
				free(args[x]);
				args[x] = NULL;
			}
		}
		// Execute the command.
		if( (pid=fork()) == -1)
		{
			perror("Fork error. Cannot create child process ");  // something went wrong
			return 1;        
		}
		if(pid == 0)	// Child
		{
			if(fdwrite != -1)
				dup2(fdwrite,STDOUT_FILENO);
			if(fdread != -1)
				dup2(fdread,STDIN_FILENO); 

			if(readpipe && fdread == -1)
			{
				dup2(commpipe[(pipeiterator-1)%2][0],STDIN_FILENO);
			}
			if(writepipe && fdwrite == -1)
			{
				dup2(commpipe[pipeiterator%2][1],STDOUT_FILENO);
			}
/*			if(strncmp(command,"cd",2) == 0)
			{
				exit(1);
			}
*/			if(execlp(command,command,args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9],NULL) == -1)
			{
				perror("Command Not Found ");
				exit(1);
			}
			exit(0);  // exit the child after its execution
		}
		else	// Parent
		{
			if(!backgroundflag)
				wait(&status);				// Wait for child process to end
			if(writepipe)
				close(commpipe[pipeiterator%2][1]);
			if(readpipe)
				close((commpipe[(pipeiterator-1)%2][0]));
		}
		pipeiterator++;
		token++;
		fdread = -1;
		fdwrite = -1;
		argiterator = 0;
		for(x=0;x<argiterator;x++)
			free(args[x]);
		for(x=0;x<MAXARGS;x++)
			args[x] = NULL;
		writepipe = 0;
		readpipe = 0;
		backgroundflag = 0;

		free(command);
	}
	return 0;
}