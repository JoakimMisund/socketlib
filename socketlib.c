#include "socketlib.h"

#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>

int socketlib_error(const char* str)
{
  fprintf(stderr, "ERROR in socketlib: %s\nErrno: %d, Errnomsg: %s\n",
          str, errno, strerror(errno));
  
  exit(errno);
}

char* socket_atop(int sock, char* buf, uint16_t buf_size)
{
  struct sockaddr_storage saddr;
  socklen_t saddrlen;
  static char sbuf[ADDR_STRING_LEN];
  char port_str[8];
  char *buf_to_use;
  int ret;

  if(buf != NULL && buf_size < ADDR_STRING_LEN) {
    socketlib_error("Provided buf to socket_atop is to small!");
  }

  //Decide on which buffer to put the put the result in.
  buf_to_use = (buf == NULL) ? sbuf:buf;

  //Find the address of the socket by calling getsockname
  saddrlen = sizeof(struct sockaddr_storage);
  if( (ret = getsockname(sock, (SA*) &saddr, &saddrlen)) == -1 ) {
    //error
    socketlib_error("Error in the call to getsockname");
  }

  //Based on the type in the sockaddr struct use the appropriate way to extract the rep

  switch(saddr.ss_family) {
  case AF_INET: {
    struct sockaddr_in *addr = (struct sockaddr_in*)&saddr;
    if(inet_ntop(AF_INET, &addr->sin_addr, buf_to_use, ADDR_STRING_LEN) == NULL)
      socketlib_error("Call to inet_ntop failed");
    
    uint16_t port = ntohs(addr->sin_port);
    if(snprintf(port_str, 8, ":%d", port) < 0) {
      socketlib_error("Call to snprintf failed");
    }

    strncat(buf_to_use, port_str, ADDR_STRING_LEN);
    break;
  }

  case AF_INET6: {
    struct sockaddr_in6 *addr = (struct sockaddr_in6*)&saddr;
    if(inet_ntop(AF_INET6, &addr->sin6_addr, buf_to_use, ADDR_STRING_LEN) == NULL)
      socketlib_error("Call to inet_ntop failed");

    uint16_t port = ntohs(addr->sin6_port);
    if(snprintf(port_str, 8, ":%d", port) < 0) {
      socketlib_error("Call to snprintf failed");
    }   

    strncat(buf_to_use, port_str, ADDR_STRING_LEN);
    break;
  }
  }

  //return a pointer to the buf
  return buf_to_use;
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

