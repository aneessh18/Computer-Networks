#include <bits/stdc++.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#define ADDRESS  "mysocket"
using namespace std;
pthread_t server_threads[100];
int thread_no = 0;
struct info
{
	vector <int> nsfd; // to store the client information
};
map <int,info> server; // to store the information about its pid's and fd's
map <int,int> want_back_up;
// it should receive signals from server to serve the clients

int Spid = -1;

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



void *server_handler(void *arg)
{
	int *num = (int*) arg;
	int spid = *num;
	
	
	int  usfd;
	struct sockaddr_un userv_addr,ucli_addr;
  	int userv_len;

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

	socklen_t ucli_len=sizeof(ucli_addr);

	int nusfd;
	nusfd=accept(usfd, (struct sockaddr *)&ucli_addr, &ucli_len);
	
	if(want_back_up[Spid] == 0) // it's asking for a backup
	{
		char buf[100];
		struct info client_details;
		recv(nusfd, buf, 100, 0); // first it receives a msg, that contain's the no of client's its gonna receive
		int num_of_client = atoi(buf);
		cout<<"no of clients it's gonna pass is "<<num_of_client<<"\n";
		for(int i=0;i<num_of_client;i++)
		{
			int cnsfd = recv_fd(nusfd) ; // getting the client nsfd
			client_details.nsfd.push_back(cnsfd);
		}
		server[spid] = client_details;
		// sending singals to all clients
		for(int i=0;i<num_of_client;i++)
		{
			char msg[] = "hi iam your new boss ";
			send(client_details.nsfd[i], msg, strlen(msg), 0);
		}
		want_back_up[Spid] = 1;
	}
	else
	{	
		cout<<"OK now do your own work \n";
		want_back_up[Spid] = 0;
	}
}
struct sigaction  sa;
void get_pid(int sig, siginfo_t *info, void *context)
{
    Spid = info->si_pid;
}
void backup_func(int signo)
{
	
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = get_pid; 
	// create a thread to handle to server
	cout<<"sender's pid "<<Spid<<"\n";
	//pthread_create(&server_threads[thread_no], NULL, server_handler, (int*) &Spid);
	thread_no++;
	
}
int main()
{
	cout<<"My process id is "<<getpid()<<"\n";
	sigaction(SIGUSR1, &sa, NULL);
	signal(SIGUSR1, backup_func);
	pthread_create(&server_threads[0], NULL, server_handler, (int*) &Spid);
	while(1)
	{
		
	}
	for(int i=0;i<thread_no ;i++)
	pthread_join(server_threads[i], NULL);	
}
