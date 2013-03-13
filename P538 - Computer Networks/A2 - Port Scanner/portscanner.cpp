/* ==========================================================================
 Name        : portscanner.cpp
 Author      : Harsh Savla & Purshottam Vishwakarma 
 Version     : 1.0
 Description : Main Program. Starts the Portscanner
 Last modified : 10/17/2012
 ============================================================================
*/

#include "portscanner.h"
using namespace std;

map<Port_State, char*> port_state_map;	// maps enums to c-like strings
pthread_mutex_t port_mutex;		// mutex on above queue
pthread_mutex_t print_mutex;		// mutex on printing the output

char source_ip[20];
int LOCAL_PORT = 3490;
const char *scan_types[NUMBER_OF_SCAN_TYPES] = {"SYN","NULL","FIN","XMAS","ACK","PROTOCOL"};

/* different ways of receiving ip addresses*/
typedef enum Input_ip_address_source
{
	IP_INPUT,
	IP_PREFIX_INPUT,
	FILE_INPUT
}Input_ip_address_source;

/* maintains the command line arguments such as ports, scans, ip addresses */
struct command_line_args
{
	map<char*,queue<long>* >::iterator map_iterator;		// we associate a queue of ports to scan for each ip address
	vector<char*> ip_addresses;
	map<char*,queue<long>* >		ip_map;
	queue<long> global_ports;		// ports to scan
	vector<char*> scans;
	bool scan_type[NUMBER_OF_SCAN_TYPES];
	vector<short> protocol_range;
	int speedup;
};

command_line_args args;

void port_state_map_init()
{
	char* open 	  = (char*) malloc (sizeof(char) * (strlen("OPEN") + 1));
	char* closed 	  = (char*) malloc (sizeof(char) * (strlen("CLOSED") + 1));
	char* filtered 	  = (char*) malloc (sizeof(char) * (strlen("FILTERED") + 1));
	char* unfiltered =  (char*) malloc (sizeof(char) * (strlen("UNFILTERED") + 1));
	char* open_or_fil = (char*) malloc (sizeof(char) * (strlen("OPEN | FILTERED") + 1));

	strcpy(open,"OPEN");
	strcpy(closed,"CLOSED");
	strcpy(filtered,"FILTERED");
	strcpy(unfiltered,"UNFILTERED");
	strcpy(open_or_fil,"OPEN | FILTERED");

	port_state_map.insert(pair<Port_State, char*>(OPEN, open));
	port_state_map.insert(pair<Port_State, char*>(CLOSED, closed));
	port_state_map.insert(pair<Port_State, char*>(FILTERED, filtered));
	port_state_map.insert(pair<Port_State, char*>(UNFILTERED, unfiltered));
	port_state_map.insert(pair<Port_State, char*>(OPEN_OR_FILTERED, open_or_fil));
}

void print_ports();
void print_scans();
void recv_packet();

void init_protocol_range(char* optarg)
{
	char range_separator_delimiter[10] = "- ";
	char* end_ptr;
	char* tokenizer;
	int base_of_arg = 10;

	if(strchr(optarg, '-')!= NULL)
	{
		tokenizer = strtok(optarg, range_separator_delimiter);
		long start_range = strtol(tokenizer,&end_ptr, base_of_arg);
		tokenizer = strtok(NULL, range_separator_delimiter);
		long end_range = strtol(tokenizer, &end_ptr, base_of_arg);

		short s_range = (short) start_range;
		short e_range = (short) end_range;
		short i;

		for(i = s_range; i <= e_range; i++)
		{
			args.protocol_range.push_back(i);
		}
	}
	else 
	{
		strcpy(range_separator_delimiter, " ,");
		tokenizer = strtok(optarg, range_separator_delimiter);

		while(tokenizer != NULL)
		{
			long range = strtol(tokenizer, &end_ptr, base_of_arg);
			args.protocol_range.push_back((short)range);
			tokenizer = strtok(NULL,range_separator_delimiter);
		}
	}

	vector<short>::iterator it;

	/*for(it = args.protocol_range.begin(); it != args.protocol_range.end(); it++)
		cout << (*it) << endl;*/
}

void init_protocol_range()
{
	short int i;

	for(i = 1; i <= 255; i++)				// need to check whether to start from 0 or 1
		args.protocol_range.push_back(i);
}

void init_ports(vector<long> recvd_ports)
{
	vector<long>::iterator it;

	for(it = recvd_ports.begin(); it!= recvd_ports.end(); it++)
		args.global_ports.push(*it);
}

void init_ports(long start, long end)
{
	for(long i = start; i <= end; i++)
		args.global_ports.push(i);
}

void init_ports()
{
	for(long i = 1; i <= 1024; i++)
		args.global_ports.push(i);
}

void assign_ports(char *optarg)
{
	char port_separator_delimiter[10] = "-";
	char* end_ptr;
	char* tokenizer;
	int base_of_args = 10;
	vector<long> ports;

	if(strchr(optarg, '-')!= NULL)
	{
		tokenizer = strtok(optarg, port_separator_delimiter);
		long start_port = strtol(tokenizer,&end_ptr, base_of_args);
		tokenizer = strtok(NULL, port_separator_delimiter);
		long end_port = strtol(tokenizer, &end_ptr, base_of_args);		
		init_ports(start_port, end_port);
		return;
	}

	strcpy(port_separator_delimiter, "\t ,");
	tokenizer = strtok(optarg, port_separator_delimiter);

	while(tokenizer!= NULL)
	{
		long port = strtol(tokenizer, &end_ptr, base_of_args);
		ports.push_back(port);
		tokenizer = strtok(NULL, port_separator_delimiter);
	}

	init_ports(ports);
}

void getTime(char time_buffer[])
{
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime ( &rawtime );
	memset(time_buffer, '0', sizeof(time_buffer));
	strftime (time_buffer,80,"%m/%d/%y %H:%M:%S" ,timeinfo);
}

void print_usage()
{
	cout << endl;
	cout << "\tTo invoke the program, use: sudo ./ps <option1> <value1> ... <optionN> <valueN>" << endl;
	cout << "\tFor eg." << endl;
	cout << "\t\t\t\t\t--help" << endl;
	cout << "\t\t\t\t\t--ports '22, 80, 587, 43' or --port '1 - 100' or --ports '22 80 43 587'" << endl;
	cout << "\t\t\t\t\t--scans 'syn, ack, null'" << endl;
	cout << "\t\t\t\t\t--ip 'silo.cs.indiana.edu'" << endl;
	cout << "\t\t\t\t\t--prefix '192.168.1.250/30'" << endl;
	cout << "\t\t\t\t\t--file file.txt" << endl;
	cout << "\t\t\t\t\t--protocol-range '6,78,30' or  --protocol-range '10 - 50' or --protocol-range '10 34 56'" << endl;
	cout << "\t\t\t\t\t--speedup 5" << endl;
	exit(1);
}

void print_ports()
{
	queue<long> dup_queue(args.global_ports);

	while(!dup_queue.empty())
	{
		cout << dup_queue.front() << endl;
		dup_queue.pop();
	}
}

void init_scans()
{
	char* syn_scan = (char*) malloc (sizeof(char) * (strlen("SYN" + 1)));
	char* null_scan = (char*) malloc (sizeof(char) * (strlen("NULL" + 1)));
	char* fin_scan = (char*) malloc (sizeof(char) * (strlen("FIN" + 1)));
	char* xmas_scan = (char*) malloc (sizeof(char) * (strlen("XMAS" + 1)));
	char* ack_scan = (char*) malloc (sizeof(char) * (strlen("ACK" + 1)));
	char* protocol_scan = (char*) malloc (sizeof(char) * (strlen("PROTOCOL" + 1)));

	strcpy(syn_scan,"SYN");
	strcpy(null_scan,"NULL");
	strcpy(fin_scan,"FIN");
	strcpy(xmas_scan,"XMAS");
	strcpy(ack_scan,"ACK");
	strcpy(protocol_scan,"PROTOCOL");

	args.scans.push_back(syn_scan);
	args.scans.push_back(null_scan);
	args.scans.push_back(fin_scan);
	args.scans.push_back(xmas_scan);
	args.scans.push_back(ack_scan);
	args.scans.push_back(protocol_scan);

	for(int i=0;i< NUMBER_OF_SCAN_TYPES; i++)
		args.scan_type[i] = true;
}

