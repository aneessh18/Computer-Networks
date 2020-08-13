#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h> 
#include <netinet/ip_icmp.h> 
#include <time.h> 
#include <fcntl.h> 
#include <signal.h> 
#include <sys/time.h> 

unsigned short in_cksum(unsigned short *addr, int len)
{
	int nleft = len;
	int sum = 0;
	unsigned short *w = addr;
	unsigned short answer = 0;

	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		*(unsigned char *) (&answer) = *(unsigned char *) w;
		sum += answer;
	}
	
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	answer = ~sum;
	return (answer);
}


int main()
{
    struct sockaddr_in source, dest;
    unsigned long daddr,saddr;
    int max_hops = 30;
    int msg_count = 0;
    saddr = INADDR_ANY;
    daddr = inet_addr("216.58.200.142");
    int rsfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(rsfd < 0)
    {
        perror("raw socket ");
        exit(-1);
    }
    int on = 1;
    if(setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, (const char *)&on, sizeof(on))<0)
    {
        perror("set sock opt");
        exit(-1);
    }
    int pcksize = sizeof(struct iphdr) + sizeof(struct icmphdr);
    char *buffer = (char*)malloc(pcksize);

    struct iphdr *ip = (struct iphdr *)buffer;
    struct icmphdr *icmp = (struct icmphdr *)(buffer + sizeof(struct iphdr));
    memset(buffer, 0, pcksize);
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = daddr;
    memset(dest.sin_zero,0,sizeof(dest));


    for(int i=1;i<=max_hops;i++)
    {
        ip->version = 4;
	    ip->ihl = 5;
	    ip->tos = 0;
	    ip->tot_len = htons (pcksize);
	    ip->id = rand ();
	    ip->frag_off = 0;
	    ip->ttl = i;
	    ip->protocol = IPPROTO_ICMP;
	    ip->saddr = saddr;
	    ip->daddr = daddr;
        ip->check = 0;
        ip->check = in_cksum ((unsigned short int *) ip, sizeof (struct iphdr));

        icmp->type = ICMP_ECHO;
	    icmp->code = 0;
  	    icmp->un.echo.sequence = msg_count++;
  	    icmp->un.echo.id = getpid();
  	    icmp->checksum = 0;
		icmp->checksum = in_cksum((unsigned short *)icmp, sizeof(struct icmphdr));
        if((sendto (rsfd, buffer, pcksize, 0, (struct sockaddr *) & dest, sizeof (dest)))<0)
        {
            perror("sendto ");
            exit(-1);
        }
        struct sockaddr_in client;
        socklen_t len = sizeof (struct sockaddr_in);
        memset(buffer, 0, pcksize);
        if((recvfrom (rsfd, buffer, pcksize, 0, (struct sockaddr *) & client, &len))<0)
        {
            perror("recv from");
            exit(-1);
        }
        struct icmphdr *icmphd2 = (struct icmphdr *) (buffer + sizeof(struct iphdr));
        if (icmphd2->type != 0)
            printf ("hop limit:%d Address:%s\n", i, inet_ntoa (client.sin_addr));
        else
        {
            printf ("Reached destination:%s with hop limit:%d\n",inet_ntoa (client.sin_addr), i);
            exit (0);
        }
    }


}