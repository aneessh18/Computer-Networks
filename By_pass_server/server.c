#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include  <string.h>
#include <unistd.h>
#include <poll.h>
#define port 8080
int main()
{
    int sfd1; // for receving requests from group1
    sfd1=socket(AF_INET,SOCK_DGRAM,0);
    
    struct sockaddr_in server,client;

    server.sin_addr.s_addr=inet_addr("127.0.0.1");
    server.sin_port=htons(port);
    server.sin_family=AF_INET;

    if((bind(sfd1,(struct sockaddr *)&server,sizeof(server)))<0)
    {
        puts("bind failed");
        return -1;
    }

    while(1)
    {
        char buff[100];
        int c=sizeof(client);
        int sz=recvfrom(sfd1,buff,100,0,(struct sockaddr*)&client,&c);
        buff[sz]='\0';
        int portno;
        printf("enter the port no of process %s \n",buff);
        scanf("%d",&portno);
        struct sockaddr_in g2;
        g2.sin_addr.s_addr=inet_addr("127.0.0.1");
        g2.sin_family=AF_INET;
        g2.sin_port=htons(portno);

        //client.sin_port=htons(8082);
        sendto(sfd1,(struct sockaddr *)&client,sizeof(client),0,(struct sockaddr*)&g2,sizeof(g2));
        printf("send to g2 \n");
    }
    
}
