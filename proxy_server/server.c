#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>
#include <pthread.h>
int clients_fd[100];
int client_no=0;
void *serve()
{
    while(1)
    {
        struct pollfd clients[client_no];
        for(int i=0;i<client_no;i++)
        {
            clients[i].fd=clients_fd[i];
            clients[i].events=POLLIN;
        }
        int r=poll(clients,client_no,5000);
        if(r>0)
        {
            for(int i=0;i<client_no;i++)
            {
                if(clients[i].revents>0)// we got to server this client man !!
                {
                    char buff[100];
                    int sz=recv(clients[i].fd,buff,100,0); // recv and send the msg
                    buff[sz]='\0';
                    strcat(buff,"0");
                    puts(buff);
                    send(clients[i].fd,buff,strlen(buff),0);
                }
            }
        }
    }
}
int main()
{
    pthread_t thread;
    struct sockaddr_in server;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_family=AF_INET;
    int port;
    puts("enter port no");
    scanf("%d",&port);
    server.sin_port=htons(port);

    int sfd=socket(AF_INET,SOCK_STREAM,0);
    int opt=1;
    if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt))<0)
    {
        puts("failed");
    }
    if(bind(sfd,(struct sockaddr*)&server,sizeof(server))<0)
    {
        puts("bind failed");
    }
    listen(sfd,10);
    struct sockaddr_in proxy_server;
    int l=sizeof(proxy_server);
    pthread_create(&thread,NULL,serve,NULL);
    while(1)
    {
        int nsfd=accept(sfd,(struct sockaddr*)&proxy_server,&l);
        clients_fd[client_no]=nsfd;
        client_no++;
        printf("i got to server another client !!!!\n");
    }

    if(l<0)
    {
        puts("accpet failed");
    }
    pthread_join(thread,NULL);
}