#include <arpa/inet.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
int main() 
{ 
	int sfd; 
	char* message = "Hello this is TCP client"; 
	struct sockaddr_in servaddr; 

	int n, len; 
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
		printf("socket creation failed"); 
		exit(0); 
	} 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(8080); 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

	if (connect(sfd, (struct sockaddr*)&servaddr, 
							sizeof(servaddr)) < 0) { 
		printf(" connect failed \n"); 
	} 

	send(sfd, message,strlen(message),0); 
	close(sfd); 
} 
