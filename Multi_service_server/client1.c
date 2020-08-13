#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// create socket
// connect to server
// send and receive messsages
int main ()
{
	int sfd=socket(AF_INET,SOCK_STREAM,0);

	struct sockaddr_in server ; // server address

	server.sin_family=AF_INET;
	server.sin_addr.s_addr=inet_addr("127.0.0.1");
	server.sin_port=htons(8080);
	if(connect(sfd,(struct sockaddr *)&server,sizeof(server))<0)
	{
		puts("connection failed");
	}
	char buff[100],msg[100];
	int sz=recv(sfd,msg,100,0);// first connection
	msg[sz]='\0';
	puts(msg);
	while(1)
	{
		read(0,buff,100);
		send(sfd,buff,strlen(buff),0);
		sz=recv(sfd,buff,100,0);
		buff[sz]='\0';
		puts(buff);
	}
	close(sfd);
	return 0;
}