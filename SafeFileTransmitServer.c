#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "Protocol.h"
#include "FileOperation.h"
#include "Common.h"

#define PORT 3333

char* server_dir = "/Users/kevin/Server/";

void fill_addr_struct(struct sockaddr_in* server_addr)
{
	bzero(server_addr, sizeof(struct sockaddr_in));
	
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(PORT);
	server_addr->sin_addr.s_addr = htonl(INADDR_ANY);
}

int process_upload_file(int new_sockfd)
{
	Frame server_frame;
	ssize_t recv_bytes = 0;
	char pathname[FILE_NAME_MAX_LENGTH] = {'\0'};
	int fd = -1;
	int error_code = 0;
	unsigned file_length = 0;
	int data_packet_number = 0;
	char* file_data_ptr = NULL;
	char* file_data_temp_ptr = NULL;
	int i = 0;
	
	/***************************** Response ok to client ********************************/
	memset(&server_frame, 0, sizeof(server_frame));
	
	fill_response_frame(&server_frame, RESPONSE_OK);
	
	send(new_sockfd, (char*)&server_frame, sizeof(server_frame), 0);
	
	/***********************************************************************************/
	
	/****************************** Receive Frame From Client **************************/
	memset(&server_frame, 0, sizeof(server_frame));
	
	/* 2.1 Receive file info from client */
	recv_bytes = recv(new_sockfd, &server_frame, sizeof(server_frame), 0);
	
	/**********************************************************************************/
	
	/*.2.2 Create file as received name */
	strncpy(pathname, server_dir, strlen(server_dir) + 1);
	
	strcat(pathname, server_frame.file_name);
	
	printf("pathname = %s\n", pathname);
	
	file_length = server_frame.file_length;
	
	if((fd = open(pathname, O_RDWR|O_CREAT)) == -1)
	{
		printf("Open error\n");
		goto ERROR;
	}
	
	file_data_ptr = malloc(file_length + 1);
	if(file_data_ptr == NULL)
	{
		perror("malloc error\n");
		goto ERROR;
	}
	
	data_packet_number = (file_length % DATA_LENGTH) == 0 ?
		(file_length / DATA_LENGTH) : (file_length / DATA_LENGTH) + 1;
		
	
	/***************************** Response ok to client *******************************/
	memset(&server_frame, 0, sizeof(server_frame));
	
	fill_response_frame(&server_frame, RESPONSE_OK);
	
	send(new_sockfd, (char*)&server_frame, sizeof(server_frame), 0);
	
	/**********************************************************************************/
	
	printf("Server: data_packet_number = %d\n", data_packet_number);
	
	for(i = 0; i < data_packet_number; i++)
	{
		/****************************** Receive Frame From Client **************************/
		memset(&server_frame, 0, sizeof(server_frame));
		
		/* 2.1 Receive file info from client */
		recv_bytes = recv(new_sockfd, &server_frame, sizeof(server_frame), 0);
		
		printf("Server: received packet: %d\n", i);
		
		MemDump(server_frame.data, DATA_LENGTH);

		/**********************************************************************************/
		
		strncpy(file_data_ptr + i * DATA_LENGTH, server_frame.data, DATA_LENGTH);
		
		/***************************** Response ok to client *******************************/
		memset(&server_frame, 0, sizeof(server_frame));
		
		fill_response_frame(&server_frame, RESPONSE_OK);
		
		send(new_sockfd, (char*)&server_frame, sizeof(server_frame), 0);
		
		/**********************************************************************************/
	}
	
	printf("Server: Upload success\n");
	
	write(fd, file_data_ptr, file_length);
	
	close(fd);
	
	return 0;
	
ERROR:
	if(fd > 0)
	{
		close(fd);
	}

	return -1;
}

int process_download_file(int new_sockfd)
{
	
	return 0;	
}

int handle_request(int new_sockfd)
{
	Frame server_frame;
	ssize_t recv_bytes = 0;
	Command command;
	
	memset(&server_frame, 0, sizeof(server_frame));
	
	/* Receive request from client */
	recv_bytes = recv(new_sockfd, &server_frame, sizeof(server_frame), 0);
	
	command = server_frame.command;
	
	switch(command)
	{
		case UPLOAD_FILE:
			process_upload_file(new_sockfd);
			break;
		case DOWNLOAD_FILE:
			process_download_file(new_sockfd);
			break;
		default:
			break;
	}
	
	return 0;	
}

int main(int argc, char* argv[])
{
	int sockfd = -1;
	int new_sockfd = -1;
	struct sockaddr_in server_addr = {0};
	struct sockaddr_in client_addr = {0};
	socklen_t client_addr_len = sizeof(struct sockaddr);
	
	int max_connect_number = 0;
	int server_run_times = 0;
	
	pid_t child_id = 0;
	
	if(argc != 3)
	{
		printf("Usage: RemoteFileShare MaxConnectNumber ServerRunTimes\n");
		exit(0);
	}
	else
	{
		max_connect_number = atoi(argv[1]);
		server_run_times = atoi(argv[2]);
	}
	
	/* 1. Create socket */
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) //IPv4, TCP
	{
		printf("Create socket error\n");
		exit(-1);
	}

	fill_addr_struct(&server_addr);
	
	/* 2. Bind address */
	if((bind(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr))) == -1)
	{
		printf("Bind address error\n");
		exit(-1);
	}
	
	/* 3. Listen port, assert server can accept connect */
	if(listen(sockfd, max_connect_number) == -1)
	{
		printf("Listen error\n");
		exit(-1);
	}
	
	while(server_run_times)
	{	
		/* 4. Wait connect */
		if((new_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_len)) == -1)
		{
			printf("Accept error\n");
			exit(-1);
		}
		
		child_id = fork();
		
		if(child_id == 0)
		{
			printf("Cilent %s connected\n", inet_ntoa(client_addr.sin_addr));
	
			handle_request(new_sockfd);
			
			/* 6. Disconnect */
			close(new_sockfd);
			
			exit(0);
		}
		else if(child_id > 0)
		{
			printf("Main process\n");
			sleep(1);
		}
		else
		{
			printf("Fork error\n");
		}
		
		server_run_times--;
	}
	
	close(sockfd);
	
	return 0;
}
