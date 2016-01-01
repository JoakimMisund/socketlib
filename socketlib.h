#ifndef SOCKETLIB_H
#define SOCKETLIB_H

#include <sys/types.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <netdb.h>

#define SERVER 1
#define CLIENT 2

int create_tcp_server(const char *node, const char *port);
int create_udp_server(const char *node, const char *port);
int create_datagramtcp_server(const char *node, const char *port);

int connect_to_tcp_server(const char *node, const char *port);

char* socketlib_strerror(int errnum);

#endif
