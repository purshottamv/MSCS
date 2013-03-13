#include "PacketInfoCollector.h"

PacketInfoCollector::PacketInfoCollector()
{
	num_tcp_packets = 0;
	num_icmp_packets = 0;
	num_udp_packets = 0;
}

/* gets the percentage of ip address or ether address */
double PacketInfoCollector::getPercent(string address, MapType mapType)
{	
	switch(mapType)
	{
		case ETHER_SRC_MAP:
			return ((double)((ether_src_addresses[address]*100.0)/total_packet_count));	
		
		case ETHER_DEST_MAP:
			return ((double)((ether_dest_addresses[address]*100.0)/total_packet_count));	
		
		case IP_SRC_MAP:
			return ((double)((ip_src_addresses[address]*100.0)/num_ip_packets));
		
		case IP_DEST_MAP:
			return ((double)((ip_dest_addresses[address]*100.0)/num_ip_packets));

		case ICMP_SRC_MAP:
			return ((double)((icmp_src_addresses[address]*100.0)/num_icmp_packets));
		
		case ICMP_DEST_MAP:
			return ((double)((icmp_dest_addresses[address]*100.0)/num_icmp_packets));

		case TCP_FLAGS:
			return ((double)((tcp_flags[address]*100.0)/num_tcp_packets));

		case TCP_OPTION:
			return ((double)((tcp_option[address]*100.0)/num_tcp_packets));
	
	}
}

double PacketInfoCollector::getPercent(unsigned short value, MapType mapType)
{	
	switch(mapType)
	{
		case TCP_SRC_PORT_MAP:
			return ((double)((tcp_src_ports[value]*100.0)/num_tcp_packets));
			
		case TCP_DEST_PORT_MAP:
			return ((double)((tcp_dest_ports[value]*100.0)/num_tcp_packets));
			
		case UDP_SRC_PORT_MAP:
			return ((double)((udp_src_ports[value]*100.0)/num_udp_packets));
		
		case UDP_DEST_PORT_MAP:
			return ((double)((udp_dest_ports[value]*100.0)/num_udp_packets));
			
		case IP_TTL_MAP:
			return ((double)((ip_ttl_map[value]*100.0)/num_ip_packets));

		case ICMP_TYPES_MAP:
			return ((double)((icmp_types[value]*100.0)/num_icmp_packets));	

		case ICMP_CODES_MAP:
			return ((double)((icmp_codes[value]*100.0)/num_icmp_packets));

		case TCP_OPTION_KIND:
			return ((double)((tcp_option_kind[value]*100.0)/num_tcp_packets));	

		case NETWORK_PROTOCOLS:
			return ((double)((network_protocols[value]*100.0)/total_packet_count));	
		
		case TRANSPORT_PROTOCOLS:
			return ((double)((transport_protocols[value]*100.0)/num_transport_packets));	
	}
}

void PacketInfoCollector::printTime()
{
	time_t current_time = time(0);
	struct tm *now = localtime(&current_time);
	cout << setw(WIDTH_SPECIFIER) << "\tStart Date: " <<(now -> tm_year + 1900)<< '-' << (now -> tm_mon + 1) << '-' << (now -> tm_mday) << endl;
}

void PacketInfoCollector::printDuration()
{
	double elapsed_time = (tend.tv_sec - tstart.tv_sec)*1000.0;
	elapsed_time += (tend.tv_usec - tstart.tv_usec) / 1000.0;
	cout << setw(WIDTH_SPECIFIER) << "\tDuration: " << elapsed_time << " ms" << endl;
}

