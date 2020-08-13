#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>

int main()
{
    int sfd; // accepting from client's
    sfd=socket(AF_INET,SOCK_STREAM,0);
    int opt=1;
    if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,(char *)&opt,sizeof(opt))<0)
    {
        puts("failed");
    }
    struct sockaddr_in server,client;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_family=AF_INET;
    server.sin_port=htons(8080);

    if(bind(sfd,(struct sockaddr*)&server,sizeof(server))<0)
    {
        puts("bind failed");
    }
    if(listen(sfd,50)==-1)
    {
        perror("listen");
    }
    int l=sizeof(client);
    int nsfd=accept(sfd,(struct sockaddr*)&client,&l);
    if(nsfd<-1)
    {
        puts("accpet failed");
    }
    char buff[100];
    int sz=recv(nsfd,buff,100,0);
    if(sz==-1)
    {
        perror("recv");
    }
    puts(buff);
}
