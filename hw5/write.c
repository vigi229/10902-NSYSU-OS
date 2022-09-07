#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>	//for file
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#define BUF_SIZE 100

int main()
{
	int file = open("test",O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(file==-1)
	{
		printf("File open failed!\n");
		exit(1);
	}
	else
		printf("=====Open a empty file.=====\n");
		
	char *map_ptr = ( char * )mmap( 0, getpagesize(), PROT_WRITE, MAP_SHARED, file, 0 );
	if(map_ptr==MAP_FAILED)
	{
		close(file);
		printf("Map failed!\n");
		exit(1);
	}
	
	struct stat file_state={0};
	fstat(file, &file_state);
	int file_length=(int)file_state.st_size;
	
	while(1)
	{
		char buf[BUF_SIZE];
		printf("============================\n");
		printf("Input something into file...\n");
		printf("    (or type 'q' to quit)\n");
		
		fgets(buf,BUF_SIZE,stdin);
		if(buf[0]=='q'&&strlen(buf)==2)
			break;
		
		lseek(file, file_length+strlen(buf), SEEK_SET);
		write(file, "", 1);
		lseek(file, 0, SEEK_SET);
		
		int i;
		for (i=0;i<strlen(buf);i++)
		{
		    map_ptr[file_length + i] = buf[i];
		}
		file_length+=strlen(buf);
	}
	close(file);
	return 0;
}
