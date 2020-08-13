#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
// main server routing clients to s1 and s2
#define port1 8080
#define port2 8081
int main()
{
    int sfd1,sfd2;
    sfd1=socket(AF_INET,SOCK_STREAM,0);
    sfd2=socket(AF_INET,SOCK_STREAM,0);
    if(sfd1<0 || sfd2<0)
    {
        printf("socket failed \n");
        return -1;
    }
    // bind the sockets

    struct sockaddr_in server1,server2,client;
    server1.sin_family=AF_INET;
    server1.sin_port=htons(port1);
    server1.sin_addr.s_addr=INADDR_ANY;

    server2.sin_family=AF_INET;
    server2.sin_port=htons(port2);
    server2.sin_addr.s_addr=INADDR_ANY;

    if(bind(sfd1,(struct sockaddr*)&server1,sizeof(server1))<0)
    {
        printf("bind failed\n");
        return -1;
    }

    if(bind(sfd2,(struct sockaddr*)&server2,sizeof(server2))<0)
    {
        printf("bind failed\n");
        return -1;
    }
    listen(sfd1,3);
    listen(sfd2,3);
    struct pollfd pfd[2];
    pfd[0].fd=sfd1; // linked to server1
    pfd[1].fd=sfd2; // linked to server2
    pfd[0].events=POLLIN;
    pfd[1].events=POLLIN;
    int no_of_clients=0;
    while(1)
    {
        int r=poll(pfd,2,5000);
        if(r>0)
        {
            if(pfd[0].revents>0) // client request for server1
            {
                int nsfd,c=sizeof(server1);
                if((nsfd=accept(pfd[0].fd,(struct sockaddr *)& server1,(socklen_t *)&c))<0)
                {
                    printf("not able to accpet\n");
                    return -1;
                }
                //create a process for server1
                no_of_clients++;
                if(fork()==0) // child process
                {
                    close(sfd1);
                    close(sfd2);
                    printf("we are connecting to s1 \n");
                    printf("client %d connected to s1\n",no_of_clients);
                    dup2(nsfd,0);// input and output streams are changed
                    dup2(nsfd,1);
                    char *argv[1];
                    argv[0]=NULL;
                    
                    execvp("./s1",argv); 
                }
                else
                {
                    close(nsfd);
                }
            }
            if(pfd[1].revents>0) // client request for server2
            {
                int nsfd,c=sizeof(server2);
                if((nsfd=accept(pfd[1].fd,(struct sockaddr *)&server2,(socklen_t *)&c))<0)
                {
                    printf("not able to accept\n");
                    return -1;
                }
                no_of_clients++;
                if(fork()==0)
                {
                    close(sfd2);
                    close(sfd1);
                    printf("client %d connected to s2\n",no_of_clients);
                    printf("we are connecting to s2\n");
                    dup2(nsfd,0);
                    dup2(nsfd,1);
                    char *argv[1];
                    argv[0]=NULL;
                    execvp("./s2",argv); // connecting to s2
                }
                else
                {
                    close(nsfd);
                }
                
            }
        }
    }

}