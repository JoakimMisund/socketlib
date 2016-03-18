#ifndef SOCKETLIB_H
#define SOCKETLIB_H

#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>

#define SERVER 1
#define CLIENT 2

#define ADDR_STRING_LEN 128
#define UNIX_PATH_MAX 108

#define SA struct sockaddr

#define ERROR_MISSING_PARAMETER -1
#define ERROR_BUFFER_TOO_SMALL -2

struct icmpdata {
	char* content;
	size_t content_size;

	uint8_t pointer;
	char *gw_internet_addr;

	uint16_t id;
	uint16_t sequence_num;

	uint32_t orig_timestamp;
	uint32_t recv_timestamp;
	uint32_t tran_timestamp;
};

int create_tcp_server(const char *node, const char *port);
int create_udp_server(const char *node, const char *port);
int create_datagramtcp_server(const char *node, const char *port);

int create_unix_server(const char* path_name);

int connect_to_tcp_server(const char *node, const char *port);
int connect_to_unix_server(const char* path_name);

int send_echo_msg(int sock, char* dst, char* src, uint16_t id);
int create_icmp_header(uint8_t type, uint8_t code, struct icmpdata *data, char *buf, size_t buf_size);

int create_raw_ip_socket();
int create_icmp_socket();

void print_icmp_message(struct iphdr *ip_hdr);



/*Returns a string rep of the address associated with the provided socket.
  If the buf perameter is not equal to zero the rep is put there.
  If however the buf is equal to null the rep is put in a static buffer,
  making the function non thread safe.
  The (if) provided buffer must have a length of at least ADDR_STRING_LEN*/
char* socket_atop(int sock, char* buf, uint16_t buf_size);


char* socketlib_strerror(int errnum);

#endif
