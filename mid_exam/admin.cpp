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
#define ADDRESS  "/home/indavarapu/Desktop/mid_exam/aneesh"
#define port1 8080
#define port2 8081
#define port3 8082
using namespace std;
struct mesg_buff{    
    long mesg_type;
    char mesg_text[100];
    //pid_t pid;
}msg;
pthread_t accept_thread,receive_thread;
map <int,vector<int> > sfd_to_mod;
map <int,int > sfd_pointers;
map <int,pair<int,int> > modpids_nsfd;
int no_of_mods = 0;
int usfd;
vector <int> mod_fd;


int send_fd(int socket, int fd_to_send)
 {
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  char message_buffer[1];
  /* storage space needed for an ancillary element with a paylod of length is CMSG_SPACE(sizeof(length)) */
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
  int available_ancillary_element_buffer_space;

  /* at least one vector of one byte must be sent */
  message_buffer[0] = 'F';
  io_vector[0].iov_base = message_buffer;
  io_vector[0].iov_len = 1;

  /* initialize socket message */
  memset(&socket_message, 0, sizeof(struct msghdr));
  socket_message.msg_iov = io_vector;
  socket_message.msg_iovlen = 1;

  /* provide space for the ancillary data */
  available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
  memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
  socket_message.msg_control = ancillary_element_buffer;
  socket_message.msg_controllen = available_ancillary_element_buffer_space;

  /* initialize a single ancillary data element for fd passing */
  control_message = CMSG_FIRSTHDR(&socket_message);
  control_message->cmsg_level = SOL_SOCKET;
  control_message->cmsg_type = SCM_RIGHTS;
  control_message->cmsg_len = CMSG_LEN(sizeof(int));
  *((int *) CMSG_DATA(control_message)) = fd_to_send;

  return sendmsg(socket, &socket_message, 0);
 }





void *accepting_topics(void *args)
{
    struct sockaddr_un ucli_addr;
    int ucli_len=sizeof(ucli_addr);

	int nusfd;
	while(1)
    {
        nusfd=accept(usfd, (struct sockaddr *)&ucli_addr,(socklen_t *)&ucli_len);
        perror("accept");
        cout<<"mod connected "<<nusfd<<endl;
        mod_fd.push_back(nusfd);
    }
}

void *receive_info(void *args) // receive info from mod, for what topics they choose to serve
{
    while(1)
    {
        int n = mod_fd.size();
        if (n<=0) continue;
        struct pollfd pfd[n];
        for(int i=0;i<n;i++)
        {
            pfd[i].fd=mod_fd[i];
            pfd[i].events=POLLIN;
        }
        int r = poll(pfd,n,10000);
        if(r>0)
        {
            for(int i=0;i<n;i++)
            {
                if(pfd[i].revents > 0)
                {
                    // first receive pid,no of topics,then topic no's
                    char buf[100];
                    recv(pfd[i].fd, buf, 100, 0);
                    int mod_pid = atoi(buf);
                    cout<<"mod pid is "<<mod_pid<<"\n";
                    bzero(buf,sizeof(buf));
                    recv(pfd[i].fd, buf, 100, 0);
                    int no_of_topics = atoi(buf);
                    cout<<"no of topics is "<<no_of_topics<<"\n";
                    cout<<"the topics are\n";
                    for(int j=0;j<no_of_topics;j++)
                    {
                        bzero(buf, 100);
                        recv(pfd[i].fd, buf, 100, 0);
                        int topic_no = atoi(buf);
                        cout<<topic_no<<" ";
                        sfd_to_mod[topic_no].push_back(pfd[i].fd);
                    }
                    
                    cout<<"\n";
                    modpids_nsfd[i]=make_pair(mod_pid,pfd[i].fd);// storing their details
                }
            }
        }
    }
}
int main()
{
    // usfd part
	struct sockaddr_un userv_addr,ucli_addr;
  	int userv_len,ucli_len;

	usfd = socket(AF_UNIX , SOCK_STREAM , 0);
	perror("socket");

  	bzero(&userv_addr,sizeof(userv_addr));

  	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, ADDRESS);
	unlink(ADDRESS);
	userv_len = sizeof(userv_addr);

	if(bind(usfd, (struct sockaddr *)&userv_addr, userv_len)==-1)
	perror("server: bind");

	listen(usfd, 5);

    // for accepting clients
