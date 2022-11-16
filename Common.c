#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Common.h"

int str_digit_check(char* str, unsigned length)
{
	int ret = 1;
	unsigned i = 0;
	
	for(i = 0; i < length; i++)
	{
		if(str[i] > '9' || str[i] < '0')
		{
			ret = 0;
			break;
		}
	}
	
	return ret;
}

int ip_addr_valid_check(char* ip_addr, unsigned length)
{
	int ret = 1;
	unsigned i = 0;
	char ip_addr_temp[IP_ADDR_STR_LENGTH] = {'\0'};
	char* str = ip_addr_temp;
	
	strncpy(ip_addr_temp, ip_addr, length);
	
	for(i = 0; i < length; i++)
	{
		if(ip_addr_temp[i] == '.' || ip_addr_temp[i] == '\0')
		{
			str = strtok(str, ".");
			
			if(str_digit_check(str, strlen(str)))
			{
				if(atoi(str) > 255 || atoi(str) < 0)
				{
					ret = 0;
					break;
				}
			}
			else
			{
				ret = 0;
				break;	
			}
			
			str = ip_addr_temp + i + 1;
		}	
	}

	return ret;
}

void MemDump(void* memPtr, unsigned int len)
{
	unsigned int index = 0;

	char* memPtrTemp = memPtr;

	if((memPtr != NULL) && (len > 0))
	{
		printf("Dump ptr: 0x%p Lenth: 0x%x\n", memPtrTemp, len);

		for(index = 0; index < len; index++)
		{
			if((index % 16) == 0)
			{
				printf("\n");
				printf("0x%08x-0x%08x: ", index, (index + 15));
			}
			printf("0x%02x ", (char)(*(memPtrTemp + index)));
		}
		printf("\nDone\n");
	}
}

