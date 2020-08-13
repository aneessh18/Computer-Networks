#include <bits/stdc++.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/un.h>
#include <arpa/inet.h>
using namespace std;
#define ADDRESS  "/home/indavarapu/Desktop/mid_exam/aneesh"

 struct mesg_buff{    
    long mesg_type;
    char mesg_text[100];
    //pid_t pid;
}msg;
int recv_fd(int socket)
 {
  int sent_fd, available_ancillary_element_buffer_space;
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  char message_buffer[1];
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];

  /* start clean */
  memset(&socket_message, 0, sizeof(struct msghdr));
  memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));

  /* setup a place to fill in message contents */
  io_vector[0].iov_base = message_buffer;
  io_vector[0].iov_len = 1;
  socket_message.msg_iov = io_vector;
  socket_message.msg_iovlen = 1;

  /* provide space for the ancillary data */
  socket_message.msg_control = ancillary_element_buffer;
  socket_message.msg_controllen = CMSG_SPACE(sizeof(int));

  if(recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
   return -1;

  if(message_buffer[0] != 'F')
  {
   /* this did not originate from the above function */
   return -1;
  }

  if((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
  {
   /* we did not provide enough space for the ancillary element array */
   return -1;
  }

  /* iterate ancillary elements */
   for(control_message = CMSG_FIRSTHDR(&socket_message);
       control_message != NULL;
       control_message = CMSG_NXTHDR(&socket_message, control_message))
  {
   if( (control_message->cmsg_level == SOL_SOCKET) &&
       (control_message->cmsg_type == SCM_RIGHTS) )
   {
    sent_fd = *((int *) CMSG_DATA(control_message));
    return sent_fd;
   }
  }

  return -1;
 }
key_t key1;
int msgid1;
int usfd;
void answer_time(int signo)
{
    cout<<"it's my time to answer the question \n";
    msgrcv(msgid1,&msg,sizeof(msg),getpid(),0); // only it's msg
    int fd = recv_fd(usfd);
    cout<<"we received client info\n";
    char buf[]="answer is the following";
    send(fd,buf,strlen(buf),0);
}
 int main()
 {

	struct sockaddr_un userv_addr;
  	int userv_len,ucli_len;

  	usfd = socket(AF_UNIX, SOCK_STREAM, 0);

  	if(usfd==-1)
  	perror("\nsocket  ");

  	bzero(&userv_addr,sizeof(userv_addr));
  	userv_addr.sun_family = AF_UNIX;
   	strcpy(userv_addr.sun_path, ADDRESS);

	userv_len = sizeof(userv_addr);

	if(connect(usfd,(struct sockaddr *)&userv_addr,userv_len)==-1)
	perror("\n connect ");

	else printf("\nconnect succesful\n");

    int pid = getpid();
    char buf[100];
    sprintf(buf,"%d\n",pid);
    int n;
    cout<<"enter the no of topics \n";
    cin>>n;
    vector <int> topics(n);
    for(int i=0;i<n;i++)
    {
        cin>>topics[i];
    }
    send(usfd,buf,strlen(buf),0);
    cout<<buf<<endl;
    bzero(buf,sizeof(buf));
    sprintf(buf,"%d\n",n); // no of topics
    send(usfd,buf,strlen(buf),0);
    cout<<buf<<endl;
    for(int i=0;i<n;i++)
    {
        bzero(buf,sizeof(buf));
        sprintf(buf,"%d\n",topics[i]);
        send(usfd,buf,strlen(buf),0);
        cout<<buf<<endl;
    }

    key1=ftok("topicqueue",65); // for sending message to clients
    msgid1=msgget(key1,0666 | IPC_CREAT);
    signal(SIGUSR1,answer_time);
    while(1);
 }