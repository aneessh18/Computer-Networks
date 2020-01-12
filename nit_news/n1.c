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
#define SNAME1 "/mysem1"

struct news{ // writing to new's readers table through fifo
    long type;
    char msg[100];
    int pid;
}pointer;

struct memory{
    bool is_live_telecast;
};
struct memory* x;

struct client
{
    int pid;
}cr;

int main()
{
    key_t key1;
    int msgid1;

    int shmid;
    int key=12345;
    //creating the shared memory for synchronizing telecasts
    shmid=shmget(key,sizeof(struct memory),IPC_CREAT | 0666);
    x=(struct memory*)shmat(shmid,NULL,0);
    x->is_live_telecast=false;

    sem_t *s1=sem_open(SNAME1,O_CREAT,0644,0);

    key1=ftok("readerqueue",65); // for receiving messages
    msgid1=msgget(key1,0666 | IPC_CREAT);

    int fd1=open("editor",O_RDWR);

    mkfifo("screen",0600);

    int fd2=open("screen",O_RDWR);

    int pid=getpid(); // registration block
    cr.pid=pid;
    write(fd1,&cr,sizeof(cr));

     while(1) // get the present message
    {
         //printf("waiting\n");
        if(msgrcv(msgid1,&pointer,sizeof(pointer),0,0)<=0)
        {
            continue;
        }
        printf("news reader %d  message  \n",pid);
       if(strlen(pointer.msg)>=2 && pointer.msg[0]=='.' ) // we are about live telecast
        {
            kill(pointer.pid,SIGUSR1);// signalling the editor
            x->is_live_telecast=true;
            char id[6];
            int len=0;
            for(int i=1;i<strlen(pointer.msg);i++)
            {
                id[len++]=pointer.msg[i];
            }
            int lpid=atoi(id);
            char path[100],msg[100];
            sprintf(path,"l%d.pipe",lpid);
            int fd3=open(path,O_RDWR);
            printf("we are about to live telecast \n");
            int c=0;
            while(c<3)
            {
                int sz=read(fd3,msg,100);
                if(sz<=0)
                {
                   // sleep(3);
                    continue;
                }
                c++;
                write(fd2,msg,sz);
            }
            printf("live telecast has been completed\n");
            x->is_live_telecast=false;
        }
       else
       {
            write(fd2,pointer.msg,strlen(pointer.msg));   
       }
       sem_post(s1);
    }
}