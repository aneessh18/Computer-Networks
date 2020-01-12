#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <poll.h>
#define no_of_groups 2
// this is server
struct client
{
    pid_t pid;
    int gid;
    char msg[100];
    bool firsttime;
};
int main()
{
    int rfds[no_of_groups],wfd;
    pid_t groups[no_of_groups][100];
    int group_start[no_of_groups];
    for(int i=0;i<no_of_groups;i++) // intializing all the group values 
    group_start[i]=0;

    char group_pipes[no_of_groups][100]; // all the group named pipes
    for(int i=0;i<no_of_groups;i++)
    {
        sprintf(group_pipes[i],"%d.pipe",i); 
        mkfifo(group_pipes[i],0600); // creating the named pipes
    }

    for(int i=0;i<no_of_groups;i++) // opening the file descriptors for all the group pipes 
    {
        rfds[i]=open(group_pipes[i],O_RDWR);
    }

    struct pollfd pfds[no_of_groups];  // because we are reading the input from group pipes
    for(int i=0;i<no_of_groups;i++) // intializing the poll descriptors
    {
        pfds[i].fd=rfds[i];
        pfds[i].events=POLLIN;
    }

    int no_of_clients=0,errno;
    struct client *pointer=malloc(sizeof(struct client));
    while(1) // server main has started
    {
        int r=poll(pfds,no_of_groups,10000); // polling has started

        if(r>0)
        {
            for(int i=0;i<no_of_groups;i++)
            {

                if(pfds[i].revents !=0 ) // some group pipe has got the message which has to be forwaded
                {
                    int sz=read(pfds[i].fd,pointer,sizeof (struct client)); // reading from the group pipe
                    printf("namaste %d \n",pointer->pid);
                    int group_no;
                    if(pointer->firsttime) // this client will be registered here first time 
                    {
                        printf("client id : %d \n",pointer->pid);
                        group_no=pointer->gid;
                        groups[group_no][group_start[group_no]]=pointer->pid; // adding the client
                        group_start[group_no]++;
                        no_of_clients++;
                        printf("no of clients are %d \n",no_of_clients);
                    }  
                    else // client is here for writing the msg
                    {
                        group_no=pointer->gid; // get the group number
                        for(int i=0;i<group_start[group_no];i++)
                        {
                            if(groups[group_no][i]!=pointer->pid)
                            {
                                char path[100];
                                sprintf(path,"%d.pipe",groups[group_no][i]);// writing to his client pipe
                                wfd=open(path,O_WRONLY);
                                write(wfd,pointer->msg,strlen(pointer->msg));
                                close(wfd);
                            }
                        }

                    }
                }
            }
        }
        else
        {
            printf("server is waiting for a client to write \n");
        }
        
    }
    for(int i=0;i<no_of_groups;i++)
    close(rfds[i]);
}