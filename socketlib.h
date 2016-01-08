#ifndef SOCKETLIB_H
#define SOCKETLIB_H

#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER 1
#define CLIENT 2

#define ADDR_STRING_LEN 128
#define UNIX_PATH_MAX 108

#define SA struct sockaddr

int create_tcp_server(const char *node, const char *port);
int create_udp_server(const char *node, const char *port);
int create_datagramtcp_server(const char *node, const char *port);

int create_unix_server(const char* path_name);

int connect_to_tcp_server(const char *node, const char *port);
int connect_to_unix_server(const char* path_name);

/*Returns a string rep of the address associated with the provided socket.
  If the buf perameter is not equal to zero the rep is put there.
  If however the buf is equal to null the rep is put in a static buffer,
  making the function non thread safe.
  The (if) provided buffer must have a length of at least ADDR_STRING_LEN*/
char* socket_atop(int sock, char* buf, uint16_t buf_size);


char* socketlib_strerror(int errnum);

#endif
