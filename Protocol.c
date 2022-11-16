#include <stdio.h>
#include <string.h>
#include "Protocol.h"

void fill_command_frame(Frame* frame, Type type, Command command)
{
	if(frame != NULL)
	{
		memset(frame, 0, sizeof(Frame));
		
		frame->type = type;
		frame->command = command;
	}
}

void fill_response_frame(Frame* frame, Type type)
{
	if(frame != NULL)
	{
		memset(frame, 0, sizeof(Frame));
		
		frame->type = type;
	}
}

void fill_file_info_frame(Frame* frame, Type type, char* file_name,
	unsigned file_name_length, unsigned file_length)
{
	if(frame != NULL && file_name != NULL)
	{
		memset(frame, 0, sizeof(Frame));
		
		frame->type = type;
		
		strncpy(frame->file_name, file_name, file_name_length);
		
		frame->file_name_length = file_name_length;
		frame->file_length = file_length;
	}
}

void fill_data_frame(Frame* frame, Type type, char* data, unsigned data_length)
{
	if(frame != NULL && data != NULL)
	{
		memset(frame, 0, sizeof(Frame));
		
		frame->type = type;
		
		strncpy(frame->data, data, data_length);
	}
}

