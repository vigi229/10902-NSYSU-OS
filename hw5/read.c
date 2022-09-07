#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>//for file
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

int main()
{
	int file = open("test",O_RDONLY);
	if(file==-1)
	{
		printf("File open failed!\n");
		exit(1);
	}
	char *map_ptr = (char *)mmap(NULL, getpagesize(), PROT_READ, MAP_SHARED, file, 0);
	//NULL the start place.->Null:let system choose
	//getpagesize() map size
	//PROT_READ mapping palce can read

	if(map_ptr==MAP_FAILED)
	{
		close(file);
		printf("Map failed!\n");
		exit(1);
	}
	//get the state of file
	struct stat file_state={0};
	fstat(file, &file_state);
	int file_length=(int)file_state.st_size;
	close(file);
	printf("========Open the file=======\n");
	while(1)
	{
		char c;
		printf("============================\n");
		printf("type 'g' to get the content.\n");
		printf("   (or type 'q' to quit)\n");
		scanf("%c",&c);
		getchar();
		if(c=='q')
			break;
		int i;
		printf("------file's content------\n");
		if(file_length)
		for(i = 0; i < getpagesize(); i++)
			printf("%c", map_ptr[i]);
		else
		{
			printf("\x1b[;36;1;1m");
			printf("empty file\n");
			printf("\x1b[0;m");
			printf("--------------------------\n");
			exit(1);
		}
		printf("--------------------------\n");
	}
	
}
