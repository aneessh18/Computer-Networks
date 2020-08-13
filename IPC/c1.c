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
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_family=AF_INET;
    server.sin_port=htons(8080);

    if(connect(sfd,(struct sockaddr*)&server,sizeof(server))<0)
    {
        puts("connect failed");
    }
    char msg[100];
    scanf("%s",msg);
    send(sfd,msg,strlen(msg),0);
}