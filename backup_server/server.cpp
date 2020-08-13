#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <arpa/inet.h>
#define ADDRESS     "mysocket"
using namespace std;

pthread_t connect_thread;
vector <int> nsfd_array;
struct sockaddr_in cli_addr;
int sfd;

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
 void *connect_func(void *args)
{
	while(1)
	{
		int nsfd;
	socklen_t cli_len;
	if((nsfd = accept(sfd , (struct sockaddr *)&cli_addr , &cli_len))==-1)
	perror("\n accept ");
	else printf("\n accept successful");
	nsfd_array.push_back(nsfd); // collected the nsfd array
	}
}
int main()
{
	
	struct sockaddr_in serv_addr;
	
	int port_no=8080;

	if((sfd = socket(AF_INET,SOCK_STREAM,0))==-1)
	perror("\n socket ");
	else printf("\n socket created successfully");

	bzero(&serv_addr,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))==-1)
	perror("\n bind : ");
	else printf("\n bind successful ");

	listen(sfd,10);
	int x = 0;
	pthread_create(&connect_thread, NULL, connect_func, NULL);
	int c = 0;
	while(1)
	{
		
		int n = nsfd_array.size();
		cout<<n<<"\n";
		struct pollfd pfd[n];
		for(int i=0;i<n;i++)
		{
			pfd[i].fd = nsfd_array[i];
			pfd[i].events = POLLIN;
		}
		int r = poll(pfd, n ,5000);
		if(r > 0)
		{
			c++;
			for(int i=0;i<n;i++)
			{
				if(pfd[i].revents > 0)
				{
					char buf[100];
					recv(pfd[i].fd, buf, 100, 0);
					cout<<buf<<" : received \n";
					for(int j=0;j<n;j++) // forward the message
					{
						if ( pfd[j].fd != pfd[i].fd )
						{
							send(pfd[j].fd, buf, strlen(buf), 0);
						}
					}
				}
			}
		}
		if(c>3)
		{
			int back_up_pid ;
			cout<<"enter back up pid \n";
			cin>>back_up_pid;
			//kill(back_up_pid,SIGUSR1);
			char num_of_client[100];
			sprintf(num_of_client,"%d",n);
		

			int usfd;
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

			else printf("\nconnect succesful");
			send(usfd, num_of_client, strlen(num_of_client), 0);
			cout<<"sending the fd's "<<endl;
			for(int i=0;i<n;i++)
			{
				send_fd(usfd, pfd[i].fd);
			}
			sleep(60);
			//kill(back_up_pid,SIGUSR1);

		}		
	}
	pthread_join(connect_thread,NULL);
}
