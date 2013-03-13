#ifndef WIRETAP_H
#define WIRETAP_H

#include<iostream>
#include<sstream>
#include<cstdlib>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<netinet/ip.h>
#include<netinet/udp.h>
#include<netinet/ip_icmp.h>
#include<net/ethernet.h>
#include<net/if_arp.h>
#include<netinet/ether.h>
#include<arpa/inet.h>
#include<errno.h>
#include<limits.h>
#include<time.h>
#include<sys/time.h>
#include<vector>
#include<map>
#include<algorithm>
#include<ctime>
#include<iomanip>
#include<fstream>
#include<queue>
#include<fcntl.h>
#include<math.h>
#include <pcap.h>
#include "Logger.h"

#define PCAP_END_PROCESSING_NOTIFIER 0
#define ETHERNET_HEADER_SIZE 14
#define ETHERNET_ADDRLEN 6
#define MIN_IP_HEADER_SIZE 20
#define IP_HEADER_LENGTH_MULTIPLIER 4
#define TCP_HEADER_LENGTH_MULTIPLIER 4
#define IP_NO_OF_OCTETS		4

typedef struct arpheader
{
    unsigned short int ar_hrd;          /* Format of hardware address.  */
    unsigned short int ar_pro;          /* Format of protocol address.  */
    unsigned char ar_hln;               /* Length of hardware address.  */
    unsigned char ar_pln;               /* Length of protocol address.  */
    unsigned short int ar_op;           /* ARP opcode (command).  */
    unsigned char __ar_sha[ETH_ALEN];   /* Sender hardware address.  */
    unsigned char __ar_sip[4];          /* Sender IP address.  */
    unsigned char __ar_tha[ETH_ALEN];   /* Target hardware address.  */
    unsigned char __ar_tip[4];          /* Target IP address.  */
}arphdr_t;


#endif // WIRETAP_H