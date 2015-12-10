#include <sys/types.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <netdb.h>

#define ERROR_SOFT 0x01
#define ERROR_HARD 0x02
#define ERROR_SILENT 0x04
#define ERROR_VERBOSE 0x08
#define TIME_WAIT 0x10

#define SERVER 0x01
#define CLIENT 0x02

uint8_t options;

int init_socketlib(const uint8_t flags);

int socketlib_error(const char* str);

int create_tcp_server(const char *node, const char *port);
int create_udp_server(const char *node, const char *port);
int create_datagramtcp_server(const char *node, const char *port);

int connect_to_tcp_server(const char *node, const char *port);

int Send(int sockfd, const void *buf, size_t len, int flags);
