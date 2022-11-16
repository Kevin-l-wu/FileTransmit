#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define DATA_LENGTH 512
#define FILE_NAME_LENGTH 255

typedef enum _Command
{
	UPLOAD_FILE,
	DOWNLOAD_FILE,
} Command;

typedef enum _Type
{
	REQUEST = 0,
	RESPONSE_OK,
	RESPONSE_ERROR,
	DATA,
	TYPE_NONE,
} Type;

typedef struct _Frame
{
	Type type;
	Command command;
	char file_name[255];
	unsigned file_name_length;
	unsigned file_length;
	char data[DATA_LENGTH];
	unsigned actual_data_length;
} Frame;

void fill_command_frame(Frame* frame, Type type, Command command);
void fill_response_frame(Frame* frame, Type type);
void fill_file_info_frame(Frame* frame, Type type, char* file_name,
	unsigned file_name_length, unsigned file_length);
void fill_data_frame(Frame* frame, Type type, char* data, unsigned data_length);

#endif