int sfd1,sfd2,sfd3;
    sfd1=socket(AF_INET,SOCK_STREAM,0);
    sfd2=socket(AF_INET,SOCK_STREAM,0);
    sfd3=socket(AF_INET,SOCK_STREAM,0);
    if(sfd1<0 || sfd2<0 || sfd3 <0)
    {
        perror("socket failed \n");
        return -1;
    }
    // bind the sockets

    struct sockaddr_in server1,server2,server3,client;
    server1.sin_family=AF_INET;
    server1.sin_port=htons(port1);
    server1.sin_addr.s_addr=INADDR_ANY;

    server2.sin_family=AF_INET;
    server2.sin_port=htons(port2);
    server2.sin_addr.s_addr=INADDR_ANY;

    server3.sin_family=AF_INET;
    server3.sin_port=htons(port3);
    server3.sin_addr.s_addr=INADDR_ANY;

    if(bind(sfd1,(struct sockaddr*)&server1,sizeof(server1))<0)
    {
        perror("bind failed");
    }

    if(bind(sfd2,(struct sockaddr*)&server2,sizeof(server2))<0)
    {
        perror("bind failed");
    }

    if(bind(sfd3,(struct sockaddr*)&server3,sizeof(server3))<0)
    {
        perror("bind failed");
    }
    listen(sfd1,3);
    listen(sfd2,3);
    listen(sfd3,3);
    struct pollfd pfd[3];
    pfd[0].fd=sfd1; // linked to server1
    pfd[1].fd=sfd2; // linked to server2
    pfd[2].fd=sfd3; // linked to server3
    pfd[0].events=POLLIN;
    pfd[1].events=POLLIN;
    pfd[2].events=POLLIN;

    key_t key1;
    int msgid1;

    key1=ftok("topicqueue",65); // for sending message to clients
    msgid1=msgget(key1,0666 | IPC_CREAT);
    pthread_create(&accept_thread, NULL,accepting_topics,NULL);
    pthread_create(&receive_thread, NULL,receive_info,NULL);
    int nsfd,len;
    while(1)
    {
        int r = poll(pfd,3,1);
        if(r>0)
        {
            if(pfd[0].revents > 0 )// first topic 
            {
                nsfd = accept(sfd1,(struct sockaddr *) &client,(socklen_t *)&len);
                cout<<"client connected\n";
                perror("accept");
                int mod_no_to_send = sfd_pointers[0];
                int mod_pid = modpids_nsfd[mod_no_to_send].first;
                int mod_fd = modpids_nsfd[mod_no_to_send].second;
                strcpy(msg.mesg_text,"answer this client");
                msg.mesg_type = mod_pid; // should go to respective moderator
                
                msgsnd(msgid1,&msg,sizeof(msg),0);
                send_fd(mod_fd,nsfd);
                kill(mod_pid,SIGUSR1);
                cout<<sfd_to_mod[0].size()<<"size \n";
                sfd_pointers[0]=(sfd_pointers[0]+1)%sfd_to_mod[0].size();
                close(nsfd);
            }
            if(pfd[1].revents > 0) {
                nsfd = accept(sfd2,(struct sockaddr *) &client,(socklen_t *)&len);
                cout<<"client connected\n";
                perror("accept");
                int mod_no_to_send = sfd_pointers[1];
                int mod_pid = modpids_nsfd[mod_no_to_send].first;
                int mod_fd = modpids_nsfd[mod_no_to_send].second;
                strcpy(msg.mesg_text,"answer this client");
                msg.mesg_type = mod_pid; // should go to respective moderator
                msgsnd(msgid1,&msg,sizeof(msg),0);
                send_fd(mod_fd,nsfd);
                kill(mod_pid,SIGUSR1);
                sfd_pointers[1]=(sfd_pointers[1]+1)%sfd_to_mod[1].size();
                close(nsfd);
            }
            if(pfd[2].revents > 0) {

                nsfd = accept(sfd3,(struct sockaddr *) &client,(socklen_t *)&len);
                cout<<"client connected\n";
                perror("accept");
                int mod_no_to_send = sfd_pointers[2];
                int mod_pid = modpids_nsfd[mod_no_to_send].first;
                int mod_fd = modpids_nsfd[mod_no_to_send].second;
                strcpy(msg.mesg_text,"answer this client");
                msg.mesg_type = mod_pid; // should go to respective moderator
                msgsnd(msgid1,&msg,sizeof(msg),0);
                send_fd(mod_fd,nsfd);
                kill(mod_pid,SIGUSR1);
                sfd_pointers[2]=(sfd_pointers[2]+1)%sfd_to_mod[2].size();
                close(nsfd);
            }
        }
    }
    
}
