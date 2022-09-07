#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#define MAXBUF_SIZE 100
#define INPUT_SIZE 1000
#define COMMAND_SIZE 100
#define STD_INPUT 0
#define STD_OUTPUT 1


static char command[INPUT_SIZE];
static char *process1[COMMAND_SIZE],*process2[COMMAND_SIZE],*file;
static bool EXIT=0,BackGround=0,PipeLine=0,Redir_In=0,Redir_Out=0;

void type_prompt();
void read_command();
void initialize();
void pipeline();

int main()
{
	while(1)
	{
		type_prompt();
		read_command();
		if(EXIT) break;
		else
		{
			if(!strcmp(process1[0], "cd"))
			{
				if(chdir(process1[1])==-1)
					printf("[Error] Can not find it.\n");
				continue;	
			}
			pid_t pid;
			pid = fork();
			int status;
			if (pid > 0) {
			    if(!BackGround)
			    	waitpid(pid, &status, 0); 
			    else
			    	printf("[%u]\n",pid);
			}
			else if (pid == 0) {
			    if(PipeLine)
			    	pipeline();
				else if(Redir_Out)
				{
					int outFile=open(file,O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, 0666);
					dup2(outFile,STD_OUTPUT);
					execvp(process1[0],process1);
					close(outFile);
				}
				else if(Redir_In)
				{
					int inFile = open(file, O_RDONLY, 0666);
					dup2(inFile, STD_INPUT);
					execvp(process1[0], process1);
					close(inFile);
				}
				else
					execvp(process1[0],process1);
			}
			else {
				 printf("[Error] Fork Error\n");
			}
			
		}
	}
}

void type_prompt()
{
	char buf[ MAXBUF_SIZE ];
	getcwd(buf, MAXBUF_SIZE);
	printf("\033[1;32m""MyShell:\033[0m""\033[1;34m~%s\033[0m""$ ",buf);
}
void read_command()
{
	initialize();
	if(fgets(command,INPUT_SIZE,stdin))
	{
		if(command[strlen(command) - 1] == '\n')
			command[strlen(command) - 1] = '\0';
		if(!strcmp(command, "exit") )
		{
			EXIT = 1;
			exit(0);
		}
		char* split = strtok(command," \t\r\n\a");
		int p1=0,p2=0;
		while(split!=NULL)
		{
			
			if(!strcmp(split,"<"))
			{
				Redir_In=1;
				split = strtok(NULL," \t\r\n\a");
				file=split;
			}
			else if(!strcmp(split,">"))
			{
				Redir_Out=1;
				split = strtok(NULL," \t\r\n\a");
				file=split;
			}
			else if(!strcmp(split,"|"))
				PipeLine=1;
			else if (!strcmp(split,"&"))
				BackGround=1;
			else
			{
				if(PipeLine) 
				{
					process2[p2]=split;
					p2++;
				}
				else
				{
					process1[p1]=split;
					p1++;
				}
			}
			split = strtok(NULL," \t\r\n\a"); //NULL keep read 
		}
	}
	else
	{
		printf("[Error] fail input\n");
		exit(0);
	}
	
}
void initialize()
{
	memset(command,'\0',sizeof(char)*INPUT_SIZE);
	memset(process1,'\0',sizeof(char *)*COMMAND_SIZE);
	memset(process2,'\0',sizeof(char *)*COMMAND_SIZE);
	file=NULL;
	EXIT=0;BackGround=0;PipeLine=0;Redir_In=0,Redir_Out=0;
}
void pipeline()
{
	int fd[2];
	pipe(&fd[0]);
	pid_t pid;
	pid = fork();
	int status;
	if (pid > 0) {			//parent   do process2(read from pipe)
		//waitpid(pid, &status, 0);
		close(fd[1]); // process 2 does not need to write to pipe
		dup2(fd[0], STD_INPUT); // instead standard input by fd[0]
		close(fd[0]); // this file descriptor not needed any more
		if(Redir_Out)
		{
			int outFile=open(file,O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, 0666);
			dup2(outFile,STD_OUTPUT);
			execvp(process2[0],process2);
			close(outFile);
		}
		else
			execvp(process2[0], process2);
	}
	else if (pid == 0) {	//child	  do process1(write to pipe)
		close(fd[0]);
		dup2(fd[1], STD_OUTPUT);
		close(fd[1]);
		//execvp(process1[0], process1);
		if(Redir_In)
		{
			int inFile = open(file, O_RDONLY, 0666);
			dup2(inFile, STD_INPUT);
			execvp(process1[0], process1);
			close(inFile);
		}
		else
			execvp(process1[0], process1);
	}
	else {
		 printf("[Error] Fork Error\n");
	}
}