void init_scans(char* optarg)
{
	char delimiter[] = " ,";

	if(strlen(optarg) == 0)
	{
		init_scans();
		return;
	}		

	char* tokenizer = strtok(optarg, delimiter);

	for(int i=0;i< NUMBER_OF_SCAN_TYPES; i++)
		args.scan_type[i] = false;

	while(tokenizer!= NULL)
	{
		int i;

		for(i = 0; tokenizer[i]!='\0'; i++)
			tokenizer[i] = toupper(tokenizer[i]);

		args.scans.push_back(tokenizer);

		if(!strcmp(tokenizer,"SYN"))
			args.scan_type[SCAN_TYPE_SYN] = true;
		else if(!strcmp(tokenizer,"NULL"))
			args.scan_type[SCAN_TYPE_NULL] = true;
		else if(!strcmp(tokenizer,"FIN"))
			args.scan_type[SCAN_TYPE_FIN] = true;
		else if(!strcmp(tokenizer,"XMAS"))
			args.scan_type[SCAN_TYPE_XMAS] = true;
		else if(!strcmp(tokenizer,"ACK"))
			args.scan_type[SCAN_TYPE_ACK] = true;
		else if(!strcmp(tokenizer,"PROTOCOL"))
			args.scan_type[SCAN_TYPE_PROTOCOL] = true;
		else
		{
			cout<<"Invalid scan type \""<<tokenizer<<"\" in the command line. Exiting...\n";
			exit(1);
		}
		tokenizer = strtok(NULL,delimiter);
	}
}

void print_scans()
{
	/* vector<char*>::iterator it;
	for(it = args.scans.begin(); it != args.scans.end(); it++)
		cout << (*it) << endl; */
		
	#ifdef _TRACE	
		for(int i = 0; i < NUMBER_OF_SCAN_TYPES; i++)
			if(args.scan_type[i])
				log.write(INFO, "scan type: %d", i);
	#endif
}

void assign_scans(char* optarg)
{
	init_scans(optarg);
	print_scans();
}

void assign_ip_addresses(char* optarg,Input_ip_address_source input_ip_source)
{
	if(input_ip_source == FILE_INPUT)
	{
		if(!access(optarg, F_OK))
		{
			string str;
			ifstream ifs(optarg);	

			while(ifs.good())
			{
				getline(ifs, str);

				if(str.length() > 6)
				{
					const char* ch = str.c_str();
					char* p = (char*) malloc(sizeof(char) * (strlen(ch) + 1));
					strcpy(p,ch);
					args.ip_addresses.push_back(p);
				}		
			}
			ifs.close();
		}
	}

	else if(input_ip_source == IP_INPUT)
	{
		args.ip_addresses.push_back(optarg);
	}	

	else if(input_ip_source == IP_PREFIX_INPUT)
	{
		// needs to be parsed	
		char *split,ipaddress[20];
		int prefix;
		
		if((split=strchr(optarg,'/'))!=NULL)
			prefix=atoi(split+1);
		else
		{
			cout<<"Error in command line IP prefix. Exiting..."<<endl;
			exit(1);
		}

		memset(ipaddress,'\0',20);
		strncpy(ipaddress,optarg,split-optarg);
		long int temp = 0xffffffff;
		int totalipaddress = 1;
		
		for(int i =32-prefix; i > 0; i--)
		{	
			temp = temp << 1;
			totalipaddress = 2 * totalipaddress;
		}
		
		struct in_addr ip;
		
		if(inet_aton(ipaddress,&ip) <= 0)
		{
			cout<<"Error in command line IP address with prefix. Exiting..."<<endl;
			exit(1);
		}
		long int ipaddr = ntohl(ip.s_addr);
		
		ipaddr = ipaddr & temp;
		char *ipaddress2;
		for(int i=0;i<totalipaddress;i++)
		{
			ipaddress2 = (char*) malloc(20);
			memset(ipaddress2,'\0',20);
			ip.s_addr=htonl(ipaddr);
			optarg = inet_ntoa(ip);
			strcpy(ipaddress2,optarg);
			args.ip_addresses.push_back(ipaddress2);
			#ifdef _TRACE
				log.write(DEBUG,"Pushing back IP %s",ipaddress2);
			#endif
			ipaddr++;
		}
	}
}

/* this function reads the command line arguments */
void fillOptions(int argc, char* argv[])
{
	int next_option;
	static const char *shortOptions = "h:p:i:x:f:s:n:r?";
	bool ports_available = false;
	bool scans_available = false;
	bool ip_available = false;
	bool help_requested = false;
	bool protocol_range_available = false;
	char* end_ptr;
	int base_of_arg = 10;

	static struct option long_options[] = {
		{"help",				no_argument,			NULL,			'h'},
		{"ports",               required_argument,      NULL,           'p'},
		{"ip",                  required_argument,      NULL,           'i'},
		{"prefix",              required_argument,      NULL,           'x'},
		{"file",                required_argument,      NULL,           'f'},
		{"speedup",             required_argument,      NULL,           's'},
		{"scans",               required_argument,      NULL,           'n'},
		{"protocol-range",      required_argument,      NULL,           'r'},
		{NULL,                  0,                      0,               0}
	};

	do {
		next_option = getopt_long(argc, argv, shortOptions,long_options, NULL);

		switch(next_option) {

			case 'h':
				help_requested = true;
				print_usage();
				break;

			case 'p':
				ports_available = true;
				assign_ports(optarg);
				break;

			case 'i':
				ip_available = true;
				assign_ip_addresses(optarg, IP_INPUT);
				break;

			case 'x':
				ip_available = true;
				assign_ip_addresses(optarg, IP_PREFIX_INPUT);
				break;

			case 'f':
				ip_available = true;
				assign_ip_addresses(optarg,FILE_INPUT);
				break;

			case 's':
				long speed_up;
				speed_up  = strtol(optarg, &end_ptr,base_of_arg);
				if(speed_up > 0)
					args.speedup = (int) speed_up;
				break;

			case 'n':
				scans_available = true;
				assign_scans(optarg);
				break;

			case 'r':
				protocol_range_available = true;
				init_protocol_range(optarg);
				break;
		}
	} while(next_option!=-1);

	if(!ip_available && !help_requested)
	{
		print_usage();
		cout << "IP not entered. Please enter ip/ip prefix/ file. Program will now exit" << endl;
		exit(1);
	}

	if(!ports_available)
		init_ports();

	if(!scans_available)
		init_scans();

	if(!protocol_range_available)
		init_protocol_range();
}

void global_init()
{
	port_state_map_init();
	args.speedup = 1;	
}

/*  compute TCP header checksum */
/* References: Unix Network Programming, Richard Stevens*/
uint16_t checksum_comp (uint16_t *addr, int len) 
{   
	// From http://sock-raw.org/papers/syn_scanner
	register long sum = 0;
	int count = len;
	uint16_t temp;

	while (count > 1)  {
		temp = htons(*addr++);  
		sum += temp;
		count -= 2;
	}

	/*  Add left-over byte, if any */
	if(count > 0)
		sum += *(unsigned char *)addr;

	/*  Fold 32-bit sum to 16 bits */
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	uint16_t checksum = ~sum;
	return checksum;
}

/*	References: binarytides.com */
int get_local_ip ( char * buffer)
{
	int sock = socket ( AF_INET, SOCK_DGRAM, 0);

	const char* dnsIp = DNSIP;
	int dns_port = 53;

	struct sockaddr_in serv;

	memset( &serv, 0, sizeof(serv) );
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr(dnsIp);
	serv.sin_port = htons( dns_port );

	int err = connect( sock , (const struct sockaddr*) &serv , sizeof(serv) );

	struct sockaddr_in name;
	socklen_t namelen = sizeof(name);
	err = getsockname(sock, (struct sockaddr*) &name, &namelen);

	const char *p = inet_ntop(AF_INET, &name.sin_addr, buffer, 20);
	close(sock);
}

void build_ip_header(struct iphdr* iph, struct addrinfo *dest_servinfo, u_int8_t transport_protocol)
{
	iph -> ihl = IP_HEADER_LENGTH;
	iph -> version = IP_VERSION_4;
	iph -> tos = 0;
	iph -> tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
	iph -> id = htons(IP_IDENTIFIER);
	iph -> frag_off = 0;
	iph -> ttl = IP_TIME_TO_LIVE;
	iph -> protocol = transport_protocol;
	iph -> check = 0;
	iph -> saddr = inet_addr ( source_ip );	
	iph -> daddr =(((sockaddr_in*) (dest_servinfo -> ai_addr)) -> sin_addr).s_addr;

	iph -> check = checksum_comp((unsigned short*)iph, sizeof (struct ip));
}

