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
 
#define BUF_SIZE 42
#define DEVICE "enp3s0"
struct arp_header {
  uint16_t htype;
  uint16_t ptype;
  uint8_t hlen;
  uint8_t plen;
  uint16_t opcode;
  uint8_t sender_mac[6];
  uint8_t sender_ip[4];
  uint8_t target_mac[6];
  uint8_t target_ip[4];
};

void* buffer = NULL;
long total_packets = 0;
long answered_packets = 0;

void print_mac_address(uint8_t mac[6])
{
  for (int i=0; i<5; i++) {
    printf ("%02x:", mac[i]);
  }
  printf ("%02x\n", mac[5]);
   
}
void print_mac(unsigned char add[6])
{
  for (int i=0; i<5; i++) {
    printf ("%02x:", add[i]);
  }
  printf ("%02x\n", add[5]);
   
}
void print_ip_address(uint8_t ip[4])
{
  for(int i=0;i<4;i++)
  {
    printf("%d.",ip[i]);
  }
  printf("\n");
}
int main()
{
    buffer = (void*)malloc(BUF_SIZE); // ethernet frame 
    unsigned char* etherhead = buffer;   
    struct ethhdr *eh = (struct ethhdr *)etherhead; // pointer to extract ethernet header
    unsigned char* arphead = buffer + 14;
    struct arp_header *ah = (struct arp_header *)arphead; // pointer to extract arp header
    uint8_t src_mac[6],src_ip[4],dst_mac[6],dst_ip[4];        /*our MAC address*/
 
    struct ifreq ifr;
    struct sockaddr_ll socket_address;
    int ifindex = 0;         /*Ethernet Interface index*/
    int i;
    int length;      /*length of received packet*/
    int sent;

    int rsfd = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(rsfd < 0)
    {
        perror("socket failed \n");
        exit(EXIT_FAILURE);
    }
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
    printf("Source mac address \n");
    print_mac_address(src_mac);
    int c = 0;
    socket_address.sll_family = PF_PACKET;
  socket_address.sll_protocol = htons(ETH_P_ARP);
  socket_address.sll_ifindex = ifindex;
  socket_address.sll_hatype = ARPHRD_ETHER;
  socket_address.sll_pkttype = 0; //PACKET_OTHERHOST;
  socket_address.sll_halen = 6;
  memcpy (socket_address.sll_addr, src_mac, 6 * sizeof (uint8_t));
  
        while(c<3)
    {
        int sz = recvfrom(rsfd, buffer, BUF_SIZE, 0, NULL, NULL); // we got the packet

        if(ntohs(eh->h_proto) == ETH_P_ARP && ntohs(ah->opcode) == ARPOP_REQUEST)
        {
            if(ntohs(ah->opcode) == ARPOP_REPLY)
            {
              printf("it's arp reply packet\n");
            }
            else if(ntohs(ah->opcode) == ARPOP_REQUEST)
            {
              printf("it's arp request packet\n");
            }
            printf("--------------ETHERNET HEADER-----------------\n");
            printf("source mac address --> ");
            print_mac(eh->h_source);
            printf("destination mac address --> ");
            print_mac(eh->h_dest);
            printf("--------------ARP HEADER -----------------\n");
            printf("source mac address ---> ");
            print_mac_address(ah->sender_mac);
            printf("source ip address --> ");
            print_ip_address(ah->sender_ip);
            printf("destination mac address ---> ");
            print_mac_address(ah->target_mac);
            printf("destination ip address ---> ");
            print_ip_address(ah->target_ip);
            printf("ARP packet type %d \n",ah->opcode);
            c++;

            printf("\n \n \n");
        }

    }
}