#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>
#include <pthread.h>
pthread_t thread;
int sfd;

void *service()
{
    printf("client is ready to read\n");
    while(1)
    {
        char msg[100];
        int sz=recv(sfd,msg,100,0);
        if(sz<=0)
        continue;
        msg[sz]='\0';
        puts(msg);
    }
}

int main()
{
    sfd=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in proxy_server;
    proxy_server.sin_addr.s_addr=INADDR_ANY;
    proxy_server.sin_family=AF_INET;
    proxy_server.sin_port=htons(8080);

    if(connect(sfd,(struct sockaddr*)&proxy_server,sizeof(proxy_server))<0)
    {
        puts("connect failed");
    }
    pthread_create(&thread,NULL,service,NULL);
    while(1)
    {
        char server_no[10];
        puts("enter server no");
        scanf("%s",server_no);
        send(sfd,server_no,strlen(server_no),0);
        char buff[100];
        int sz=read(0,buff,100);
        buff[sz]='\0';
        send(sfd,buff,strlen(buff),0);
    }
    pthread_join(thread,NULL);
    
}