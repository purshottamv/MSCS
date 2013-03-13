/* ============================================================

Project: Wiretap, P538-Project 3
Authors: Harsh Savla, Purshottam Vishwakarma

General principles to be followed for this project:
1. memsetting everything before using
2. freeing everything before exit
3. avoid any magic numbers
4. commenting as much as possible
=============================================================*/
#ifndef WIRETAP_CPP
#define WIRETAP_CPP

#include "PacketInfoCollector.h"
using namespace std;

/* used to print to console */
Logger logger;

PacketInfoCollector packetInfoCollector;		

void print_usage(string MESG_TYPE)
{
	logger.write(MESG_TYPE.c_str(), "Please run the program as follows:");
	logger.write(MESG_TYPE.c_str(),"./wt <name_of_tcpdump_file>");
}

void memset_to_zero(void* arg, int arg_size)
{
	memset(arg, '\0', arg_size);
}

void handleUDP(struct iphdr* iph, int size_ip, const u_char *packet)
{
	struct udphdr* udph = (struct udphdr*)(packet + ETHERNET_HEADER_SIZE + (iph -> ihl)*IP_HEADER_LENGTH_MULTIPLIER);
	// logger.write(INFO,"udp src port:%d, udp dest port:%d, len:%d", ntohs(udph -> source),ntohs(udph -> dest), ntohs(udph -> len));
	
	packetInfoCollector.insertIntoMap(ntohs(udph -> source), PacketInfoCollector::UDP_SRC_PORT_MAP);
	packetInfoCollector.insertIntoMap(ntohs(udph -> dest), PacketInfoCollector::UDP_DEST_PORT_MAP);
}

void handleTCP(struct iphdr* iph, int size_ip, const u_char *packet)
{
	struct tcphdr* tcph = (struct tcphdr*)(packet + ETHERNET_HEADER_SIZE + (iph -> ihl)*IP_HEADER_LENGTH_MULTIPLIER);

	packetInfoCollector.insertIntoMap(ntohs(tcph -> source), PacketInfoCollector::TCP_SRC_PORT_MAP);
	packetInfoCollector.insertIntoMap(ntohs(tcph -> dest), PacketInfoCollector::TCP_DEST_PORT_MAP);
	string temp;

	// TCP Flags
	if(tcph->ack == 1)
		temp.append("ACK,");
	if(tcph->syn == 1)
		temp.append("SYN,");
	if(tcph->fin == 1)
		temp.append("FIN,");
	if(tcph->rst == 1)
		temp.append("RST,");
	if(tcph->psh == 1)
		temp.append("PSH,");
	if(tcph->urg == 1)
		temp.append("URG,");

	packetInfoCollector.insertIntoMap(temp, PacketInfoCollector::TCP_FLAGS);

	unsigned int tcphdrlen = tcph->doff * TCP_HEADER_LENGTH_MULTIPLIER;

	// TCP Options
	if(tcphdrlen > sizeof(*tcph))
	{
		const u_char *tcp_options;
		u_int tcp_option,datalen,len;
		bool TCPOPT_NOP_FLAG = false;
		tcphdrlen = tcphdrlen - sizeof(*tcph);
		tcp_options = (const u_char *)tcph + sizeof(*tcph);
		
		while(tcphdrlen > 0)
		{
			tcp_option = *tcp_options++;

			if(tcp_option == TCPOPT_NOP || tcp_option == TCPOPT_EOL)
				len = 1;
			else
			{
				len = *tcp_options++;	// Total including type and len
				if (len < 2 || len > tcphdrlen)
					break;
				tcphdrlen--;	// For length byte
			}
			tcphdrlen--;		// For type byte
			datalen = 0;

			switch(tcp_option)
			{
                        case TCPOPT_EOL:
							return;
                        case TCPOPT_NOP:
                                /*  Do Nothing */
                                break;
						case TCPOPT_MAXSEG:
                                datalen = 2;
                                break;
                        case TCPOPT_SACK_PERMITTED:
                                break;
                        case TCPOPT_TIMESTAMP:
                                datalen = 8;
                                break;
						case TCPOPT_WINDOW:
								datalen = 1;
                        default:
//                                datalen = len - 2;
								break;
			}
			if(tcp_option == TCPOPT_MAXSEG || tcp_option == TCPOPT_SACK_PERMITTED || tcp_option == TCPOPT_TIMESTAMP || tcp_option == TCPOPT_WINDOW)
				packetInfoCollector.insertIntoMap(tcp_option, PacketInfoCollector::TCP_OPTION_KIND);

			if(tcp_option == TCPOPT_NOP && TCPOPT_NOP_FLAG == false)
			{
				packetInfoCollector.insertIntoMap(tcp_option, PacketInfoCollector::TCP_OPTION_KIND);
				TCPOPT_NOP_FLAG = true;
			}
			tcp_options += datalen;
			tcphdrlen -= datalen;
			++datalen;
			if(tcp_option != TCPOPT_NOP)
				++datalen;
		}
	}
}

