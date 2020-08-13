#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
//create socket
// bind the socket to address
// listen 
//accept clients
//send and receive messages
int main()
{
	int sfd;
	sfd=socket(AF_INET,SOCK_DGRAM,0); // socket

	struct sockaddr_in server,client,bypass;

    bypass.sin_addr.s_addr=inet_addr("127.0.0.1");
    bypass.sin_family=AF_INET;
    bypass.sin_port=htons(8080);

    server.sin_addr.s_addr=inet_addr("127.0.0.1");
    server.sin_family=AF_INET;
    server.sin_port=htons(8081);

    if(bind(sfd,(struct sockaddr *)&server,sizeof(server))<0)
    {
        puts("bind failed ");
        return -1;
    }

    int c=sizeof(bypass);
    recvfrom(sfd,(struct sockaddr*)&client,sizeof(client),0,(struct sockaddr*)&bypass,&c);

    printf("client %d\n",client.sin_port);
    int sfd2=socket(AF_INET,SOCK_STREAM,0);
    int x=sizeof(client);
    if(connect(sfd2,(struct sockaddr *)&client,x)<0)
	{
		puts("connection failed");
        return -1;
	}
    printf("hi\n");
    while(1)
    {
        char in[100];
        int sz=read(0,in,100);
        in[sz]='\0';
        send(sfd2,in,strlen(in),0);
    }
	close(sfd);
	return 0;
}