void build_tcp_header(struct tcphdr* tcph, int& source_port, int& destination_port, int& scan_type)
{
	tcph -> source = htons(source_port);
	tcph -> dest = htons(destination_port);

	tcph -> seq = htonl(rand());
	tcph -> ack_seq = 0;
	tcph -> doff = sizeof(struct tcphdr)/4;
	
	tcph -> fin = (scan_type == SCAN_TYPE_FIN || scan_type == SCAN_TYPE_XMAS)?1:0;
	tcph -> syn = (scan_type == SCAN_TYPE_SYN)?1:0;
	tcph -> ack = (scan_type == SCAN_TYPE_ACK)?1:0;
	tcph -> rst = 0;
	
	if(scan_type == SCAN_TYPE_XMAS)
	{
		tcph -> psh = 1;
		tcph -> urg = 1;
	}
	else
	{
		tcph -> psh = 0;
		tcph -> urg = 0;
	}

	tcph -> window = htons(TCP_WINDOW_SIZE);
	tcph -> check = 0;	
	tcph -> urg_ptr = 0;
}

void build_udp_header(struct udphdr* udph, int& source_port, int& destination_port)
{
	int datasize = 0;
	udph -> source	= htons(source_port);
	udph -> dest	= htons(destination_port);
	udph -> len		= htons(sizeof(struct udphdr) + datasize);
    udph -> check		= 0;
}

void build_pseudo_header_tcp(struct pseudo_hdr *phdr, struct tcphdr* tcph, struct iphdr *iph, u_int8_t transport_protocol)
{
	phdr->src = iph->saddr;
	phdr->dst = iph->daddr;
	phdr->mbz = 0;
	phdr->proto = transport_protocol;
	phdr->len = htons( sizeof(struct tcphdr) );
	memcpy(&(phdr->tcp) , tcph , sizeof (struct tcphdr));
	tcph -> check = htons(checksum_comp((unsigned short *)phdr, sizeof(struct pseudo_hdr)));		// Checksum
}
void build_pseudo_header_udp(struct pseudo_hdr_udp *phdr, struct udphdr* udph, struct iphdr *iph, u_int8_t transport_protocol)
{
	phdr->src = iph->saddr;
	phdr->dst = iph->daddr;
	phdr->mbz = 0;
	phdr->proto = transport_protocol;
	phdr->len = htons( sizeof(struct udphdr) );
	memcpy(&(phdr->udp) , udph , sizeof (struct udphdr));
	udph -> check = htons(checksum_comp((unsigned short *)phdr, sizeof(struct pseudo_hdr_udp)));		// Checksum
}