/* needs to be filled */
void handleICMP(struct iphdr* iph, int size_ip, const u_char *packet)
{
	struct icmphdr *icmp = (struct icmphdr *)(packet + ETHERNET_HEADER_SIZE + (iph -> ihl)*IP_HEADER_LENGTH_MULTIPLIER);
	packetInfoCollector.insertIntoMap((unsigned short)icmp->type, PacketInfoCollector::ICMP_TYPES_MAP);
	packetInfoCollector.insertIntoMap((unsigned short)icmp->code, PacketInfoCollector::ICMP_CODES_MAP);

	char src_address[INET6_ADDRSTRLEN],dest_address[INET6_ADDRSTRLEN];
	struct in_addr src_in_addr, dest_in_addr;
	memset_to_zero(src_address, INET6_ADDRSTRLEN);
	memset_to_zero(dest_address, INET6_ADDRSTRLEN);

	/* get presentation address for source */
	src_in_addr.s_addr = iph -> saddr;
	inet_ntop(AF_INET, &src_in_addr, src_address, INET6_ADDRSTRLEN);

	/* get presentation address for destination */
	dest_in_addr.s_addr = iph -> daddr;
	inet_ntop(AF_INET, &dest_in_addr, dest_address, INET6_ADDRSTRLEN);

	packetInfoCollector.insertIntoMap(src_address, PacketInfoCollector::ICMP_SRC_MAP);
	packetInfoCollector.insertIntoMap(dest_address, PacketInfoCollector::ICMP_DEST_MAP);
		
	struct icmp_type_code icmp_tc((int)icmp->type,(int)icmp->code);

	packetInfoCollector.insertIntoMap(icmp_tc, PacketInfoCollector::ICMP_TYPES_CODES_MAP);
}

void handleIp(const u_char *packet)
{
	struct iphdr *iph;
	struct in_addr src_in_addr, dest_in_addr;
	int size_ip;
	char src_address[INET6_ADDRSTRLEN];
	char dest_address[INET6_ADDRSTRLEN];

	memset_to_zero(src_address, INET6_ADDRSTRLEN);
	memset_to_zero(dest_address, INET6_ADDRSTRLEN);

	iph = (struct iphdr*)(packet + ETHERNET_HEADER_SIZE);
	size_ip = (iph -> ihl)*IP_HEADER_LENGTH_MULTIPLIER;

	if(size_ip < MIN_IP_HEADER_SIZE)
	{
		logger.write(ERROR,"Invalid IP header length, header length: %d", size_ip);
		return;
	}

	/* get presentation address for source */
	src_in_addr.s_addr = iph -> saddr;
	inet_ntop(AF_INET, &src_in_addr, src_address, INET6_ADDRSTRLEN);

	/* get presentation address for destination */
	dest_in_addr.s_addr = iph -> daddr;
	inet_ntop(AF_INET, &dest_in_addr, dest_address, INET6_ADDRSTRLEN);

	packetInfoCollector.insertIntoMap(src_address, PacketInfoCollector::IP_SRC_MAP);
	packetInfoCollector.insertIntoMap(dest_address, PacketInfoCollector::IP_DEST_MAP);
	packetInfoCollector.insertIntoMap(iph -> ttl, PacketInfoCollector::IP_TTL_MAP);

	//logger.write(INFO, "IP src:%s, IP dest:%s, sizeIP:%d", src_address, dest_address, size_ip);

	switch(iph -> protocol)
	{
		case IPPROTO_ICMP:
			packetInfoCollector.num_icmp_packets++;
			handleICMP(iph, size_ip, packet);
			packetInfoCollector.num_transport_packets++;
			break;

		case IPPROTO_TCP:
			packetInfoCollector.num_tcp_packets++;
			handleTCP(iph, size_ip, packet);
			packetInfoCollector.num_transport_packets++;
			break;

		case IPPROTO_UDP:
			packetInfoCollector.num_udp_packets++;
			handleUDP(iph, size_ip, packet);
			packetInfoCollector.num_transport_packets++;
			break;

		default:
			packetInfoCollector.insertIntoMap((unsigned short)iph -> protocol,PacketInfoCollector::TRANSPORT_PROTOCOLS);
			packetInfoCollector.num_transport_packets++;
			break;
	}
}

