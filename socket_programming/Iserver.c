#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
    int sfd;
    sfd=socket(AF_INET,SOCK_STREAM,0); // created a socket

    struct sockaddr_in server,client;
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(8888);


    if(bind(sfd,(struct sockaddr *)&server ,sizeof(server))<0)
    {
        puts("bind failed");
    }
    puts("bind done\n");

    listen(sfd,3);
    puts("waiting for incoming connections \n");
    int nsfd,c=sizeof(client);

    if((nsfd=accept(sfd,(struct sockaddr *)&client,(socklen_t *)&c))<0)
    {
        puts("accept failed");
    }
    puts("connections accepted \n");

    char *msg="hello client";
    send(nsfd,msg,strlen(msg),0);
    puts("sent the msg to client");
}