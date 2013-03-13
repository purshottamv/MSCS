#ifndef PACKETINFOCOLLECTOR_H
#define PACKETINFOCOLLECTOR_H

#include "wiretap.h"

struct icmp_type_code
{
	icmp_type_code(int t,int c):type(t),code(c){}
	int type;
	int code;
};

/* stores the data for the packet header's sources, destinations, ports */
class PacketInfoCollector
{
	public:

		int num_arp_packets;
		int num_icmp_packets;
		int num_tcp_packets;
		int num_udp_packets;
		int num_ip_packets;
		int num_transport_packets;
		
		struct timeval tstart, tend;

		struct icmp_cmp
		{
			bool operator() (struct icmp_type_code const a, struct icmp_type_code const b)
			{
				if(a.type > b.type)
					return true;
				else if(a.type < b.type)
					return false;
				else
				{
					if(a.code > b.code)
						return true;
					else
						return false;
				}
			}
		};
		

		/* tells which map to put entry into*/
		enum MapType
		{
			ETHER_SRC_MAP,
			ETHER_DEST_MAP,
			IP_SRC_MAP,
			IP_DEST_MAP,
			IP_TTL_MAP,
			TCP_SRC_PORT_MAP,
			TCP_DEST_PORT_MAP,
			UDP_SRC_PORT_MAP,
			UDP_DEST_PORT_MAP,
			ICMP_SRC_MAP,
			ICMP_DEST_MAP,
			ICMP_TYPES_MAP,
			ICMP_CODES_MAP,
			ICMP_TYPES_CODES_MAP,
			TCP_FLAGS,
			TCP_OPTION_KIND,
			TCP_OPTION,
			NETWORK_PROTOCOLS,
			TRANSPORT_PROTOCOLS,
			};
		
		static unsigned int minPacketSize;
		static unsigned int maxPacketSize;
		static unsigned int totalPacketSize;
		static unsigned int total_packet_count;
		
		/* maps used to store data*/
		map<string, int> ip_src_addresses;
		map<string, int> ip_dest_addresses;
		map<unsigned short, int> ip_ttl_map;
		map<unsigned short, int> tcp_src_ports;
		map<unsigned short, int> tcp_dest_ports;
		map<unsigned short, int> udp_src_ports;
		map<unsigned short, int> udp_dest_ports;
		map<string, int> ether_src_addresses;
		map<string, int> ether_dest_addresses;

		map<string, int> icmp_src_addresses;
		map<string, int> icmp_dest_addresses;		
		map<unsigned short, int> icmp_types;
		map<unsigned short, int> icmp_codes;
		map<struct icmp_type_code, int,icmp_cmp> icmp_types_codes;

		map<string, string> unique_arp_participants;		// Unique ARP Participants
		map<string, int> tcp_flags;
		map<unsigned short, int> tcp_option_kind;
		map<string, int> tcp_option;
		map<unsigned short, int> network_protocols;
		map<unsigned short, int> transport_protocols;

		PacketInfoCollector();

		/* gets the percentage of ip address or ether address */
		double getPercent(string address, MapType mapType);
		double getPercent(unsigned short value, MapType mapType);
		
		#ifndef WIDTH_SPECIFIER 
		#define WIDTH_SPECIFIER 30
		#endif // WIDTH_SPECIFIER
		
		void printTime();
		void printDuration();		
		void printAll();
		void insertIntoMap(string address, MapType mapType);
		void insertIntoMap(unsigned short value, MapType mapType);
		void insertIntoMap(struct icmp_type_code value, MapType mapType);
		void insertIntoMap(string macaddress, string ipaddress);	// This is for ARP
		void freeAll();
};

#ifndef INITIAL_MIN_PACKET_SIZE
#define INITIAL_MIN_PACKET_SIZE UINT_MAX 
#endif // INITIAL_MIN_PACKET_SIZE

#ifndef INITIAL_MAX_PACKET_SIZE
#define INITIAL_MAX_PACKET_SIZE 0
#endif // INITIAL_MAX_PACKET_SIZE

#ifndef INITIAL_PACKET_COUNT
#define INITIAL_PACKET_COUNT 0
#endif // INITIAL_PACKET_COUNT

#ifndef INITIAL_PACKET_SIZE
#define INITIAL_PACKET_SIZE 0
#endif // INITIAL_PACKET_SIZE


#endif // PACKETINFOCOLLECTOR_H

