#include "socketlib.h"

#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>

int socketlib_init(const uint8_t flags)
{
  options &= flags;
}

int socketlib_set_sendrecv_protocol(const uint8_t proto)
{
  sendrecv_proto = proto;
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

ssize_t Send(int sockfd, const void *buf, size_t len, int flags)
{
  ssize_t sent = 0;
  
  if(sendrecv_proto & PROTO_LEN != 0) {
    size_t len = htonl((uint32_t)len);
    send(sockfd, &len, sizeof(size_t), flags);
  } else if(sendrecv_proto & PROTO_ESCAPE != 0) {
    socketlib_error("ESCAPE proto not implemented!\n");
  }

  int tries = 0;

  while(sent < len) {
    int ret_value = send(sockfd, buf + sent, len - sent, flags);

    if(ret_value == -1)
      return -1;

    sent += ret_value;
    
    if(tries++ > MAX_NR_SEND_TRIES)
      return -1;
  }

  if(sendrecv_proto & PROTO_ESCAPE != 0) {
    socketlib_error("ESCAPE proto not implemented!\n");
  }
  
  return sent;
}


ssize_t Recv(int sockfd, void *buf, size_t len, int flags)
{
  ssize_t recvd = 0;



  if(sendrecv_proto & PROTO_LEN != 0) {

    size_t msg_len = -1;
    recv(sockfd, &msg_len, sizeof(size_t), flags);
    msg_len = ntohl(msg_len);

    if(len < msg_len) {
      return -1;
    }

    int tries = 0;
    do {
      int ret_value = recv(sockfd, buf + recvd, msg_len - recvd, flags);

      if(ret_value == -1)
        return -1;

      recvd += ret_value;

      if(tries++ > MAX_NR_RECV_TRIES)
        return -1;

    } while(recvd < msg_len);

  } else if(sendrecv_proto & PROTO_ESCAPE != 0) {

    socketlib_error("ESCAPE proto not implemented!\n");

  } else {
  
    recvd = recv(sockfd, buf, len, flags);
  }
  
  

  return recvd;
}