void PacketInfoCollector::printAll()
{
	map<string,int>::iterator it1;
	map<unsigned short,int>::iterator it2;
	map<string,string>::iterator it3;
	cout.setf(ios::fixed, ios::floatfield);
	cout.setf(ios::showpoint);	
	
	cout << endl << "======================== Summary ===============================" << endl << setprecision(2)<<endl;
	printTime();
	printDuration();
	cout << setw(WIDTH_SPECIFIER) <<"\t# Packets: " << total_packet_count << endl;
	cout << setw(WIDTH_SPECIFIER) <<"\tSmallest: " << minPacketSize << endl;
	cout << setw(WIDTH_SPECIFIER) <<"\tLargest: " << maxPacketSize << endl;
	cout << setw(WIDTH_SPECIFIER) <<"\tAverage: " << (double)(1.0*totalPacketSize/total_packet_count) << endl;
//	cout << setw(WIDTH_SPECIFIER) <<"\tNumber of TCP Packets: " << num_tcp_packets << endl;
	cout << "================================================================" << endl << endl;
	
	cout << "=== Link Layer ===" << endl;
	
	cout << "--- Source Ethernet addresses ---" << endl;
	for(it1 = ether_src_addresses.begin(); it1 != ether_src_addresses.end(); it1++)
		cout << setw(WIDTH_SPECIFIER) << it1 -> first << setw(WIDTH_SPECIFIER) << it1 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it1 -> first,ETHER_SRC_MAP)<<"%"<< endl;
	cout << "\n" << endl;

	cout << "--- Destination Ethernet addresses ---" << endl;
	for(it1 = ether_dest_addresses.begin(); it1 != ether_dest_addresses.end(); it1++)
		cout << setw(WIDTH_SPECIFIER) << it1 -> first << setw(WIDTH_SPECIFIER) << it1 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it1 -> first, ETHER_DEST_MAP)<<"%" << endl;
	cout << "\n" << endl;
	
	cout << "=========================== Network Layer ==============================" << endl;
	
	cout << "--- Network Layer protocols --- " << endl;
	for(it2 = network_protocols.begin(); it2 != network_protocols.end(); it2++)
		cout << setw(WIDTH_SPECIFIER) <<"0x00"<<hex << it2 -> first << dec <<setw(WIDTH_SPECIFIER) << it2 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it2 -> first, NETWORK_PROTOCOLS)<<"%" << endl;
	cout << setw(WIDTH_SPECIFIER) << "ARP" << setw(WIDTH_SPECIFIER) << num_arp_packets << 
			setw(WIDTH_SPECIFIER) << ((double)num_arp_packets/(double)(num_arp_packets + num_ip_packets))*100<<"%" << endl;	
	cout << setw(WIDTH_SPECIFIER) << "IP" << setw(WIDTH_SPECIFIER) << num_ip_packets << 
			setw(WIDTH_SPECIFIER) << ((double)num_ip_packets/(double)(num_arp_packets + num_ip_packets))*100<<"%" << endl;	
	/* print ip src and dest address */
	cout << "\n--- Source IP addresses ---" << endl;
	for(it1 = ip_src_addresses.begin(); it1 != ip_src_addresses.end(); it1++)
		cout << setw(WIDTH_SPECIFIER) << it1 -> first << setw(WIDTH_SPECIFIER) << it1 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it1 -> first, IP_SRC_MAP)<<"%" << endl;
	cout << "\n" << endl;

	cout << "--- Destination IP addresses ---" << endl;
	for(it1 = ip_dest_addresses.begin(); it1 != ip_dest_addresses.end(); it1++)
		cout << setw(WIDTH_SPECIFIER) << it1 -> first << setw(WIDTH_SPECIFIER) << it1 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it1 -> first, IP_DEST_MAP)<<"%" << endl;
	cout << "\n" << endl;
	
	cout << "--- TTLs --- " << endl;
	for(it2 = ip_ttl_map.begin(); it2 != ip_ttl_map.end(); it2++)
		cout << setw(WIDTH_SPECIFIER) << it2 -> first << setw(WIDTH_SPECIFIER) << it2 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it2 -> first, IP_TTL_MAP)<<"%" << endl;
	cout << "\n" << endl;
	
	cout << "--- Unique ARP Participants --- " << endl;
	for(it3 = unique_arp_participants.begin(); it3 != unique_arp_participants.end(); it3++)
		cout << it3 -> first << "\t\\\t" << it3 -> second<<endl;
	
	cout << "\n============================= Transport layer === " << endl;
	
	cout << "--- Transport Layer protocols --- " << endl;
	for(it2 = transport_protocols.begin(); it2 != transport_protocols.end(); it2++)
		cout << setw(WIDTH_SPECIFIER) <<"0x00"<<hex << it2 -> first << dec <<setw(WIDTH_SPECIFIER) << it2 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it2 -> first, TRANSPORT_PROTOCOLS)<<"%" << endl;
	cout << setw(WIDTH_SPECIFIER) << "ICMP" << setw(WIDTH_SPECIFIER) << num_icmp_packets << 
			setw(WIDTH_SPECIFIER) << ((double)num_icmp_packets/(double)(num_transport_packets))*100<<"%" << endl;
	cout << setw(WIDTH_SPECIFIER) << "TCP" << setw(WIDTH_SPECIFIER) << num_tcp_packets << 
			setw(WIDTH_SPECIFIER) << ((double)num_tcp_packets/(double)(num_transport_packets))*100<<"%" << endl;
	cout << setw(WIDTH_SPECIFIER) << "UDP" << setw(WIDTH_SPECIFIER) << num_udp_packets << 
			setw(WIDTH_SPECIFIER) << ((double)num_udp_packets/(double)(num_transport_packets))*100<<"%" << endl;

	cout << "=== Transport layer:TCP === " << endl;

	cout << "--- Source TCP ports ---" << endl;
	for(it2 = tcp_src_ports.begin(); it2 != tcp_src_ports.end(); it2++)
		cout << setw(WIDTH_SPECIFIER) << it2 -> first << setw(WIDTH_SPECIFIER) << it2 -> second <<
			setw(WIDTH_SPECIFIER) << getPercent(it2 -> first, TCP_SRC_PORT_MAP)<<"%" << endl;
	cout << "\n";

	cout << "--- Destination TCP ports ---" << endl;
	for(it2 = tcp_dest_ports.begin(); it2 != tcp_dest_ports.end(); it2++)
		cout << setw(WIDTH_SPECIFIER) << it2 -> first << setw(WIDTH_SPECIFIER) << it2 -> second <<
			setw(WIDTH_SPECIFIER) << getPercent(it2 -> first, TCP_DEST_PORT_MAP)<<"%" << endl;
	cout << "\n";
	
	cout << "--- TCP flags --- " << endl;
	for(it1 = tcp_flags.begin(); it1 != tcp_flags.end(); it1++)
		cout << setw(WIDTH_SPECIFIER) << it1 -> first << setw(WIDTH_SPECIFIER) << it1 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it1 -> first, TCP_FLAGS)<<"%" << endl;
	cout << "\n" << endl;
	
	cout << "--- TCP Options --- " << endl;
	for(it2 = tcp_option_kind.begin(); it2 != tcp_option_kind.end(); it2++)
		cout << setw(WIDTH_SPECIFIER) <<"0x00"<<hex<<it2 -> first<<dec << setw(WIDTH_SPECIFIER) << it2 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it2 -> first, TCP_OPTION_KIND)<<"%" << endl;
	cout << "\n" << endl;

	cout << "=== Transport layer:UDP === " << endl;
	
	cout << "--- Source UDP ports ---" << endl;                         	
		for(it2 = udp_src_ports.begin(); it2 != udp_src_ports.end(); it2++)
			cout << setw(WIDTH_SPECIFIER) << it2 -> first << setw(WIDTH_SPECIFIER) << it2 -> second << 
				setw(WIDTH_SPECIFIER) << getPercent(it2 -> first, UDP_SRC_PORT_MAP)<<"%" << endl;
	cout << "\n";

	cout << "--- Destination UDP ports ---" << endl;
	for(it2 = udp_dest_ports.begin(); it2 != udp_dest_ports.end(); it2++)
		cout << setw(WIDTH_SPECIFIER) << it2 -> first << setw(WIDTH_SPECIFIER) << it2 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it2 -> first, UDP_DEST_PORT_MAP)<<"%" << endl;
	cout << "\n";
	
	cout << "\t============================= Transport layer:ICMP ============================= " << endl;
	
	cout << "--- Source IPs for ICMP --- " << endl;
	for(it1 = icmp_src_addresses.begin(); it1 != icmp_src_addresses.end(); it1++)
		cout << setw(WIDTH_SPECIFIER) << it1 -> first << setw(WIDTH_SPECIFIER) << it1 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it1 -> first, ICMP_SRC_MAP)<<"%" << endl;
	
	cout << "\n--- Destination IPs for ICMP --- " << endl;
	for(it1 = icmp_dest_addresses.begin(); it1 != icmp_dest_addresses.end(); it1++)
		cout << setw(WIDTH_SPECIFIER) << it1 -> first << setw(WIDTH_SPECIFIER) << it1 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it1 -> first, ICMP_DEST_MAP)<<"%" << endl;
	
	cout << "\n--- ICMP types --- " << endl;
	for(it2 = icmp_types.begin(); it2 != icmp_types.end(); it2++)
		cout << setw(WIDTH_SPECIFIER) << it2 -> first << setw(WIDTH_SPECIFIER) << it2 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it2 -> first, ICMP_TYPES_MAP)<<"%" << endl;
	
	cout << "\n--- ICMP codes --- " << endl;
	for(it2 = icmp_codes.begin(); it2 != icmp_codes.end(); it2++)
		cout << setw(WIDTH_SPECIFIER) << it2 -> first << setw(WIDTH_SPECIFIER) << it2 -> second << 
			setw(WIDTH_SPECIFIER) << getPercent(it2 -> first, ICMP_CODES_MAP)<<"%" << endl;	
	cout << "\n--- ICMP responses --- " << endl;
	
	cout << " ================================================================== " << endl;
}

