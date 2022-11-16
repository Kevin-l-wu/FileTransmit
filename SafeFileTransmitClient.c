#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Common.h"
#include "Protocol.h"
#include "FileOperation.h"

const char* client_work_dir = "/Users/kevin/Client/";


static void fill_addr_struct(struct sockaddr_in* server_addr, char* server_ip)
{
	bzero(server_addr, sizeof(struct sockaddr_in));
	
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(PORT);
	server_addr->sin_addr.s_addr = inet_addr(server_ip);
}

static void print_client_menu()
{
	printf("******************* Safe File Transmit Client *******************\n\n");
	
	printf("\t\t\t1. Upload file\n");
	printf("\t\t\t2. Download file\n");
	printf("\t\t\t3. Exit\n\n");
	
	printf("*****************************************************************\n");
}

static int upload_file(int sockfd)
{
	Frame client_frame; 
	char file_name[FILE_NAME_MAX_LENGTH] = {'\0'};
	char pathname[FILE_NAME_MAX_LENGTH] = {'\0'};
	int file_length = 0;
	int recv_bytes = 0;
	char* file_data_ptr = NULL;
	int i = 0;
	int data_packet_number = 0;
	
	int index = 0;

	
	printf("Please input file name\n");
	
	fgets(file_name, FILE_NAME_MAX_LENGTH, stdin);
	
	file_name[strlen(file_name) - 1] = '\0';
/*		
	for(i = 0; i < 9; i++)
	{
		printf("%x\n", file_name[i]);
		printf("%c\n", file_name[i]);
	}
	
	printf("strlen(file_name) = %x\n", strlen(file_name));
*/	
	strncpy(pathname, client_work_dir, strlen(client_work_dir) + 1);
	strcat(pathname, file_name);
	
	printf("Pathname = %s\n", pathname);
	
	/* 1. Send upload file request to server and wait response ok */
	
	fill_command_frame(&client_frame, REQUEST, UPLOAD_FILE);
	
	send(sockfd, (char*)&client_frame, sizeof(Frame), 0);
	
	memset(&client_frame, 0, sizeof(Frame));
	
	recv_bytes = recv(sockfd, &client_frame, sizeof(Frame), 0);
	
	if(client_frame.type != RESPONSE_OK)
	{
		perror("Client: Response error\n");
		goto ERROR;
	}
	
	/* 2. Send file information packet and wait response ok */
	
	file_length = get_file_length(pathname);
	
	fill_file_info_frame(&client_frame, REQUEST, file_name, strlen(file_name), file_length);
	
	send(sockfd, (char*)&client_frame, sizeof(Frame), 0);
	
	memset(&client_frame, 0, sizeof(Frame));
	
	recv_bytes = recv(sockfd, &client_frame, sizeof(Frame), 0);
	
	if(client_frame.type != RESPONSE_OK)
	{
		perror("Client: Response error\n");
		goto ERROR;
	}

	/* 3. Send file data packet and wait response ok */
	
	file_data_ptr = malloc(file_length + 1);
	if(file_data_ptr == NULL)
	{
		perror("malloc error\n");
		goto ERROR;
	}
	
	read_file_to_buffer(pathname, file_data_ptr, file_length);

	/* Packet data to multi-packet */
	data_packet_number = ((file_length % DATA_LENGTH) == 0) ?
		(file_length / DATA_LENGTH) : ((file_length / DATA_LENGTH) + 1);
	
	for(i = 0; i < data_packet_number; i++)
	{
		fill_data_frame(&client_frame, DATA, file_data_ptr + i * DATA_LENGTH, DATA_LENGTH);
		
		printf("Dump data frame: %d\n", i);
		
		MemDump(file_data_ptr + i * DATA_LENGTH, DATA_LENGTH);	


		send(sockfd, (char*)&client_frame, sizeof(Frame), 0);
		
		memset(&client_frame, 0, sizeof(Frame));
		
		recv_bytes = recv(sockfd, &client_frame, sizeof(Frame), 0);
		
		if(client_frame.type != RESPONSE_OK)
		{
			perror("Client: Response error\n");
			goto ERROR;
		}
		
		printf("Client: received server response: %d\n", i);
	}
	
	printf("Client: Upload success\n");
	
	/* 4. Exit */
	
	if(file_data_ptr != NULL)
	{
		free(file_data_ptr);
		file_data_ptr = NULL;
	}
	
	return 0;
	
ERROR:
	if(file_data_ptr != NULL)
	{
		free(file_data_ptr);
		file_data_ptr = NULL;
	}
	
	return -1;
}

static void download_file(int sockfd)
{
	Frame client_frame; 
	
	/* 1. Send download file request to server and wait response ok */
	fill_command_frame(&client_frame, REQUEST, DOWNLOAD_FILE);
	
	/* 2. Recv file information packet and response ok */
	
	/* 3. Create file as file information */
	
	/* 4. Recv file data packet and response ok */
	
	/* 5. Exit */
}

static int input_command_handle(int sockfd)
{
	char command = 0;
	int temp = 0;
	
	printf("Please input the command:");
	
	command = getchar();
	
	temp = getchar();
	
	switch(command)
	{
		case '1':
			printf("Upload file\n");
			upload_file(sockfd);
			break;
		case '2':
			printf("Download file\n");
			break;
		case '3':
			printf("Exit\n");
			break;
		default:
			printf("Invalid input!\n");
			input_command_handle(sockfd);
	}
	
	return 0;
}

int main(int argc, char* argv[])
{
	int sockfd = -1;
	char server_ip[IP_ADDR_STR_LENGTH + 1] = {'\0'};
	struct sockaddr_in server_addr = {0};
	char send_buf[127] = {0};
	
	if(argc != 2)
	{
		printf("Usage: ./SafeFileTransmitClient ServerIp\n");
		exit(-1);
	}
	
	if(!ip_addr_valid_check(argv[1], strlen(argv[1]) + 1))
	{
		printf("Invalid ServerIp\n");
		exit(-1);
	}
	
	strncpy(server_ip, argv[1], strlen(argv[1]) + 1);
	
	printf("Server ip: %s\n", server_ip);
	
	/* 1. Create socket */
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) //IPv4, TCP
	{
		printf("Create socket error\n");
		exit(-1);
	}
	
	fill_addr_struct(&server_addr, server_ip);
	
	/* 2. Connect */
	if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		printf("Connect fail");
		exit(-1);
	}

	print_client_menu();
	
	input_command_handle(sockfd);
	
	close(sockfd);
	
	return 0;
}
