#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
int main()
{
    int sfd; // file descriptor for a socket
    struct sockaddr_in server;
    sfd=socket(AF_INET,SOCK_STREAM,0); // connection oriented 

     //intialization block
    server.sin_addr.s_addr=inet_addr("216.58.197.46"); //connection to google's ip address
    server.sin_family=AF_INET;
    server.sin_port=htons(80);// port address 80

    // connection block
    if(connect(sfd,(struct sockaddr *)& server,sizeof(server))<0)
    {
        puts("error while connecting");
        return 0;
    }
    puts("connected to google");

    // message sending block
    char *msg="GET / HTTP/1.1\r\n\r\n";
    if(send(sfd,msg,strlen(msg),0)<0)
    {
        puts("failed to send a message");
    }
    puts("message has been sent");

    // message receiving block
    char reply[2000];
    if(recv(sfd,reply,2000,0)<0)
    {
        puts("error while reading msg");
    }
    puts("message received");
    puts(reply);
    return 0;
}