void PacketInfoCollector::insertIntoMap(string macaddr, string ipaddr)
{		
	map<string, string>::iterator it = this -> unique_arp_participants.find(macaddr);
	if(it == this -> unique_arp_participants.end())
		this -> unique_arp_participants[macaddr] = ipaddr;
}

void PacketInfoCollector::insertIntoMap(string address, MapType mapType)
{		
	switch(mapType)
	{
		case IP_SRC_MAP:
		{
			map<string, int>::iterator it = this -> ip_src_addresses.find(address);
			if(it != this -> ip_src_addresses.end())
				this -> ip_src_addresses[address] = this -> ip_src_addresses[address] + 1;
			else
				this -> ip_src_addresses[address] = 1;
		}
		break;

		case IP_DEST_MAP:
		{
			map<string, int>::iterator it = this -> ip_dest_addresses.find(address);
			if(it != ip_dest_addresses.end())
				this -> ip_dest_addresses[address] = this -> ip_dest_addresses[address] + 1;
			else
				this -> ip_dest_addresses[address] = 1;
		}
		break;

		case ETHER_SRC_MAP:
		{
			map<string, int>::iterator it = this -> ether_src_addresses.find(address);
			if(it != ip_dest_addresses.end())
				this -> ether_src_addresses[address] = this -> ether_src_addresses[address] + 1;
			else
				this -> ether_src_addresses[address] = 1;
		}
		break;

		case ETHER_DEST_MAP:
		{
			map<string, int>::iterator it = this -> ether_dest_addresses.find(address);
			if(it != ether_dest_addresses.end())
				this -> ether_dest_addresses[address] = this -> ether_dest_addresses[address] + 1;
			else
				this -> ether_dest_addresses[address] = 1;
		}
		break;

		case ICMP_SRC_MAP:
		{
			map<string, int>::iterator it = this -> icmp_src_addresses.find(address);
			if(it != icmp_src_addresses.end())
				this -> icmp_src_addresses[address]++;
			else
				this -> icmp_src_addresses[address] = 1;
		}
		break;

		case ICMP_DEST_MAP:
		{
			map<string, int>::iterator it = this -> icmp_dest_addresses.find(address);
			if(it != icmp_dest_addresses.end())
				this -> icmp_dest_addresses[address]++;
			else
				this -> icmp_dest_addresses[address] = 1;
		}
		break;

		case TCP_FLAGS:
		{
			map<string, int>::iterator it = this -> tcp_flags.find(address);
			if(it != tcp_flags.end())
				this -> tcp_flags[address]++;
			else
				this -> tcp_flags[address] = 1;
		}
		break;

		case TCP_OPTION:
		{
			map<string, int>::iterator it = this -> tcp_option.find(address);
			if(it != tcp_option.end())
				this -> tcp_option[address]++;
			else
				this -> tcp_option[address] = 1;
		}
		break;
	}
}

