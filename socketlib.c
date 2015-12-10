#include "socketlib.h"

#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>

int init_socketlib(const uint8_t flags)
{
  options &= flags;
}

int socketlib_error(const char* str)
{
  if(options & ERROR_SILENT == 0)
    fprintf(stderr, "ERROR in socketlib: %s\nErrno: %d, Errnomsg: %s\n",
            str, errno, strerror(errno));
  
  if(options & ERROR_SOFT == 0)
    exit(-1);

  return -1;
}

int create_server_client(const char *node, const char *port,
                         int st, int p, uint8_t type)
{
 struct addrinfo *serverinfo, hints;
 struct addrinfo *itr;
 int sock; /*The returned socket fd*/
 int ret_value; /*Used to store return values from function calls*/
 int optval = 1;

 /*clearing all variables*/
 memset( &hints, 0, sizeof(hints) );
 serverinfo = 0;
 sock = -1;
 ret_value = 0;
 
 hints.ai_family = AF_UNSPEC;
 hints.ai_socktype = st;
 hints.ai_protocol = p;
 hints.ai_flags = AI_PASSIVE;
 
 ret_value = getaddrinfo( node, port, &hints, &serverinfo );
 if( ret_value < 0 ) {
   return socketlib_error(gai_strerror(ret_value));
 }

 for( itr = serverinfo; itr != NULL; itr = itr->ai_next ) {

   sock = socket( itr->ai_family, itr->ai_socktype, itr->ai_protocol );

   if( sock >= 0 ) { /*Socket was created*/

     if( type == SERVER ) {
       if(options & TIME_WAIT == 0)
         setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

       if( bind( sock, itr->ai_addr, itr->ai_addrlen ) == 0 ) { /*success*/
         break;
       }
     } else {
       if( connect( sock, itr->ai_addr, itr->ai_addrlen ) != -1 ) { /*success*/
         break;
       }
     }
   }
 }

 freeaddrinfo( serverinfo );


 if( itr == NULL ) {
   return socketlib_error("Unable to connect/bind to any of the addresses provided by getaddrinfo!\n");
 }

 if( type == SERVER ) {
   ret_value = listen( sock, 10 );
   if( ret_value < 0 ) {
     return socketlib_error("Error in the call to listen");
   }
 }

 return sock;
}

int create_tcp_server(const char *node, const char *port)
{
  return create_server_client(node, port, SOCK_STREAM, IPPROTO_TCP, SERVER);
}
int create_udp_server(const char *node, const char *port)
{
  return create_server_client(node, port, SOCK_DGRAM, IPPROTO_UDP, SERVER);
}
int create_datagramtcp_server(const char *node, const char *port)
{
  return create_server_client(node, port, SOCK_SEQPACKET, 0, SERVER);
}

int connect_to_tcp_server(const char *node, const char *port)
{
  return create_server_client(node, port, SOCK_STREAM, IPPROTO_TCP, CLIENT);  
}

int Send(int sockfd, const void *buf, size_t len, int flags)
{
  return socketlib_error("Error Send not implemented\n");
}
