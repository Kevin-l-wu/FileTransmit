#ifndef FILE_TRANSMIT_COMMON_H_
#define FILE_TRANSMIT_COMMON_H_

#define PORT 3333
#define IP_ADDR_STR_LENGTH 16

int str_digit_check(char* str, unsigned length);

int ip_addr_valid_check(char* ip_addr, unsigned length);

void extract_file_name(char* pathname, char* file_name);

void MemDump(void* memPtr, unsigned int len);


#endif
