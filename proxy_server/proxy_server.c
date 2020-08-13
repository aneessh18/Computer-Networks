#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>
#define no_of_servers 2
pthread_t server_read[no_of_servers],server_write[no_of_servers];


// connections are of the type many to many

int server_client[no_of_servers][100]; // servers holding their client nfds
int client_server[no_of_servers][100];
int client_server_size[no_of_servers];
int server_client_size[no_of_servers];
int server_sfd[no_of_servers]; // sfd's for connecting through their servers

void * read_service(void *argc)
{
    int *s=argc;
    int server_no=*s;
    printf("It's server no : %d \n",server_no);
    while(1)
    {
      int n=client_server_size[server_no];
      struct pollfd clients[n];
      for(int i=0;i<n;i++) // taking input from clients
      {
          clients[i].fd=client_server[server_no][i];
          clients[i].events=POLLIN;
      }
      int r=poll(clients,n,5000);
      if(r>0)
      {
          for(int i=0;i<n;i++)
          {
              if(clients[i].revents>0)
              {
                  char buff[100];
                  int sz=recv(clients[i].fd,buff,100,0);
                  buff[sz]='\0';
                  printf("message to server %s \n",buff);
                  send(server_client[server_no][i],buff,strlen(buff),0);
              }
          }
      }

  }
}
void *write_service(void *argc)
{
    int *s=argc;
    int server_no=*s;
    printf("It's server no : %d \n",server_no);
    while(1)
    {
        int n=server_client_size[server_no];
        struct pollfd clients[n];
        for(int i=0;i<n;i++) // writing output to clients
        {
          clients[i].fd=server_client[server_no][i];
          clients[i].events=POLLIN;
        }
        int r=poll(clients,n,5000);
        if(r>0)
        {
          for(int i=0;i<n;i++)
          {
              if(clients[i].revents>0)
              {
                  char msg[100];
                  int sz=recv(clients[i].fd,msg,100,0);
                  msg[sz]='\0';
                  printf("message from server %s\n",msg);
                  send(client_server[server_no][i],msg,100,0);
              }
          }
        }
    }
}


int main()
{
    for(int i=0;i<no_of_servers;i++)
    {
        server_sfd[i]=socket(AF_INET,SOCK_STREAM,0);
    }
    int sfd; // accepting from client's
    sfd=socket(AF_INET,SOCK_STREAM,0);
    int opt=1;
    if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt))<0)
    {
        puts("failed");
    }
    struct sockaddr_in proxy_server;
    proxy_server.sin_addr.s_addr=INADDR_ANY;
    proxy_server.sin_family=AF_INET;
    proxy_server.sin_port=htons(8080);
    
    if(bind(sfd,(struct sockaddr*)&proxy_server,sizeof(proxy_server))<0)
    {
        puts("bind failed");
    }
    listen(sfd,10);
    for(int i=0;i<no_of_servers;i++)
    {
        server_client_size[i]=0;
        client_server_size[i]=0;
    }

    int server_port=8081;
    struct sockaddr_in server[5];
    for(int i=0;i<no_of_servers;i++)
    {
        server[i].sin_addr.s_addr=INADDR_ANY;
        server[i].sin_family=AF_INET;
        server[i].sin_port=htons(server_port);
        server_port++;
    }
    for(int i=0;i<no_of_servers;i++)
    {
        if(connect(server_sfd[i],(struct sockaddr*)&server[i],sizeof(server[i]))<0)
        {
            puts("connect failed");
        }
        else
        {
            puts("connected");
        }
        
    }
    int server_id[no_of_servers];
    for(int i=0;i<no_of_servers;i++)
    server_id[i]=i;

    for(int i=0;i<no_of_servers;i++) // starting the service of special servers
    {
        pthread_create(&server_read[i],NULL,read_service,&server_id[i]);
        pthread_create(&server_write[i],NULL,write_service,&server_id[i]);
    }

    struct sockaddr_in client;
    while(1)
    {
        int l=sizeof(client);
        int nsfd=accept(sfd,(struct sockaddr*)&client,&l);
        if(nsfd<-1)
        {
            puts("error in accept");
        }
        char buff[100];
        int sz=recv(nsfd,buff,100,0);
        buff[sz]='\0';
        int server_no=atoi(buff);
        printf("got the server no %d \n",server_no);
        // stored client nsfd and incremented size

        // mapping client to server relationships
        client_server[server_no-1][client_server_size[server_no-1]]=nsfd;
        client_server_size[server_no-1]++; 

        server_client[server_no-1][server_client_size[server_no-1]]=socket(AF_INET,SOCK_STREAM,0);
        if(connect(server_client[server_no-1][server_client_size[server_no-1]],(struct sockaddr*)&server[server_no-1],sizeof(server[server_no-1]))<0)
        {
            puts("connection failed");
        }
        server_client_size[server_no-1]++;
    }
    for(int i=0;i<no_of_servers;i++)
    {
        pthread_join(server_read[i],NULL);
        pthread_join(server_write[i],NULL);
    }
}