void PacketInfoCollector::insertIntoMap(unsigned short value, MapType mapType)
{
	switch(mapType)
	{
		case TCP_SRC_PORT_MAP:
		{
			map<unsigned short, int>::iterator it = this -> tcp_src_ports.find(value);
			if(it != tcp_src_ports.end())
				this -> tcp_src_ports[value] = this -> tcp_src_ports[value] + 1;
			else
				this -> tcp_src_ports[value] = 1;
		}
		break;

		case TCP_DEST_PORT_MAP:
		{
			map<unsigned short, int>::iterator it = this -> tcp_dest_ports.find(value);
			if(it != tcp_dest_ports.end())
				this -> tcp_dest_ports[value] = this -> tcp_dest_ports[value] + 1;
			else
				this -> tcp_dest_ports[value] = 1;
		}
		break;
		
		case IP_TTL_MAP:
		{
			map<unsigned short, int>::iterator it = this -> ip_ttl_map.find(value);
			if(it != ip_ttl_map.end())
				this -> ip_ttl_map[value] = this -> ip_ttl_map[value] + 1;
			else
				this -> ip_ttl_map[value] = 1;
		}
		break;

		case UDP_SRC_PORT_MAP:
		{
			map<unsigned short, int>::iterator it = this -> udp_src_ports.find(value);
			if(it != udp_src_ports.end())
				this -> udp_src_ports[value] = this -> udp_src_ports[value] + 1;
			else
				this -> udp_src_ports[value] = 1;
		}
		break;

		case UDP_DEST_PORT_MAP:
		{
			map<unsigned short, int>::iterator it = this -> udp_dest_ports.find(value);
			if(it != udp_dest_ports.end())
				this -> udp_dest_ports[value] = this -> udp_dest_ports[value] + 1;
			else
				this -> udp_dest_ports[value] = 1;
		}
		break;
		
		case ICMP_TYPES_MAP:
		{
			map<unsigned short, int>::iterator it = this -> icmp_types.find(value);
			if(it != icmp_types.end())
				this->icmp_types[value]++;
			else
				this->icmp_types[value] = 1;
		}
		break;

		case ICMP_CODES_MAP:
		{
			map<unsigned short, int>::iterator it = this -> icmp_codes.find(value);
			if(it != icmp_codes.end())
				this->icmp_codes[value]++;
			else
				this->icmp_codes[value] = 1;
		}
		break;

		case TCP_OPTION_KIND:
		{
			map<unsigned short, int>::iterator it = this -> tcp_option_kind.find(value);
			if(it != tcp_option_kind.end())
				this->tcp_option_kind[value]++;
			else
				this->tcp_option_kind[value] = 1;
		}
		break;

		case NETWORK_PROTOCOLS:
		{
			map<unsigned short, int>::iterator it = this -> network_protocols.find(value);
			if(it != network_protocols.end())
				this->network_protocols[value]++;
			else
				this->network_protocols[value] = 1;
		}
		break;
		
		case TRANSPORT_PROTOCOLS:
		{
			map<unsigned short, int>::iterator it = this -> network_protocols.find(value);
			if(it != transport_protocols.end())
				this->transport_protocols[value]++;
			else
				this->transport_protocols[value] = 1;
		}
		break;
		
	}
}

void PacketInfoCollector::insertIntoMap(struct icmp_type_code value, MapType mapType)
{
	switch(mapType)
	{
		case ICMP_TYPES_CODES_MAP:
		{
			map<struct icmp_type_code, int>::iterator it = this -> icmp_types_codes.find(value);
			if(it != icmp_types_codes.end())
				this->icmp_types_codes[value]++;
			else
				this->icmp_types_codes[value] = 1;
		}
		break;
	}
}

void PacketInfoCollector::freeAll()
{
}

unsigned int PacketInfoCollector::minPacketSize = INITIAL_MIN_PACKET_SIZE;
unsigned int PacketInfoCollector::maxPacketSize = INITIAL_MAX_PACKET_SIZE;
unsigned int PacketInfoCollector::totalPacketSize = INITIAL_PACKET_SIZE;
unsigned int PacketInfoCollector::total_packet_count = INITIAL_PACKET_COUNT;
