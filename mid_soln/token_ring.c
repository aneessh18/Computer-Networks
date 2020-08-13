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
struct frame
{
	int rpid,spid;
	char msg[100];
};
int next_pid;
struct frame *pointer;
void signal_handler()
{
	pointer=malloc(sizeof(struct frame));
	printf("its my turn\n");
	int fd=open("token",O_RDWR);
	int sz=read(fd,pointer,sizeof(struct frame));
	printf("%d\n",pointer->rpid);
	if(sz>0)
	{
		if(pointer->rpid==getpid())
		{
			printf("this message is for me ");
			puts(pointer->msg);
		}
		else if(pointer->rpid!=0) // write the msg again in to the fifo
		{
			printf("this message is not for me ");
			write(fd,pointer,sizeof(struct frame));
		}
		if(pointer->spid==getpid())
		{
			printf("i am clearing the frame \n");
			pointer->rpid=0;
			pointer->spid=0;
		}
		char opt;
		printf("do you want to send msg \n");
		scanf(" %c",&opt);
		if(opt=='y') // we are seeing the frame
		{
			printf("enter the msg \n");
			scanf("%s",pointer->msg);
			int rpid;
			printf("enter the dest \n");
			scanf("%d",&rpid);
			pointer->rpid=rpid;
			pointer->spid=getpid();
			printf("hi\n");
		}
		else
		{
			printf("passing the frame \n");
		}
	}
	else
	{
		printf("not able to read the frame\n");
	}
	write(fd,pointer,sizeof(struct frame));
	read(fd,pointer,sizeof(struct frame));
	printf("%d\n",pointer->spid);
	write(fd,pointer,sizeof(struct frame));
	close(fd);
	printf("sent msg to next process\n");
	kill(next_pid,SIGUSR1);
}
int main()
{
	signal(SIGUSR1,signal_handler); // setting the func 
	printf("my ip address is %d \n",getpid());
	printf("give my neighbour's pid\n");
	scanf("%d",&next_pid);

	while(1)
	{
	
	}
}
	
	
	
