#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdbool.h>
#define SNAME1 "/mysem1"
#define SNAME2 "/mysem2"
// mode of communication of pid's is through shared memory
struct memory{
    int num;
    int pid1,pid2;
    bool isp1ready;
    bool isp2ready;
};

struct memory* x; // for sharing x and y

void handler(int signum)
{
    if(signum==SIGUSR1)
    {
        printf("p2 has raised a signal\n");
    }
    
}

int main()
{
    int pid=getpid();// current process id 
    int shmid;
    int key=12345;
    //creating the shared memory
    shmid=shmget(key,sizeof(struct memory),IPC_CREAT | 0666);
   // sem_t *s1=sem_open(SNAME1,O_CREAT,0644,0); // creating the semaphores in 
    //sem_t *s2=sem_open(SNAME2,O_CREAT,0644,0); 
    //sem_init(s1,0,0);
    //sem_init(s2,0,0);
    x=(struct memory*)shmat(shmid,NULL,0);
    x->pid1=pid;
    x->num=0;
    x->isp1ready=true;
    signal(SIGUSR1,handler);// assigning the signal handler function
    int c=0;
    while(!x->isp2ready);
    x->isp1ready=false;
    while(c<3)
    {
        x->isp1ready=false;
        x->num++;
        printf("p1 has written value of x \n");
        printf("value of x is %d\n",x->num);
        x->isp1ready=true;
        kill(x->pid2,SIGUSR2);
        while(!x->isp2ready); // wait untill p2 is ready 
        printf("p1 read the value of y \n");
        printf("value of y is %d\n",x->num);
        c++;
    }
    shmdt((void*)x); 
    shmctl(shmid, IPC_RMID, NULL); 
}

