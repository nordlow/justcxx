/***************************************************
 * file:     testpcap1.c
 * Date:     Thu Mar 08 17:14:36 MST 2001
 * Author:   Martin Casado
 * Location: LAX Airport (woof!)
 *
 * Simple single packet capture program
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h> /* includes net/ethernet.h */
#include <time.h>

int main(int argc, char **argv)
{
  int i;
  char *dev;
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t* descr;
  const u_char *packet;
  struct pcap_pkthdr hdr;     /* pcap.h */
  struct ether_header *eptr;  /* net/ethernet.h */

  u_char *ptr; /* printing out hardware header info */

  dev = pcap_lookupdev(errbuf);
  if (dev == NULL) {
    printf("%s\n",errbuf);
    exit(1);
  }
  printf("DEV: %s\n",dev);

  /* get packet capture descriptor from capture.  Note if you change
     "prmisc" param to anything other than zero, you will get all
     packets your device sees, whether they are intendeed for you or
     not!! Be sure you know the rules of the network you are running
     on before you set your card in promiscuous mode or you could get
     yourself in serious doo doo!!! (also need to be root to run in
     promisuous mode)                                               */
  descr = pcap_open_live(dev,BUFSIZ,0,-1,errbuf);
  if (descr == NULL) {
    printf("pcap_open_live(): %s\n",errbuf);
    exit(1);
  }

  packet = pcap_next(descr,&hdr);
  if (packet == NULL) {
    printf("Didn't grab packet\n");
    exit(1);
  }

  printf("Grabbed packet of length %d\n",hdr.len);
  printf("Recieved at ..... %s\n",ctime((const time_t*)&hdr.ts.tv_sec));
  printf("Ethernet address length is %d\n",ETHER_HDR_LEN);

  eptr = (struct ether_header *) packet;

  if (ntohs (eptr->ether_type) == ETHERTYPE_IP) {
    printf("Ethernet type hex:%x dec:%d is an IP packet\n",
           ntohs(eptr->ether_type),
           ntohs(eptr->ether_type));
  } else if (ntohs (eptr->ether_type) == ETHERTYPE_ARP) {
    printf("Ethernet type hex:%x dec:%d is an ARP packet\n",
           ntohs(eptr->ether_type),
           ntohs(eptr->ether_type));
  } else {
    printf("Ethernet type %x not IP", ntohs(eptr->ether_type));
    exit(1);
  }

  ptr = eptr->ether_dhost;
  i = ETHER_ADDR_LEN;
  printf(" Destination Address:  ");
  do {
    printf("%s%x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);
  } while(--i>0);
  printf("\n");

  ptr = eptr->ether_shost;
  i = ETHER_ADDR_LEN;
  printf(" Source Address:  ");
  do {
    printf("%s%x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);
  } while(--i>0);
  printf("\n");

  return 0;
}
