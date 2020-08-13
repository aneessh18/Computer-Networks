
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 


int main() { 
	int sfd; 
	char buffer[100]; 
	char *hello = "Hello from udp client"; 
	struct sockaddr_in	 servaddr; 

	// Creating socket file descriptor 
	if ( (sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(8080); 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	
	int n, len; 
	
	sendto(sfd,hello,strlen(hello),0,(struct sockaddr *) &servaddr,sizeof(servaddr)); 
	
	close(sfd); 
	return 0; 
} 
