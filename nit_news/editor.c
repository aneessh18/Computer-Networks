#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/types.h>
#include <pthread.h>

pthread_t registercr;
struct mesg_buff{    // this is the message buffer for  receive msg's from reporters
    long mesg_type;
    char mesg_text[100];
    //pid_t pid;
}msg;

struct news{ // writing to new's readers table through fifo
    long type;
    char msg[100];
    int pid;
};

struct client
{
    int pid;
}cr;

struct memory{
    bool is_live_telecast;
};
struct memory* x;

int no_of_live_telecasts=0;

void handler()
{
    printf("the live telecasting has started \n");
    no_of_live_telecasts++;
    printf("the no of live telecasts up to now has been %d \n",no_of_live_telecasts);
     // increment the count 
}
int no_of_clients=0; // for counting no of clients
int p[100];
void *registration(void *args) // parallely registering clients
{

    int fd1=open("editor",O_RDWR);
    while(1)
    {
        int sz=read(fd1,&cr,sizeof(cr));
        if(sz<=0)
        continue;
        printf("client connection established \n");
        printf("CLIENT %d ------>\n",cr.pid);
        p[no_of_clients++]=cr.pid;
    }
}
int main()
{
    key_t key1,key2;
    int msgid1,msgid2;

    int shmid;
    int key=12345;
    //creating the shared memory for synchronizing telecasts
    shmid=shmget(key,sizeof(struct memory),IPC_CREAT | 0666);
    x=(struct memory*)shmat(shmid,NULL,0);
    x->is_live_telecast=false;

    key2=ftok("readerqueue",65); // for sending message to clients
    msgid2=msgget(key2,0666 | IPC_CREAT);

    key1=ftok("reporterqueue",65); // creating a message queue
    msgid1=msgget(key1,0666 | IPC_CREAT); 

    mkfifo("editor",0600);  // creating a fifo that editor register client
    int fd1=open("editor",O_RDWR);
    pthread_create(&registercr,NULL,(void*)registration,NULL);

    int fd2=open("text",O_WRONLY); // document
    struct news pointer1;
   
    int pid=getpid();
    int next=0;

    signal(SIGUSR1,handler);

    while(1) // reading messages from message queue
    {
        if(msgrcv(msgid1,&msg,sizeof(msg),1,0)==0 || x->is_live_telecast==true)continue;
        if(strcmp(msg.mesg_text,"end")==0)
        break;
        if(strlen(msg.mesg_text)>=2 && msg.mesg_text[0]=='/' && msg.mesg_text[1]=='d') // write to the document
        {
            write(fd2,msg.mesg_text,strlen(msg.mesg_text));
        }
        else
        {
            pointer1.pid=pid;
            strcpy(pointer1.msg,msg.mesg_text); // copying the message
           // pointer1.type=p[next];
           pointer1.type=1;
            //next=(next+1)%no_of_clients;
            msgsnd(msgid2,&pointer1,sizeof(pointer1),0);// putting the message in the reader's queue
        }
        
    }
    pthread_join(registercr,NULL);
    msgctl(msgid1,IPC_RMID,NULL);
    msgctl(msgid2,IPC_RMID,NULL);
}