/* needs to be filled */
void handleARP(const u_char *packet)
{
	arphdr_t *arp = (arphdr_t *)(packet + ETHERNET_HEADER_SIZE);
	char arp_mac[ETHER_ADDR_LEN],temp[IP_NO_OF_OCTETS];
	char arp_ip[ETHER_ADDR_LEN];
	char ip_address[INET6_ADDRSTRLEN];
	struct in_addr ip_in_addr;
	memset_to_zero(ip_address, INET6_ADDRSTRLEN);
	int i;
	string ipaddress,macaddress;
	/* get presentation address for source */
	
	for(i=0;i<ETH_ALEN;i++)
	{
		memset(temp,'\0',IP_NO_OF_OCTETS);
		sprintf(temp,"%02x",arp->__ar_sha[i]);
		macaddress.append(temp);
		if(i<ETH_ALEN-1)
			macaddress.append(":");
	}
	for(i=0;i<IP_NO_OF_OCTETS;i++)
	{
		memset(temp,'\0',IP_NO_OF_OCTETS);
		sprintf(temp,"%d",arp->__ar_sip[i]);
		ipaddress.append(temp);
		if(i<IP_NO_OF_OCTETS-1)
			ipaddress.append(".");
	}
	packetInfoCollector.insertIntoMap(macaddress,ipaddress);
	if(arp->ar_op == ARPOP_REPLY)
	{
		ipaddress.clear();
		macaddress.clear();
		
		for(i=0;i<ETH_ALEN;i++)
		{
			memset(temp,'\0',IP_NO_OF_OCTETS);
			sprintf(temp,"%02x",arp->__ar_tha[i]);
			macaddress.append(temp);
			if(i<ETH_ALEN-1)
				macaddress.append(":");
		}
		for(i=0;i<IP_NO_OF_OCTETS;i++)
		{
			memset(temp,'\0',IP_NO_OF_OCTETS);
			sprintf(temp,"%d",arp->__ar_tip[i]);
			ipaddress.append(temp);
			if(i<IP_NO_OF_OCTETS-1)
				ipaddress.append(".");
		}
		packetInfoCollector.insertIntoMap(macaddress,ipaddress);
	}
}

void process_packet(u_char* args, const struct pcap_pkthdr *header, const u_char *packet)
{
	int pcap_header_size = header -> len;
	struct ether_header *eth;
	
	eth = (ether_header*)packet;

	string ether_shost(ether_ntoa((const struct ether_addr *)&eth->ether_shost));
	string ether_dhost(ether_ntoa((const struct ether_addr *)&eth->ether_dhost));

//	logger.write(INFO, "ether src:%s, ether dest:%s", ether_shost, ether_dhost);
	
	packetInfoCollector.insertIntoMap(ether_shost,PacketInfoCollector::ETHER_SRC_MAP);
	packetInfoCollector.insertIntoMap(ether_dhost,PacketInfoCollector::ETHER_DEST_MAP);

	if(ntohs(eth -> ether_type) == ETHERTYPE_IP)
	{
		++(packetInfoCollector.num_ip_packets);
		handleIp(packet);
	}
	else if(ntohs(eth -> ether_type) == ETHERTYPE_ARP)
	{
		++(packetInfoCollector.num_arp_packets);
		handleARP(packet);
	}else
	{
		packetInfoCollector.insertIntoMap((unsigned short)ntohs(eth -> ether_type),PacketInfoCollector::NETWORK_PROTOCOLS);
	}
		
	if(header -> len > PacketInfoCollector::maxPacketSize)
		PacketInfoCollector::maxPacketSize = header -> len;
	if(header -> len < PacketInfoCollector::minPacketSize)
		PacketInfoCollector::minPacketSize = header -> len;
		
	PacketInfoCollector::totalPacketSize += header -> len;
	PacketInfoCollector::total_packet_count++;
}

void sniff_packet(char* input_file)
{
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t* pcap_handle;
	u_char *user_arg = NULL;

	/* memsetting */
	memset_to_zero(errbuf, PCAP_ERRBUF_SIZE);

	/* open the file */
	pcap_handle = pcap_open_offline(input_file, errbuf);

	if(pcap_handle == NULL)
	{
		logger.write(ERROR,"Sorry!Could not open the tcpdump file");
		exit(1);
	}

	/* check if packet is captured from ethernet*/
	if(pcap_datalink(pcap_handle) != DLT_EN10MB)
	{
		logger.write(ERROR,"Sorry!Data has not been provided from Ethernet");
		exit(1);
	}
	
	/* start the timer */
	gettimeofday(&(packetInfoCollector.tstart), NULL);
	
	/* looping through the file */
	pcap_loop(pcap_handle,PCAP_END_PROCESSING_NOTIFIER,process_packet, user_arg);
	
	/* end the timer */
	gettimeofday(&(packetInfoCollector.tend), NULL);

	/* close the file */	
	pcap_close(pcap_handle);
}

int main(int argc, char* argv[])
{
	char* input_file;

	if(argc == 2)
	{
		if(!strcmp("help", argv[1]))
		{
			print_usage(INFO);
			exit(1);
		}

		input_file = argv[1];
	}
	else
	{
		print_usage(ERROR);
		exit(1);		
	}
	sniff_packet(input_file);
	packetInfoCollector.printAll();
	packetInfoCollector.freeAll();
}
#endif // WIRETAP_CPP