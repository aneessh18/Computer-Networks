#include<stdio.h>
#include<stdlib.h> 
#include<string.h> 
#include <signal.h>
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<net/ethernet.h>
#include<netinet/ip_icmp.h>   
#include<netinet/udp.h>   
#include<netinet/tcp.h>   
#include<netinet/ip.h>    
int udp=0,tcp=0,icmp=0,others=0,total=0;
struct sockaddr_in source,dest;
void signal_handler(int signo)
{
     printf("Total packets received are %d in number\n",total);
    printf("share of TCP %d \n",tcp);
    printf("share of UDP %d \n",udp);
    printf("share of ICMP %d \n",icmp);
    printf("share of others %d \n",others);
    exit(0);
}
// void process_ip_packet(unsigned char *buffer , int size)
// {
//     unsigned short iphdrlen;

//     struct iphdr *iph = (struct iphdr *)(buffer+sizeof(struct ethhdr));

//     memset(&source, 0, sizeof(source));
//     source.sin_addr.s_addr = iph->saddr;
//     memset(&dest, 0, sizeof(dest));
//     dest.sin_addr.s_addr = iph->daddr;

//     printf("------------------IP HEADER -----------------\n");
//     printf("IP version %d \n",(unsigned int)iph->version);
//     printf("IP header length in bytes %d \n",((unsigned int )(iph->ihl))*4);
//     printf("TTL %d \n",(unsigned int )iph->ttl);
//     printf("Protocol : %d \n",(unsigned int )iph->protocol);
//     printf("source IP %s \n",inet_ntoa(source.sin_addr));
//     printf("dest address : %s \n",inet_ntoa(dest.sin_addr));


// }
void print_tcp_packet(const u_char * Buffer, int Size)
{
    unsigned short iphdrlen;
     
    struct iphdr *iph = (struct iphdr *)( Buffer  + sizeof(struct ethhdr) );
    iphdrlen = iph->ihl*4;
     
    struct tcphdr *tcph=(struct tcphdr*)(Buffer + iphdrlen + sizeof(struct ethhdr));
             
    int header_size =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;
     
    printf("\n\n***********************TCP Packet*************************\n");  
         
         
    printf("\n");
    printf("TCP Header\n");
    printf("   |-Source Port      : %u\n",ntohs(tcph->source));
    printf("   |-Destination Port : %u\n",ntohs(tcph->dest));
    printf("   |-Sequence Number    : %u\n",ntohl(tcph->seq));
    printf("   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
    printf("   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
    printf("   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
    printf("   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
    printf("   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
    printf("   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
    printf( "   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
    printf("   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
    printf( "   |-Window         : %d\n",ntohs(tcph->window));
    printf("   |-Checksum       : %d\n",ntohs(tcph->check));
    printf( "   |-Urgent Pointer : %d\n",tcph->urg_ptr);
    printf( "\n");
    printf( "                        DATA Dump                         ");
    printf( "\n");
                               
    printf( "\n###########################################################");
}
 
void print_udp_packet(const u_char *Buffer , int Size)
{
     
    unsigned short iphdrlen;
     
    struct iphdr *iph = (struct iphdr *)(Buffer +  sizeof(struct ethhdr));
    iphdrlen = iph->ihl*4;
     
    struct udphdr *udph = (struct udphdr*)(Buffer + iphdrlen  + sizeof(struct ethhdr));
     
    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof udph;
     
    printf("\n\n*********************UDP Packet*************************\n");
    
    printf("\nUDP Header\n");
    printf("   |-Source Port      : %d\n" , ntohs(udph->source));
    printf("   |-Destination Port : %d\n" , ntohs(udph->dest));
    printf("   |-UDP Length       : %d\n" , ntohs(udph->len));
    printf("   |-UDP Checksum     : %d\n" , ntohs(udph->check));
     
    printf( "\n###########################################################");
}
 
void print_ethernet_header(const u_char *Buffer, int Size)
{
    struct ethhdr *eth = (struct ethhdr *)Buffer;
     
    printf("\n");
    printf("Ethernet Header\n");
    printf("   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
    printf("   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
    printf("   |-Protocol            : %u \n",(unsigned short)eth->h_proto);
}
 
void print_ip_header(const u_char * Buffer, int Size)
{
    print_ethernet_header(Buffer , Size);
   
    unsigned short iphdrlen;
         
    struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr) );
    iphdrlen =iph->ihl*4;
     
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
     
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;
    total++;
    switch (iph->protocol)
    {
    case IPPROTO_ICMP:icmp++;
        break;
    case IPPROTO_TCP: print_tcp_packet(Buffer,Size); tcp++;
        break;
    case IPPROTO_UDP: print_udp_packet(Buffer,Size); udp++;
        break;
    default: others++;
        break;
    }
    printf("\n");
    printf("IP Header\n");
    printf("   |-IP Version        : %d\n",(unsigned int)iph->version);
    printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
    printf("   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
    printf("   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
    printf("   |-Identification    : %d\n",ntohs(iph->id));
    printf("   |-TTL      : %d\n",(unsigned int)iph->ttl);
    printf("   |-Protocol : %d\n",(unsigned int)iph->protocol);
    printf("   |-Checksum : %d\n",ntohs(iph->check));
    printf("   |-Source IP        : %s\n" , inet_ntoa(source.sin_addr) );
    printf("   |-Destination IP   : %s\n" , inet_ntoa(dest.sin_addr) );
}
 
int main()
{
    int rsfd;
    struct sockaddr_in source, dest;
    unsigned char *buffer = (unsigned char *)malloc(65536);// we receive the packet through this buffer

    rsfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); // to listen all packets on the internet
    signal(SIGINT,signal_handler);
    if(rsfd < 0)
    {
        perror("socket creation ");
        return -1;
    }
    int l = sizeof(struct sockaddr);
    int sz;
    struct sockaddr client;
    while(1) // continuosly receive packets
    {
        sz = recvfrom(rsfd, buffer, 65536, 0, &client, &l);
        if(sz<0)
        {
            perror("recvfrom error");
            exit(0);
        }
        //process_packet(buffer, sz);
        //process_ip_packet(buffer, sz);
       // print_ethernet_header(buffer,sz);
        print_ip_header(buffer,sz);
    }
    return 0;
}