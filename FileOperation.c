#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

/************************************************************
Function Name:			read_file_to_buffer
Function description:	Read file data to buffer
Author:					Kevin
Dtat:					2016.06.27
************************************************************/
int read_file_to_buffer(char* pathname, char* file_content_ptr, int file_length)
{
	int read_bytes = 0;
	struct stat file_property = {0};
	off_t file_size = file_length;
	int fd = -1;
	
	if(pathname == NULL || file_content_ptr == NULL)
	{
		printf("Invalid parameter\n");
		return -1;
	}
	
	/* 0. Get file property */
	
	if(stat(pathname, &file_property) == -1)
	{
		printf("read_file_to_buffer: Stat error\n");
		goto ERROR;
	}
	
	/* 1. Check file type */
	if(S_ISREG(file_property.st_mode))	//Check file type if is conmmon file
	{
		if(access(pathname, R_OK) == 0)
		{
			if((fd = open(pathname, O_RDONLY, S_IRUSR)) == -1)
			{
				printf("read_file_to_buffer: Open error\n");
				goto ERROR;
			}
			
			if((read_bytes = read(fd, file_content_ptr, file_size)) == -1)
			{
				printf("read_file_to_buffer: Read error\n");
				goto ERROR;
			}
			
			close(fd);
		}
	}

	return read_bytes;

ERROR:	

	if(fd != -1)
	{
		close(fd);
	}
	
	exit(-1);
}

/************************************************************
Function Name:			get_file_length
Function description:	Get file length
Author:					Kevin
Dtat:					2016.06.27
************************************************************/
int get_file_length(char* pathname)
{
	int file_length = -1;
	struct stat file_property = {0};
	
	if(stat(pathname, &file_property) == -1)
	{
		printf("get_file_length: Stat error\n");
		return(-1);
	}
	
	file_length = file_property.st_size;
	
	return file_length;
}

/************************************************************
Function Name:			extract_file_name
Function description:	As pathname to extract pure file name. 
						Note: Should make sure file name space is big enough.
Author:					Kevin
Dtat:					2016.06.27
************************************************************/
void extract_file_name(char* pathname, char* file_name)
{
	int pathname_length = 0;
	char* name_ptr = NULL;
	
	if(pathname != NULL && file_name != NULL)
	{
		pathname_length = strlen(pathname) + 1;
		name_ptr = pathname + pathname_length;
		
		while(*name_ptr != '/')
		{
			name_ptr--;
		}
		
		name_ptr++;	//Just want to get pure file name
		
		memcpy(file_name, name_ptr, strlen(name_ptr) + 1);
	}
}
