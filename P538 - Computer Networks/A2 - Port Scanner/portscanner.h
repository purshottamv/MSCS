#ifndef PORTSCANNER_H
#define PORTSCANNER_H

#include<iostream>
#include<sstream>
#include<cstdlib>
#include<stdio.h>
#include<string.h>
#include<getopt.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<netinet/ip.h>
#include<netinet/udp.h>
#include<netinet/ip_icmp.h>
// #include<linux/ipv6.h>
#include<netdb.h>
#include<sys/stat.h>
#include<arpa/inet.h>
#include<errno.h>
#include<sys/wait.h>
#include<time.h>
#include<sys/time.h>
#include<signal.h>
#include<vector>
#include<map>
#include<algorithm>
#include<ctime>
#include<fstream>
#include<queue>
#include<fcntl.h>
#include <stdexcept>
#include "Logger.h"


typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned long u_long;

typedef enum Port_State
{
	OPEN,
	CLOSED,
	FILTERED,
	UNFILTERED,	
	OPEN_OR_FILTERED,
}Port_State;

// Pseudo Header
struct pseudo_hdr
{
	u_int32_t src;          /* 32bit source ip address*/
	u_int32_t dst;          /* 32bit destination ip address */	
	u_char mbz;             /* 8 reserved bits (all 0) 	*/
	u_char proto;           /* protocol field of ip header */
	u_int16_t len;          /* tcp length (both header and data */

	struct tcphdr tcp;
}pseudo_hdr;

struct pseudo_hdr_udp
{
	u_int32_t src;          /* 32bit source ip address*/
	u_int32_t dst;          /* 32bit destination ip address */	
	u_char mbz;             /* 8 reserved bits (all 0) 	*/
	u_char proto;           /* protocol field of ip header */
	u_int16_t len;          /* tcp length (both header and data */

	struct udphdr udp;
}pseudo_hdr_udp;

struct resulttype
{
	char protocol_type[20];
	char scan_type[10];
	char state[20];
	char service[50];
	char additional_info[30];
};

#define RANDOM_DESTINATION_PORT		4567

#define IP_HEADER_LENGTH	5
#define IP_VERSION_4		4
#define IP_VERSION_6		6
#define IP_IDENTIFIER		34644
#define IP_TIME_TO_LIVE		128

#define TCP_WINDOW_SIZE		15000

#define PCKT_LEN 4096
#define ADDR_BUFFER 100
#define RECV_BUFFER	4096

#define ICMP_TYPE_3	3

#define SERVICE_RETRY_TIME_INTERVAL 1

#define RETRY_TIME_INTERVAL				2	// in seconds
#define NUMBER_OF_RETRY					2	// in seconds
#define NUMBER_OF_RETRY_PROTOCOL_SCAN	3	// in seconds

#define DNSIP "8.8.8.8"

#define		NUMBER_OF_SCAN_TYPES	6
#define		SCAN_TYPE_SYN	0
#define		SCAN_TYPE_NULL	1
#define		SCAN_TYPE_FIN	2
#define		SCAN_TYPE_XMAS	3
#define		SCAN_TYPE_ACK	4
#define		SCAN_TYPE_PROTOCOL	5

Logger log("log.txt");

#endif		// PORTSCANNER_H