void udp_scan(char* destination_address,int destination_port, int threadid, vector<resulttype>& results,struct addrinfo& dest_hints					,struct addrinfo *dest_servinfo)
{
	int UDP_SEND_SOCKET;
	int ICMP_RECV_SOCKET;
	bool retry = true;
	int status;
	struct sockaddr saddr;
	socklen_t saddr_size = sizeof saddr;
	int data_size;
	struct iphdr* ipheader;
	struct sockaddr_in source_addr,dest_addr;
	unsigned short iphdrlen;
	char addr_buffer[ADDR_BUFFER];
	fd_set rfds;
	struct timeval tv,retry_start_time,retry_end_time;
	int retval, retry_count = 0;
	int flags;
	resulttype result = {"\0","\0","\0","\0","\0"};
	char state[20] = "\0",additional_info[50] = "\0";

	sprintf(result.protocol_type,"%d/UDP",destination_port);
	sprintf(result.scan_type,"UDP");

	if((UDP_SEND_SOCKET = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0)
	{
		cout << "couldn't create raw socket for TCP sender. Bad file descriptor" << endl;	
		exit(1);
	}

	if((ICMP_RECV_SOCKET = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("couldn't create raw socket for ICMP receiver. Bad file descriptor\n");
		exit(1);
	}

	  /* initialize random seed: */
	srand ( time(NULL) );

	/* construction of packet */

	char datagram[ PCKT_LEN];

	struct iphdr* iph = (struct iphdr*) datagram;
	struct udphdr* udph = (struct udphdr*) (datagram + sizeof(struct ip));
	struct pseudo_hdr_udp phdr;

	memset(datagram, 0, PCKT_LEN);

	// build the ip header
	build_ip_header(iph, dest_servinfo, IPPROTO_UDP);

	// Create the UDP Header
	build_udp_header(udph, LOCAL_PORT,destination_port);

	// Create the UDP Pseudo Header
	build_pseudo_header_udp(&phdr, udph, iph, IPPROTO_UDP);

	//IP_HDRINCL to tell the kernel that headers are included in the packet
	int one = 1;
	const int *val = &one;
	
	if (setsockopt (UDP_SEND_SOCKET, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
	{
		printf ("Error setting IP_HDRINCL. Error number : %d . Error message : %s \n" , errno , strerror(errno));
		exit(0);
	}

	char* icmp_recv_buffer = (char*) malloc (RECV_BUFFER);
	
	while(retry)
	{
		// Start the timer
		gettimeofday(&retry_start_time, NULL);

		if(sendto(UDP_SEND_SOCKET,datagram, sizeof(struct ip) + sizeof(struct udphdr),0,(struct sockaddr*)(dest_servinfo -> ai_addr), dest_servinfo -> ai_addrlen ) < 0) {
			cout << "UDP unsuccessful send" << endl;
		}

		memset(&saddr,0, sizeof saddr);
		memset(icmp_recv_buffer,0,sizeof icmp_recv_buffer);

		flags = fcntl(ICMP_RECV_SOCKET, F_GETFL);
		flags |= O_NONBLOCK;	
		fcntl(ICMP_RECV_SOCKET,F_SETFL, flags);

		while(true)
		{
			tv.tv_sec = RETRY_TIME_INTERVAL;	// Let select timeout be equal to the retry time interval
			tv.tv_usec = 0;

			FD_ZERO(&rfds);
			FD_SET(ICMP_RECV_SOCKET, &rfds);

			retval = select(ICMP_RECV_SOCKET + 1, &rfds, NULL, NULL, &tv);
			if(retval == -1)
			{
				log.write(ERROR,"Thread %d: UDP Scan. Error in select icmp sd = %d udp sd = %d",threadid,ICMP_RECV_SOCKET,UDP_SEND_SOCKET);
				perror("select");
				retry = false;
				break;
			}
			else if (retval)
			{
				// printf("We have data\n");
			}else
			{
				#ifdef _TRACE
					log.write(DEBUG,"Thread %d: Time Out in select",threadid);
				#endif
			}
			if(FD_ISSET(ICMP_RECV_SOCKET, &rfds))
			{
				if((data_size = recvfrom(ICMP_RECV_SOCKET,icmp_recv_buffer, RECV_BUFFER, 0, &saddr, &saddr_size)) < 0)
					log.write(INFO,"Thread %d: Failed to read icmp data",threadid);
				else
				{
					ipheader = (struct iphdr*) icmp_recv_buffer;
					struct icmphdr *icmp;
					icmp = (struct icmphdr *)(icmp_recv_buffer + (ipheader->ihl)*4);
					struct iphdr* original_ipheader = (struct iphdr*) ((icmp_recv_buffer + (ipheader->ihl)*4) + sizeof(struct icmphdr));
					struct udphdr *original_udpheader=(struct udphdr*)((icmp_recv_buffer + (ipheader->ihl)*4) + sizeof(struct icmphdr) + (original_ipheader->ihl)*4);

					memset(&dest_addr, 0, sizeof(dest_addr));
					dest_addr.sin_addr.s_addr = original_ipheader->daddr;

					#ifdef _TRACE
						inet_ntop(AF_INET, &ipheader->saddr, addr_buffer, ADDR_BUFFER);
						log.write(DEBUG,"Thread %d: Received ICMP Packet - IPv%d: hdr-size=%d pkt-size=%d protocol=%d TTL=%d src=%s ",threadid,ipheader->version, ipheader->ihl*4, ntohs(ipheader->tot_len), ipheader->protocol,ipheader->ttl, addr_buffer);
						inet_ntop(AF_INET, &ipheader->daddr, addr_buffer, ADDR_BUFFER);
						log.write(DEBUG,"dst=%s", addr_buffer);
						log.write(DEBUG,"Thread %d: ICMP: type[%d/%d] checksum[%d] id[%d] seq[%d]",threadid,icmp->type, icmp->code, ntohs(icmp->checksum),icmp->un.echo.id, icmp->un.echo.sequence);
					#endif
					// 	Verify if the ICMP packet is in response to us. i.e. Payload of ICMP should be same as our original sent //	packet
					if(dest_addr.sin_addr.s_addr == (((sockaddr_in*)(dest_servinfo->ai_addr))->sin_addr).s_addr && ntohs(original_udpheader->dest) == destination_port && ntohs(original_udpheader->source) == LOCAL_PORT)
					{
						if(icmp->type == ICMP_TYPE_3 && 
							(icmp->code == ICMP_HOST_UNREACH || 
							icmp->code == ICMP_PROT_UNREACH ||
							icmp->code == ICMP_PORT_UNREACH ||
							icmp->code == ICMP_NET_ANO ||
							icmp->code == ICMP_HOST_ANO ||
							icmp->code == ICMP_PKT_FILTERED	))
						{
							#ifdef _TRACE
								log.write(INFO,"Thread %d: UDP ICMP code = %d\tPort %d Filtered",threadid,icmp->code,destination_port);
							#endif
							strcpy(state,port_state_map[FILTERED]);
							strcpy(additional_info,"Received ICMP response");
							fflush(stdout);
							retry = false;
							break;
						}
					}
				}
			}
			if(retry)
			{	// End the timer
				gettimeofday(&retry_end_time, NULL);
				if(retry_end_time.tv_sec  - retry_start_time.tv_sec >= RETRY_TIME_INTERVAL)
				{
					retry_count++;
					break;
				}
			}
		}	// end of while(true)
		if(retry_count >= NUMBER_OF_RETRY)
		{
			retry = false;
			// If no response is received after several retransmissions, the port should be marked as filtered.
			#ifdef _TRACE
				log.write(INFO,"Thread %d: UDP TIMEOUT\tPort %d Open | Filtered",threadid,destination_port);
			#endif
			strcpy(state,port_state_map[OPEN_OR_FILTERED]);
			strcpy(additional_info,"Time out");
		}
	}	// end of while(retry)

	close(UDP_SEND_SOCKET);
	close(ICMP_RECV_SOCKET);

	free(icmp_recv_buffer);

	strcpy(result.state,state);
	strcpy(result.additional_info,additional_info);
	results.push_back(result);
}

/*
		The scan consists of sending a packet to the target machine with a certain protocol type . 
		If the protocol is not active, the target host will respond with a packet icmp -type Destination protocol unreachable (type 3, code 2). 
		If the protocol is active however you will not receive any response.
	*/
void protocol_scan(char* destination_address,int protocol,int& threadid, vector<resulttype>& results,struct addrinfo& 						dest_hints,struct addrinfo *dest_servinfo)
{
	int SEND_SOCKET;
	int ICMP_RECV_SOCKET;
	int OTHER_RECV_SOCKET;
	bool retry = true;
	int status;
	struct sockaddr saddr;
	socklen_t saddr_size = sizeof saddr;
	int data_size;
	struct iphdr* ipheader;
	struct sockaddr_in source_addr,dest_addr;
	unsigned short iphdrlen;
	char addr_buffer[ADDR_BUFFER],	addr_buffer2[ADDR_BUFFER];;
	fd_set rfds;
	struct timeval tv,retry_start_time,retry_end_time,retry_current_time;
	int retval, retry_count = 0, destination_port = RANDOM_DESTINATION_PORT;
	int flags;
	resulttype result = {"\0","\0","\0","\0","\0"};
	char state[20] = "\0",additional_info[50] = "\0";

	sprintf(result.protocol_type,"Protocol/%d",protocol);
	sprintf(result.scan_type,"Protocol");

	if((SEND_SOCKET = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("couldn't create raw socket for TCP sender. Bad file descriptor : ");	
		exit(1);
	}

	if(protocol == IPPROTO_TCP && (OTHER_RECV_SOCKET = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
	{
		perror("couldn't create raw socket for ICMP receiver. Bad file descriptor\n");
		exit(1);
	}
	else if(protocol == IPPROTO_UDP && (OTHER_RECV_SOCKET = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0)
	{
		perror("couldn't create raw socket for ICMP receiver. Bad file descriptor\n");
		exit(1);
	}
	else if(protocol == IPPROTO_IGMP && (OTHER_RECV_SOCKET = socket(AF_INET, SOCK_RAW, IPPROTO_IGMP)) < 0)
	{
		perror("couldn't create raw socket for ICMP receiver. Bad file descriptor\n");
		exit(1);
	}

	if((ICMP_RECV_SOCKET = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("couldn't create raw socket for ICMP receiver. Bad file descriptor\n");
		exit(1);
	}

	  /* initialize random seed: */
	srand ( time(NULL) );

	/* construction of packet */

	char datagram[ PCKT_LEN];
	struct iphdr* iph = (struct iphdr*) datagram;
	memset(datagram, '\0', PCKT_LEN);

	// build the ip header
	build_ip_header(iph, dest_servinfo, protocol);

	//IP_HDRINCL to tell the kernel that headers are included in the packet
	int one = 1;
	const int *val = &one;
	
	if (setsockopt (SEND_SOCKET, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
	{
		printf ("Error setting IP_HDRINCL. Error number : %d . Error message : %s \n" , errno , strerror(errno));
		exit(0);
	}

	char* icmp_recv_buffer = (char*) malloc (RECV_BUFFER);
	unsigned char* recv_buffer = (unsigned char*) malloc (RECV_BUFFER);	

	if(protocol == IPPROTO_IGMP || protocol == IPPROTO_TCP || protocol == IPPROTO_UDP)
	{
		if(protocol == IPPROTO_TCP)
		{
			struct tcphdr* tcph = (struct tcphdr*) (datagram + sizeof(struct ip));
			struct pseudo_hdr phdr;
			int scan_type = SCAN_TYPE_SYN;
			build_tcp_header(tcph, LOCAL_PORT,destination_port,scan_type);	// Create the TCP Header
			build_pseudo_header_tcp(&phdr, tcph, iph, IPPROTO_TCP);			// Create the TCP Pseudo Header
		}
		else if (protocol == IPPROTO_UDP)
		{
			struct udphdr* udph = (struct udphdr*) (datagram + sizeof(struct ip));
			struct pseudo_hdr_udp phdr;
			build_udp_header(udph, LOCAL_PORT,destination_port);			// Create the UDP Header
			build_pseudo_header_udp(&phdr, udph, iph, IPPROTO_UDP);			// Create the UDP Pseudo Header
		}
	}
	while(retry)
	{
		// Start the timer
		gettimeofday(&retry_start_time, NULL);

		if(sendto(SEND_SOCKET,datagram, sizeof(struct ip),0,(struct sockaddr*)(dest_servinfo -> ai_addr), 
					dest_servinfo -> ai_addrlen ) < 0) {
			cout << "Protocol Scan : Unsuccessful send"<< endl;
		}

		memset(&saddr,0, sizeof saddr);
		memset(icmp_recv_buffer,0,sizeof icmp_recv_buffer);

		flags = fcntl(ICMP_RECV_SOCKET, F_GETFL);
		flags |= O_NONBLOCK;	
		fcntl(ICMP_RECV_SOCKET,F_SETFL, flags);

		if(protocol == IPPROTO_IGMP || protocol == IPPROTO_TCP || protocol == IPPROTO_UDP)
		{
			flags = fcntl(OTHER_RECV_SOCKET, F_GETFL);
			flags |= O_NONBLOCK;	
			fcntl(OTHER_RECV_SOCKET,F_SETFL, flags);
			memset(recv_buffer, 0, sizeof recv_buffer);
		}

		while(true)
		{
			gettimeofday(&retry_current_time, NULL);

			tv.tv_sec = RETRY_TIME_INTERVAL;	// Let select timeout be equal to the retry time interval
			tv.tv_usec = 0;

			FD_ZERO(&rfds);
			if(protocol == IPPROTO_IGMP || protocol == IPPROTO_TCP || protocol == IPPROTO_UDP)
			{
				FD_SET(OTHER_RECV_SOCKET, &rfds);
			}
			FD_SET(ICMP_RECV_SOCKET, &rfds);

			retval = select(ICMP_RECV_SOCKET + 1, &rfds, NULL, NULL, &tv);
			if(retval == -1)
			{
				log.write(ERROR,"Thread %d: Protocol Scan. Error in select",threadid);
				perror("select");
				retry = false;
				break;
			}
			else if (retval)
			{
				// printf("We have data\n");
			}else
			{
				#ifdef _TRACE
					log.write(DEBUG,"Thread %d: Time Out in select",threadid);
				#endif
			}
			if(FD_ISSET(ICMP_RECV_SOCKET, &rfds))
			{
				if((data_size = recvfrom(ICMP_RECV_SOCKET,icmp_recv_buffer, RECV_BUFFER, 0, &saddr, &saddr_size)) < 0)
					log.write(INFO,"Thread %d: Failed to read icmp data",threadid);
				else
				{
					ipheader = (struct iphdr*) icmp_recv_buffer;
					struct icmphdr *icmp;
					icmp = (struct icmphdr *)(icmp_recv_buffer + (ipheader->ihl)*4);
					struct iphdr* original_ipheader = (struct iphdr*) ((icmp_recv_buffer + (ipheader->ihl)*4) + sizeof(struct icmphdr));

					memset(&source_addr, 0, sizeof(source_addr));
					source_addr.sin_addr.s_addr = original_ipheader->saddr;

					memset(&dest_addr, 0, sizeof(dest_addr));
					dest_addr.sin_addr.s_addr = original_ipheader->daddr;

					#ifdef _TRACE
						inet_ntop(AF_INET, &ipheader->saddr, addr_buffer, ADDR_BUFFER);
						log.write(DEBUG,"Thread %d: Received ICMP Packet - IPv%d: hdr-size=%d pkt-size=%d protocol=%d TTL=%d 	src=%s ",threadid,ipheader->version, ipheader->ihl*4, ntohs(ipheader->tot_len), ipheader->protocol,
									ipheader->ttl, addr_buffer);
						inet_ntop(AF_INET, &ipheader->daddr, addr_buffer, ADDR_BUFFER);
						log.write(DEBUG,"dst=%s", addr_buffer);
						log.write(DEBUG,"Thread %d: ICMP: type[%d/%d] checksum[%d] id[%d] seq[%d]",threadid,icmp->type, icmp->code,				 ntohs(icmp->checksum),icmp->un.echo.id, icmp->un.echo.sequence);
					#endif
					// Verify if the ICMP packet is in response to us. i.e. Payload of ICMP should be same as our original sent packet
					if(dest_addr.sin_addr.s_addr == (((sockaddr_in*)(dest_servinfo->ai_addr))->sin_addr).s_addr && iph->saddr == 			original_ipheader->saddr)
					{
						if(icmp->type == ICMP_TYPE_3 &&  icmp->code == ICMP_PROT_UNREACH)
						{
							#ifdef _TRACE
								log.write(INFO,"Thread %d: Protocol %d Closed",threadid,protocol);
							#endif
							strcpy(state,port_state_map[CLOSED]);
							sprintf(additional_info,"Received ICMP type = %d code = %d",icmp->type,icmp->code);
							fflush(stdout);
							retry = false;
							break;
						}
					}
				}
			}
			if(FD_ISSET(OTHER_RECV_SOCKET, &rfds))
			{
				if((data_size = recvfrom(OTHER_RECV_SOCKET,recv_buffer, RECV_BUFFER, 0, &saddr, &saddr_size)) < 0)
					log.write(INFO, "Protocol scan: failed to get any data");
				else
				{
					ipheader = (struct iphdr*) recv_buffer;

					inet_ntop(AF_INET, &ipheader->saddr, addr_buffer, ADDR_BUFFER);
					inet_ntop(AF_INET, &ipheader->daddr, addr_buffer2, ADDR_BUFFER);
					iphdrlen = ipheader->ihl*4;

					memset(&source_addr, 0, sizeof(source_addr));
					source_addr.sin_addr.s_addr = ipheader->saddr;
				
					memset(&dest_addr, 0, sizeof(dest_addr));
					dest_addr.sin_addr.s_addr = ipheader->daddr;

					if(ipheader->protocol == IPPROTO_TCP || ipheader->protocol == IPPROTO_IGMP || ipheader->protocol == 			IPPROTO_UDP)
					{
						#ifdef _TRACE
							log.write(DEBUG,"Thread %d:Receiving packet from %s with destination %s with protocol : %d",
												threadid,addr_buffer,addr_buffer2,ipheader->protocol);
						#endif
						
						if(source_addr.sin_addr.s_addr == (((sockaddr_in*)(dest_servinfo->ai_addr))->sin_addr).s_addr)
						{
							#ifdef _TRACE
								log.write(INFO,"Thread %d: Protocol %d Open",threadid,protocol);
							#endif
							strcpy(state,port_state_map[OPEN]);
							sprintf(additional_info,"Received Response for protocol %d",ipheader->protocol);
							retry = false;
							break;
						}
					}
				}
			}
			if(retry)
			{	// End the timer
				gettimeofday(&retry_end_time, NULL);
				if(retry_end_time.tv_sec  - retry_start_time.tv_sec >= RETRY_TIME_INTERVAL)
				{
					retry_count++;
					break;
				}
			}
		}	// end of while(true)
		if(retry_count >= NUMBER_OF_RETRY_PROTOCOL_SCAN)
		{
			retry = false;
			#ifdef _TRACE
				log.write(INFO,"Thread %d: Protocol %d Open",threadid,protocol);
			#endif
			strcpy(state,port_state_map[OPEN]);
			sprintf(additional_info,"Time out");
		}
	}	// end of while(retry)

	close(SEND_SOCKET);
	close(ICMP_RECV_SOCKET);
	close(OTHER_RECV_SOCKET);

	free(icmp_recv_buffer);
	free(recv_buffer);

	strcpy(result.state,state);
	strcpy(result.additional_info,additional_info);
	results.push_back(result);
}

void get_service_response_message(struct addrinfo* dest_servinfo, char* find_service_message, char* get_service_info_buffer)
{
	int find_service_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
								
	if(find_service_socket > 0) 
	{					
		if (connect(find_service_socket, dest_servinfo -> ai_addr, dest_servinfo -> ai_addrlen) == 0)
		{
			send(find_service_socket, find_service_message, strlen(find_service_message), 0);									
			int service_finder_flags = fcntl(find_service_socket, F_GETFL);
			service_finder_flags |= O_NONBLOCK;	
			fcntl(find_service_socket,F_SETFL, service_finder_flags);
									
			fd_set find_service_read_set;								
			FD_ZERO(&find_service_read_set);
			FD_SET(find_service_socket, &find_service_read_set);
									
			struct timeval find_service_timeout;
							
			find_service_timeout.tv_sec = SERVICE_RETRY_TIME_INTERVAL;
			find_service_timeout.tv_usec = 0;
										
			int service_ret_val = select(find_service_socket + 1, &find_service_read_set, NULL, NULL, &	find_service_timeout);
									
			if(FD_ISSET(find_service_socket, &find_service_read_set))
			{
				int service_bytes;
				#ifndef SERVICE_BUFFER_SIZE
					#define SERVICE_BUFFER_SIZE 3000
				#endif
										
				if((service_bytes = recv(find_service_socket, get_service_info_buffer, SERVICE_BUFFER_SIZE,0)) 		> 0)
				{
					get_service_info_buffer[service_bytes] = '\0';
				}
			}											
		}
		else 
		{
			#ifdef _TRACE
				log.write(DEBUG, "UNABLE TO CONNECT");
			#endif
		}
	}
}

void check_http_service(struct addrinfo* dest_servinfo, char *service_version)
{
	char http_request_buffer[200] = "GET / \r\n";
	char get_service_info_buffer[SERVICE_BUFFER_SIZE];
	memset(get_service_info_buffer, '\0',SERVICE_BUFFER_SIZE);
	get_service_response_message(dest_servinfo, http_request_buffer, get_service_info_buffer);
	
	if(strlen(get_service_info_buffer) > 0)
	{
		strcpy(service_version,"HTTP");		
		
		char* http_ptr;
		
		if((http_ptr = strstr(get_service_info_buffer,"HTTP")) != NULL)
		{
			char actual_service_version[10] = "";
			
			strncpy(actual_service_version,http_ptr + strlen("HTTP") + 1, 3);
			strcat(service_version, " ");
			strcat(service_version, actual_service_version);
		}
		#ifdef _TRACE
			log.write(INFO,"running service: %s", service_version);
		#endif
	}
	else
	{
		strcpy(service_version, "HTTP");
	}
}

void check_ssh_service(struct addrinfo* dest_servinfo, char *service_version)
{
	char ssh_request_buffer[200] = "ssh -v";
	char get_service_info_buffer[SERVICE_BUFFER_SIZE];
	memset(get_service_info_buffer, '\0',SERVICE_BUFFER_SIZE);
	get_service_response_message(dest_servinfo, ssh_request_buffer, get_service_info_buffer);
	
	if(strlen(get_service_info_buffer) > 0)
	{
		#ifdef _TRACE
			log.write(INFO,get_service_info_buffer);		// for debugging
		#endif
	
		int i;
		
		for(i = 0; get_service_info_buffer[i] != '\0'; i++)
			get_service_info_buffer[i] = toupper(get_service_info_buffer[i]);
		
		char *ptr1, *ptr2;
		
		if(strstr(get_service_info_buffer, "OPENSSH_") != NULL)
		{	
			ptr1 = strstr(get_service_info_buffer, "OPENSSH_");
			if(ptr1 != NULL)
			{
				strncpy(service_version, ptr1, strlen("OPENSSH_") + 3);
				service_version[strlen("OPENSSH_") + 4] = '\0';
				#ifdef _TRACE
					log.write(INFO,"ssh version:%s",service_version);
				#endif
			}
		}
	}
	else
	{
		strcpy(service_version, "UNKNOWN");
	}
}

void check_imap_service(struct addrinfo* dest_servinfo, char *service_version)
{	
	char imap_request_buffer[200] = "fetch 1\r\n";
	char get_service_info_buffer[SERVICE_BUFFER_SIZE];
	memset(get_service_info_buffer, '\0',SERVICE_BUFFER_SIZE);
	get_service_response_message(dest_servinfo, imap_request_buffer, get_service_info_buffer);
	
	if(strlen(get_service_info_buffer) > 0)
	{
		#ifdef _TRACE
			log.write(INFO,"repsonse from server for imap request: %s",get_service_info_buffer);		
		#endif
		strcpy(service_version, "IMAP");
	}
	else
	{
		#ifdef _TRACE
			log.write(INFO,"repsonse from server for imap request: empty buffer!!");		// for debugging
		#endif
		strcpy(service_version, "UNKNOWN");
	}
}

void check_smtp_service(struct addrinfo* dest_servinfo,char *service_version)
{
	char smtp_request_buffer[200] = "";
	char get_service_info_buffer[SERVICE_BUFFER_SIZE];
	memset(get_service_info_buffer, '\0',SERVICE_BUFFER_SIZE);
	get_service_response_message(dest_servinfo, smtp_request_buffer, get_service_info_buffer);

	if(strlen(get_service_info_buffer) > 0)
	{
		char* tokenizer = strtok(get_service_info_buffer, " ");
		
		if(tokenizer != NULL)
		{
			tokenizer = strtok(NULL, " ");
			if(tokenizer != NULL)
			{
				tokenizer = strtok(NULL, " ");
				if(tokenizer != NULL)
				{
					strcat(service_version, tokenizer);
					strcat(service_version, " ");
					tokenizer = strtok(NULL, " ");
					if(tokenizer != NULL)
					{
						strcat(service_version, tokenizer);
						#ifdef _TRACE
							log.write(INFO,"repsonse from server for smtp request: %s",service_version);
						#endif
					}
				}
			}
		}
	}
	else
	{
		strcpy(service_version, "UNKNOWN");
	}
}

void check_whois_service(struct addrinfo* dest_servinfo, char *service_version)
{
	char whois_request_buffer[200] = "\r\n";
	char get_service_info_buffer[SERVICE_BUFFER_SIZE];
	memset(get_service_info_buffer, '\0',SERVICE_BUFFER_SIZE);
	get_service_response_message(dest_servinfo, whois_request_buffer, get_service_info_buffer);
	
	if(strlen(get_service_info_buffer) > 0)
	{		
		strcpy(service_version,"WHOIS ");
		char* ptr = strstr(get_service_info_buffer, "Version");
		
		if(ptr != NULL)
		{
			char* ptr2 = ptr + strlen("Version") + 1;
			strncat(service_version, ptr2, 4);
			#ifdef _TRACE
				log.write(INFO,"repsonse from server for whois request: %s",service_version);	//for debugging
			#endif
		}
	}
	else
	{
		strcpy(service_version, "UNKNOWN");
	}
}

void check_pop3_service(struct addrinfo* dest_servinfo, char *service_version)
{
	char pop_request_buffer[200] = "\r\n";
	char get_service_info_buffer[SERVICE_BUFFER_SIZE];
	memset(get_service_info_buffer, '\0',SERVICE_BUFFER_SIZE);
	get_service_response_message(dest_servinfo, pop_request_buffer, get_service_info_buffer);

	if(strlen(get_service_info_buffer) > 0)
	{
		if(strstr(get_service_info_buffer, "+OK") != NULL)
			strcpy(service_version, "POP3");
			
		#ifdef _TRACE
			log.write(INFO,"repsonse from server for pop request: %s",get_service_info_buffer);		// for debugging
		#endif
	}
	else
	{
		strcpy(service_version, "UNKNOWN");
	}
}

/* this functions checks for running services */
void check_for_all_services(struct addrinfo *dest_servinfo,int destination_port, char* service_version, resulttype& result)
{
	if(destination_port == 22)
	{
		check_ssh_service(dest_servinfo, service_version);
	} 
	
	if(destination_port == 80)
	{
		check_http_service(dest_servinfo, service_version);
	} 
	
	if(destination_port == 25 || destination_port == 587)
	{
		check_smtp_service(dest_servinfo, service_version);
	} 
	
	if(destination_port == 43)
	{
		check_whois_service(dest_servinfo, service_version);
	} 
	
	if(destination_port == 110)
	{
		check_pop3_service(dest_servinfo, service_version);
	}
	
	if(destination_port == 143 || destination_port == 993)
	{
		check_imap_service(dest_servinfo, service_version);
	}
	sprintf(result.service,"%s",service_version);
	fflush(stdout);
}

/* thread handler */
void tcp_scan(char* destination_address,int destination_port, int threadid, int scan_type, vector<resulttype>& results,struct 				addrinfo& dest_hints,struct addrinfo *dest_servinfo)
{
	int status;
	struct sockaddr saddr;
	socklen_t saddr_size = sizeof saddr;
	int data_size;
	int TCP_SEND_SOCKET;
	int TCP_RECV_SOCKET;
	int ICMP_RECV_SOCKET;
	char service_version[50] = "\0";
	bool retry = true;
	struct iphdr* ipheader;
	struct sockaddr_in source_addr,dest_addr;
	unsigned short iphdrlen;
	char addr_buffer[ADDR_BUFFER];
	fd_set rfds;
	struct timeval tv,retry_start_time,retry_end_time;
	int retval, retry_count = 0;
	int flags;
	resulttype result = {"\0","\0","\0","\0","\0"};
	char state[20] = "\0",additional_info[50] = "\0";

	memset(service_version,'\0', sizeof service_version);
	sprintf(result.protocol_type,"%d/TCP",destination_port);
	sprintf(result.scan_type,"%s",scan_types[scan_type]);
	
	if((TCP_RECV_SOCKET = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
	{
		perror("couldn't create raw socket for TCP receiver. Bad file descriptor\n");
		exit(1);
	}

	if((TCP_SEND_SOCKET = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
	{
		cout << "couldn't create raw socket for TCP sender. Bad file descriptor" << endl;	
		exit(1);
	}

	if((ICMP_RECV_SOCKET = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("couldn't create raw socket for ICMP receiver. Bad file descriptor\n");
		exit(1);
	}
	  /* initialize random seed: */
	srand ( time(NULL) );

	/* construction of packet */

	char datagram[ PCKT_LEN];

	struct iphdr* iph = (struct iphdr*) datagram;
	struct tcphdr* tcph = (struct tcphdr*) (datagram + sizeof(struct ip));
	struct pseudo_hdr phdr;

	memset(datagram, 0, PCKT_LEN);

	// Create the IP header
	build_ip_header(iph, dest_servinfo, IPPROTO_TCP);

	// Create the TCP Header
	build_tcp_header(tcph, LOCAL_PORT,destination_port,scan_type);

	// Create the TCP Pseudo Header
	build_pseudo_header_tcp(&phdr, tcph, iph, IPPROTO_TCP);

	//IP_HDRINCL to tell the kernel that headers are included in the packet
	int one = 1;
	const int *val = &one;
	
	if (setsockopt (TCP_SEND_SOCKET, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
	{
		printf ("Error setting IP_HDRINCL. Error number : %d . Error message : %s \n" , errno , strerror(errno));
		exit(0);
	}

	unsigned char* recv_buffer = (unsigned char*) malloc (RECV_BUFFER);
	char* icmp_recv_buffer = (char*) malloc (RECV_BUFFER);

	while(retry)
	{
		// Start the timer
		gettimeofday(&retry_start_time, NULL);

		if(sendto(TCP_SEND_SOCKET,datagram, sizeof(struct ip) + sizeof(struct tcphdr),0,(struct sockaddr*)(dest_servinfo -> ai_addr), dest_servinfo -> ai_addrlen ) < 0) {
				log.write(ERROR,"Thread %d: Unsuccessful send",threadid);
				perror("Unsuccessful send");
		}

		memset(&saddr,0, sizeof saddr);
		memset(recv_buffer, 0, sizeof recv_buffer);
		memset(icmp_recv_buffer,0,sizeof icmp_recv_buffer);

		flags = fcntl(TCP_RECV_SOCKET, F_GETFL);
		flags |= O_NONBLOCK;	
		fcntl(TCP_RECV_SOCKET,F_SETFL, flags);

		flags = fcntl(ICMP_RECV_SOCKET, F_GETFL);
		flags |= O_NONBLOCK;	
		fcntl(ICMP_RECV_SOCKET, F_SETFL, flags);
		while(true)
		{
			tv.tv_sec = RETRY_TIME_INTERVAL;	// Let select timeout be equal to the retry time interval
			tv.tv_usec = 0;

			FD_ZERO(&rfds);
			FD_SET(TCP_RECV_SOCKET, &rfds);
			FD_SET(ICMP_RECV_SOCKET, &rfds);

			retval = select(ICMP_RECV_SOCKET + 1, &rfds, NULL, NULL, &tv);
			if(retval == -1)
			{
				log.write(ERROR,"Thread %d: TCP Scan. Error in select",threadid);
				perror("select");
				retry = false;
				break;
			}
			else if (retval)
			{
				// printf("We have data\n");
			}else
			{
				#ifdef _TRACE
					log.write(DEBUG,"Thread %d: Time Out in select",threadid);
				#endif
			}
			if(FD_ISSET(TCP_RECV_SOCKET, &rfds))
			{
				if((data_size = recvfrom(TCP_RECV_SOCKET,recv_buffer, RECV_BUFFER, 0, &saddr, &saddr_size)) < 0)
					cout << "failed to get any data" << endl;
				else
				{
					ipheader = (struct iphdr*) recv_buffer;

					inet_ntop(AF_INET, &ipheader->saddr, addr_buffer, ADDR_BUFFER);
					#ifdef _TRACE
						log.write(DEBUG,"Thread %d:Receiving packet from %s with protocol : %d",threadid,addr_buffer,
									ipheader->protocol);
					#endif
					if(ipheader->protocol == IPPROTO_TCP)
					{
						iphdrlen = ipheader->ihl*4;
					
						struct tcphdr *tcpheader=(struct tcphdr*)(recv_buffer + iphdrlen);
							
						memset(&source_addr, 0, sizeof(source_addr));
						source_addr.sin_addr.s_addr = ipheader->saddr;
					
						memset(&dest_addr, 0, sizeof(dest_addr));
						dest_addr.sin_addr.s_addr = ipheader->daddr;
						if(scan_type == SCAN_TYPE_SYN)
						{
							//  since a rare feature of TCP can sometimes just send the SYN packet in response to a SYN, mark the port as open in that case as well.
							if(tcpheader->syn == 1 && source_addr.sin_addr.s_addr == (((sockaddr_in*)(dest_servinfo->ai_addr))->sin_addr).s_addr && ntohs(tcpheader->source) == destination_port)
							{
								#ifdef _TRACE
									log.write(INFO,"Thread %d: TCP\t%s\tPort %d Open",threadid,scan_types[scan_type],ntohs(tcpheader->source));
								#endif

								check_for_all_services(dest_servinfo, destination_port, service_version, result);
								strcpy(state,port_state_map[OPEN]);
								strcpy(additional_info,"Received SYN");
								retry = false;
								break;
							}
							else if(tcpheader->rst == 1 && tcpheader->ack == 1 && source_addr.sin_addr.s_addr == (((sockaddr_in*)(dest_servinfo->ai_addr))->sin_addr).s_addr  && ntohs(tcpheader->source) == destination_port)
							{
								#ifdef _TRACE
									log.write(INFO,"Thread %d: TCP\t%s\tPort %d Closed",threadid,scan_types[scan_type],ntohs(tcpheader->source));
 								#endif
								strcpy(state,port_state_map[CLOSED]);
								strcpy(additional_info,"Received rst = 1 and ack = 1");
								fflush(stdout);
								retry = false;
								break;
							}
						}
						else if(scan_type == SCAN_TYPE_NULL || scan_type == SCAN_TYPE_FIN || scan_type == SCAN_TYPE_XMAS)
						{
							//  if a port is closed, it should send back an RST in response to a packet containing anything other than a SYN, ACT or RST
							if(tcpheader->rst == 1 && tcpheader->ack == 1 && source_addr.sin_addr.s_addr == (((sockaddr_in*)(dest_servinfo->ai_addr))->sin_addr).s_addr  && ntohs(tcpheader->source) == destination_port)
							{
								#ifdef _TRACE
									log.write(INFO,"Thread %d: TCP\t%s\tPort %d Closed",threadid,scan_types[scan_type],ntohs(tcpheader->source));
								#endif
 								strcpy(state,port_state_map[CLOSED]);
								strcpy(additional_info,"Received rst = 1 and ack = 1");
								fflush(stdout);
								retry = false;
								break;
							}
						}
						else if(scan_type == SCAN_TYPE_ACK)
						{
							//  In the lack of firewall devices, both open and closed ports will return an RST packet and a scanner can conclude that the port is unfiltered.
							if(tcpheader->rst == 1 && source_addr.sin_addr.s_addr == (((sockaddr_in*)(dest_servinfo->ai_addr))->sin_addr).s_addr  && ntohs(tcpheader->source) == destination_port)
							{
								#ifdef _TRACE
									log.write(INFO,"Thread %d: TCP\t%s\tPort %d Unfiltered",threadid,scan_types[scan_type],ntohs(tcpheader->source));
								#endif
 								strcpy(state,port_state_map[UNFILTERED]);
								strcpy(additional_info,"Received rst = 1 and ack = 1");
								fflush(stdout);
								retry = false;
								break;
							}
						}
					}
				}
			}
			if(FD_ISSET(ICMP_RECV_SOCKET, &rfds))
			{
				if((data_size = recvfrom(ICMP_RECV_SOCKET,icmp_recv_buffer, RECV_BUFFER, 0, &saddr, &saddr_size)) < 0)
					log.write(INFO,"Thread %d: Failed to read icmp data",threadid);
				else
				{
					ipheader = (struct iphdr*) icmp_recv_buffer;
					struct icmphdr *icmp;
					icmp = (struct icmphdr *)(icmp_recv_buffer + (ipheader->ihl)*4);
					struct iphdr* original_ipheader = (struct iphdr*) ((icmp_recv_buffer + (ipheader->ihl)*4) + sizeof(struct icmphdr));
					struct tcphdr *original_tcpheader=(struct tcphdr*)((icmp_recv_buffer + (ipheader->ihl)*4) + sizeof(struct icmphdr) + (original_ipheader->ihl)*4);

					memset(&dest_addr, 0, sizeof(dest_addr));
					dest_addr.sin_addr.s_addr = original_ipheader->daddr;

					#ifdef _TRACE
						inet_ntop(AF_INET, &ipheader->saddr, addr_buffer, ADDR_BUFFER);
						log.write(DEBUG,"Thread %d: Received ICMP Packet - IPv%d: hdr-size=%d pkt-size=%d protocol=%d TTL=%d src=%s ",threadid,ipheader->version, ipheader->ihl*4, ntohs(ipheader->tot_len), ipheader->protocol,ipheader->ttl, addr_buffer);
						log.write(DEBUG,"dst=%s", addr_buffer);
						log.write(DEBUG,"Thread %d: ICMP: type[%d/%d] checksum[%d] id[%d] seq[%d]",threadid,icmp->type, icmp->code, ntohs(icmp->checksum),icmp->un.echo.id, icmp->un.echo.sequence);
					#endif
					// Verify if the ICMP packet is in response to us. i.e. Payload of ICMP should be sames as our original sent packet
					if(dest_addr.sin_addr.s_addr == (((sockaddr_in*)(dest_servinfo->ai_addr))->sin_addr).s_addr && ntohs(original_tcpheader->dest) == destination_port && ntohs(original_tcpheader->source) == LOCAL_PORT)
					{
						if(icmp->type == ICMP_TYPE_3 && 
							(icmp->code == ICMP_HOST_UNREACH || 
							icmp->code == ICMP_PROT_UNREACH ||
							icmp->code == ICMP_PORT_UNREACH ||
							icmp->code == ICMP_NET_ANO ||
							icmp->code == ICMP_HOST_ANO ||
							icmp->code == ICMP_PKT_FILTERED	))
						{
							#ifdef _TRACE
								log.write(INFO,"Thread %d: TCP ICMP\t%s\tPort %d Filtered",threadid,scan_types[scan_type],destination_port);
							#endif
													
							strcpy(state,port_state_map[FILTERED]);
							strcpy(additional_info,"Received ICMP packet");
							fflush(stdout);
							retry = false;
							break;
						}
					}
				}
			}
			if(retry)
			{	// End the timer
				gettimeofday(&retry_end_time, NULL);
				if(retry_end_time.tv_sec  - retry_start_time.tv_sec >= RETRY_TIME_INTERVAL)
				{
					retry_count++;
					break;
				}
			}
		}	// end of while(true)

		if(retry_count >= NUMBER_OF_RETRY)
		{
			retry = false;
			if(scan_type == SCAN_TYPE_SYN)
			{
				// If no response is received after several retransmissions, the port should be marked as filtered.
				#ifdef _TRACE
					log.write(INFO,"Thread %d: TCP TIMEOUT\t%s\tPort %d Filtered",threadid,scan_types[scan_type],destination_port);
				#endif
				strcpy(state,port_state_map[FILTERED]);
				strcpy(additional_info,"Time out");
			}
			else if(scan_type == SCAN_TYPE_NULL || scan_type == SCAN_TYPE_FIN || scan_type == SCAN_TYPE_XMAS || scan_type == SCAN_TYPE_ACK)
			{
				// since firewall devices can block responses, lack of response can only mean that the port is open|filtered.
				#ifdef _TRACE
					log.write(INFO,"Thread %d: TCP TIMEOUT\t%s\tPort %d Open | Filtered",threadid,scan_types[scan_type],destination_port);
				#endif
				strcpy(state,port_state_map[OPEN_OR_FILTERED]);
				strcpy(additional_info,"Time out");
			}
		}
	}		// end of while(retry)

	free(icmp_recv_buffer);
	free(recv_buffer);

	close(TCP_SEND_SOCKET);
	close(TCP_RECV_SOCKET);
	close(ICMP_RECV_SOCKET);

	strcpy(result.state,state);
	strcpy(result.additional_info,additional_info);
	results.push_back(result);
}

void* request_handler(void* arg)
{
	char *current_ipaddress;
	int port;
	int threadid = *((int*)arg);
	
	while(true)
	{
		vector<resulttype> results;
		pthread_mutex_lock(&port_mutex);
		#ifdef _TRACE
			log.write(DEBUG,"Thread %d : Mutex Lock",threadid);
		#endif
		if(args.map_iterator != args.ip_map.end())
		{
			if(((args.map_iterator)->second)->empty())
			{
				// if the queue is empty then increment the map_iterator
				args.map_iterator++;
				if(args.map_iterator == args.ip_map.end())
				{
					pthread_mutex_unlock(&port_mutex);
					#ifdef _TRACE
						log.write(DEBUG,"Thread %d : Mutex UnLock",threadid);
					#endif
					break;
				}
			}
			current_ipaddress = (args.map_iterator)->first;
			port = (int)(((args.map_iterator)->second)->front());
			((args.map_iterator)->second)->pop();
		}
		else
		{
			// There are no more ip addresses
			pthread_mutex_unlock(&port_mutex);
			#ifdef _TRACE
				log.write(DEBUG,"Thread %d : Mutex UnLock",threadid);
				//printf("Thread %d : Mutex UnLock\n",threadid);
			#endif
			break;			
		}
		pthread_mutex_unlock(&port_mutex);
		#ifdef _TRACE
			log.write(DEBUG,"Thread %d : Mutex UnLock",threadid);
			//printf("Thread %d : Mutex UnLock\n",threadid);
		#endif
		
		/* get the address of the destination */
		struct addrinfo dest_hints, *dest_servinfo;
		int status;
		memset(&dest_hints, 0, sizeof dest_hints);

		dest_hints.ai_family = AF_INET;
		dest_hints.ai_socktype = SOCK_STREAM;

		char target_port[10];
		sprintf(target_port,"%d",port);
		
		if((status = getaddrinfo(current_ipaddress,target_port,&dest_hints,&dest_servinfo)) < 0)
		{
			log.write(INFO,"Thread %d: Unable to resolve destination IP : %s. Exiting the program.",threadid,					current_ipaddress);
			cout << gai_strerror(status) << endl;
			exit(1);
		}

		char target_ip[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET,  (void*)(&(((sockaddr_in*)(dest_servinfo->ai_addr))->sin_addr)), target_ip, sizeof target_ip );
		
		#ifdef _TRACE
			log.write(INFO,"Thread %d: %s resolved to Destination IP %s Port: %d",threadid,current_ipaddress,target_ip,				port);
		#endif

		// Now Perform all the relevant scans
		for(int i=0;i< NUMBER_OF_SCAN_TYPES; i++)
		{
			if(args.scan_type[i] == true && i != SCAN_TYPE_PROTOCOL)
				tcp_scan(current_ipaddress,port,threadid,i,results,dest_hints,dest_servinfo);
		}
		
		dest_hints.ai_socktype = SOCK_DGRAM;
		udp_scan(current_ipaddress,port,threadid,results,dest_hints,dest_servinfo);

		vector<resulttype>::iterator it;
		pthread_mutex_lock(&print_mutex);

		for(it = results.begin(); it != results.end(); it++)
		{
			printf("%-5d%-20s%-20s%-10s%-20s%-50s%-30s\n",threadid,target_ip,it->protocol_type,it->scan_type,it->state,
						it->service,it->additional_info);
		}
		pthread_mutex_unlock(&print_mutex);
	}
}

void ip_map_initializer()
{
	vector<char*>::iterator addr_it;

	for(addr_it = args.ip_addresses.begin(); addr_it!= args.ip_addresses.end(); addr_it++)
	{	
		queue<long>* q = new queue<long>(args.global_ports);
		args.ip_map.insert(make_pair<char*, queue<long>*>(*addr_it,q));
	}
}

int main(int argc, char* argv[])
{
	global_init();
	fillOptions(argc, argv);
	
	int threadretval;
	void *status;
	const char *Header[] = {"IP ADDRESS","PROTOCOL/TYPE","SCAN TYPE","STATE","SERVICE","ADDITIONAL INFO","TID"};
	ip_map_initializer();
	args.map_iterator = args.ip_map.begin();
	
	pthread_t threads[args.speedup];
	int temp_thread_id[args.speedup];

	get_local_ip( source_ip );
	log.write(INFO,"Local source IP is %s",source_ip);
	printf("%-5s%-20s%-20s%-10s%-20s%-50s%-30s\n",Header[6],Header[0],Header[1],Header[2],Header[3],Header[4],Header[5]);
	for(int i=0; i< 150 ; i++)
		printf("=");
	cout<<endl;
	pthread_mutex_init(&print_mutex, NULL);

	for(int thread_count=0;thread_count<args.speedup;thread_count++)
	{
		if(thread_count % 100 == 0)
			sleep(1);
		temp_thread_id[thread_count] = thread_count + 1;
		pthread_create(&threads[thread_count], NULL, request_handler, (void *)&temp_thread_id[thread_count]);
	}
	
	for(int thread_count=0;thread_count<args.speedup;thread_count++)
	{
		threadretval = pthread_join(threads[thread_count], &status);
		if (threadretval) 
		{
			 printf("ERROR; return code from pthread_join() is %d\n", threadretval);
			 exit(-1);
		}
		#ifdef _TRACE
			log.write(DEBUG,"Main: completed join with thread %ld having a status of %ld\n",thread_count,(long)status);
		#endif		
	}
		
	// protocol scan
	
	if(args.scan_type[SCAN_TYPE_PROTOCOL])
	{
		#ifdef _TRACE
			log.write(INFO,"got inside protocol scan");
		#endif	
		vector<short>::iterator it2;
		char* current_ipaddress2;
		vector<char*>::iterator addr_iterator;
		struct addrinfo g_dest_hints;
		memset(&g_dest_hints, 0, sizeof g_dest_hints);
		g_dest_hints.ai_family = AF_INET;
		g_dest_hints.ai_socktype = SOCK_STREAM;
		
		int main_thread_id = 0;
		
		for(addr_iterator = args.ip_addresses.begin(); addr_iterator != args.ip_addresses.end(); addr_iterator++)
		{	
			vector<resulttype> protocol_results;
			struct addrinfo *g_dest_servinfo;
			char target_port[10];
			
			#ifndef RANDOM_PROTOCOL_SCAN_PORT
				#define RANDOM_PROTOCOL_SCAN_PORT 7282
			#endif
			
			sprintf(target_port,"%d",RANDOM_PROTOCOL_SCAN_PORT);
			
			current_ipaddress2 = *addr_iterator;
			getaddrinfo(current_ipaddress2,target_port,&g_dest_hints,&g_dest_servinfo);
		
			for(it2 = args.protocol_range.begin(); it2 != args.protocol_range.end(); it2++)
			{
				try
				{
					protocol_scan(current_ipaddress2,*it2,main_thread_id,protocol_results,g_dest_hints,g_dest_servinfo);
				}
				catch (exception e)
				{
					log.write(INFO,"Thread %d : Caught Exception for protocol scan. Protocol %d : ",main_thread_id,*it2,
								e.what());
				}
			}
		
			vector<resulttype>::iterator it3;
			
			char target_ip[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET,  (void*)(&(((sockaddr_in*)(g_dest_servinfo->ai_addr))->sin_addr)), target_ip, 
								sizeof target_ip );

			pthread_mutex_lock(&print_mutex);
			for(it3 = protocol_results.begin(); it3 != protocol_results.end(); it3++)
			{
				printf("%-5d%-20s%-20s%-10s%-20s%-50s%-30s\n",main_thread_id,target_ip,it3->protocol_type,it3->scan_type,
							it3-> state,it3->service,it3->additional_info);
			}
			pthread_mutex_unlock(&print_mutex);
		}
	}
	
	log.write(INFO,"Finished Port Scan");
	fflush(stdout);
}