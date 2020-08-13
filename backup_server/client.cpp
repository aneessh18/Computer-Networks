#include <bits/stdc++.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
using namespace std;
int main()
{
	int sfd;
	struct sockaddr_in serv_addr;
	int port_no=8080;

	bzero(&serv_addr,sizeof(serv_addr));

	if((sfd = socket(AF_INET , SOCK_STREAM , 0))==-1)
	perror("\n socket");
	else printf("\n socket created successfully\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if(connect(sfd , (struct sockaddr *)&serv_addr , sizeof(serv_addr))==-1)
	perror("\n connect : ");
	else printf("\nconnect succesful");
	
		
	struct pollfd pfd[2];
	pfd[0].fd = sfd; // if it reads anything then display it
	pfd[1].fd = 0; // writing
	pfd[0].events = POLLIN;
	pfd[1].events = POLLIN;
	
	while(1)
	{
		int r = poll(pfd, 2, 5000);
		if(r > 0)
		{
			if(pfd[0].revents > 0)
			{
				// we received from server
				char buf[100];
				recv(pfd[0].fd, buf, 100, 0);
				cout<<"received message : "<<buf<<"\n";	
			}
			if(pfd[1].revents > 0)
			{
				// we are about to write a msg
				char buf[100];
				int sz = read(0, buf, 100);
				buf[sz] = '\0';
				cout<<buf<<" is the sending msg \n";
				send(sfd , buf, strlen(buf), 0); 
			}
		}
			
	}
	
}
