#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
// create socket
// connect to server
// send and receive messsages
int main ()
{
	int sfd=socket(AF_INET,SOCK_DGRAM,0);

	struct sockaddr_in server,process,client ; // bypass server address

	server.sin_family=AF_INET;
	server.sin_addr.s_addr=inet_addr("127.0.0.1");
	server.sin_port=htons(8080);
    char pid[100];
    printf("enter the process no in g2 \n");
    scanf("%s",pid);
    int sfd2=socket(AF_INET,SOCK_STREAM,0);
    int t=sizeof(process);
    listen(sfd,3);
    
    sendto(sfd,pid,strlen(pid),0,(struct sockaddr*)&server,sizeof(server));
    int x=getsockname(sfd,(struct sockaddr*) &process,&t);
    printf("%d\n",x);
    printf("port no  %d %d \n",process.sin_port,process.sin_addr.s_addr);
    if(bind(sfd2,(struct sockaddr*)&process,sizeof(process)))
    {
        puts("bind failed");
        return -1;
    }
    listen(sfd2,3);
    int nsfd,c=sizeof(client);
    if((nsfd=accept(sfd2,(struct sockaddr *)& client,(socklen_t *)&c))<0)
    {
        puts("accpet failed ");
        return -1;
    }
    printf("hi\n");
    char buff[100];
    while(1)
    {
        int sz=recv(nsfd,buff,100,0);
        buff[sz]='\0';
        printf("awesome %s\n",buff);
    }


	close(sfd);
	return 0;
}