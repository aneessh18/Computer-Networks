#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
//create socket
// bind the socket to address
// listen 
//accept clients
//send and receive messages
int main()
{
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0); // socket

	struct sockaddr_in server,client;
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(8080);

	if(bind(sfd,(struct sockaddr *)&server,sizeof(server))<0)
	{
		puts("bind failed");
	}

	listen(sfd,3);
	int nsfd,c=sizeof(client);
	while(1) // reading a new client
	{
		nsfd=accept(sfd,(struct sockaddr *)&server,(socklen_t *)&c);
		if(nsfd<0)
		{
			puts("accept failed");
		}
		else
		{
			if(fork()==0) // child process for a new client
			{
				close(sfd); // no need of sfd cause of nsfd
				puts("created a child process for new client\n");
				char *msg="hello client";
				send(nsfd,msg,strlen(msg),0);
				while(1) // communicate with client receving and sending the message
				{
					char buff[100];
					int sz=recv(nsfd,buff,100,0);
					buff[sz]='\0';
					send(nsfd,buff,strlen(buff),0);
				}
				close(nsfd);
			}
			else //  parent process
			{
				close(nsfd); // it should not communicate with client
			}
		}

		
	}
	close(sfd);
	return 0;
}