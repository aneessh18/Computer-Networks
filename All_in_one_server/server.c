#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <semaphore.h>
#include <poll.h>
#define SNAME1 "/mysem1"
#define SNAME2 "/mysem2"
struct memory
{
    int pid1,pid2;
    bool isp1ready;
    bool isp2ready;
};
struct client
{
    int pid;
    char msg[100];
    int state;
};
pid_t p[100];
int no_of_clients=0;
int queue_no=-1;
void handler() // start the echoserver
{
    if(queue_no<no_of_clients)
    {
        printf("server singal \n");
        int child=fork();
        queue_no++;
        if(child==0) // start the echoserver parallelly 
        {
            kill(p[queue_no],SIGUSR2);
            printf("your are in echo server  %d  \n",getpid());
            sem_t *s1=sem_open(SNAME1,O_CREAT,0644,0); // for synchronizing
            sem_t *s2=sem_open(SNAME2,O_CREAT,0644,0); 
            char path[100];
            sprintf(path,"e%d.pipe",p[queue_no]);
            mkfifo(path,0600);
            int fd=open(path,O_RDWR);
            struct client* pointer1;
            pointer1= malloc(sizeof (struct client));
            int c=0;
            while (c<3)
            {
                c++;
                char msg[100];
                sem_wait(s1);
                int sz=read(fd,pointer1,sizeof(struct client));
                char extra[1000];
                sprintf(extra,"message from CLIENT : %d ---> %s",pointer1->pid,pointer1->msg);
                write(1,extra,strlen(extra));
                fflush(stdout);
                write(fd,pointer1,sizeof(struct client)); // after writing to client release the semaphore
                sem_post(s2);
            }
        }
    }
}
int main()
{
    int pid=getpid();

    printf("%d\n",pid);
    int shmid; // shared memory block for receiving the singal
    int key=12345;
    shmid=shmget(key,sizeof(struct memory),IPC_CREAT | 0666);
    struct memory *x;
    x=(struct memory*)shmat(shmid,NULL,0);
    x->pid2=pid;
    x->isp2ready=true; // server is ready
    signal(SIGUSR1,handler);

    char *p2fifo="p2fifo"; // opening the fifo for communicating with p2
    int ffd=open(p2fifo,O_RDWR);
    FILE *f1=popen("./p1","r"); // opening the processes p1 and p3
    FILE *f2=popen("./p3","r");
     // reading input from clients
    mkfifo("famousfifo",0600);
    int sfd=open("famousfifo",O_RDWR);

    FILE *f3=popen("./p5","w");

    struct pollfd pfds[5];
    pfds[0].fd=0; // stdin
    pfds[1].fd=fileno(f1); //from p1
    pfds[2].fd=ffd; //from p2
    pfds[3].fd=fileno(f2);//from p3
    pfds[4].fd=sfd;

    for(int i=0;i<5;i++)
    pfds[i].events=POLLIN;

    dup2(fileno(f3),1);  // setting stdout to p5
    struct client* pointer=malloc(sizeof(struct client));
    while(1)
    {
        int r=poll(pfds,5,10000); // polling the inputs
        if(r>0)
        {
            for(int i=0;i<5;i++)
            {
                if(i!=4 && pfds[i].revents >0) // we received msg from some process 
                {
                    char msg[100];
                    int sz=read(pfds[i].fd,msg,100);

                    write(1,msg,sz); // writing msg in to p5
                }
                else if(i==4 && pfds[i].revents >0) // we recevied msg from client
                {
                    int sz=read(pfds[i].fd,pointer,sizeof(struct client));
                    printf("namste : %d state %d \n",pointer->pid,pointer->state);
                    if(pointer->state==0) //register the client 
                    {   
                        printf("connection established with CLIENT :  %d \n",pointer->pid);
                        p[no_of_clients++]=pointer->pid;
                        printf("no of clients : %d \n",no_of_clients);
                    }
                    else // we received a msg from client 
                    {
                        for(int i=0;i<no_of_clients;i++)
                        {
                            if(pointer->pid!=p[i])
                            {
                                char path[100];
                                sprintf(path,"%d.pipe",p[i]); // writing message to client
                                int wfd=open(path,O_WRONLY);
                                write(wfd,pointer->msg,strlen(pointer->msg));
                                close(wfd);
                            }
                        }
                    }
                    
                }
            }
        }
    }
}