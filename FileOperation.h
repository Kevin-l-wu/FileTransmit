#ifndef FILE_OPERATION_H_
#define FILE_OPERATION_H_

#define FILE_NAME_MAX_LENGTH 255

int read_file_to_buffer(char* pathname, char* file_content_ptr, int file_length);

int get_file_length(char* pathname);

#endif