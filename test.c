#include "socketlib.h"

#include <stdio.h>
#include <unistd.h>

void test_unix_sock();

int main(void)
{
  /*int sockfd = create_tcp_server("localhost","5000");

  char buf[ADDR_STRING_LEN];

  char *sbuf = socket_atop(sockfd,buf,ADDR_STRING_LEN);
  printf("User allocated buffer: %s\n", buf);
  printf("Static buffer: %s\n", sbuf);
  bzero(buf,ADDR_STRING_LEN);

  test_unix_sock();
  */
  send_ping("192.168.1.1", "192.168.1.6");
  //close(sockfd);
  return 0;
}


void test_unix_sock()
{
  printf("Testing unix sockets\n");

  char *path = "TestNavn";

  int su_sock = create_unix_server(path);

  char *sbuf = socket_atop(su_sock,NULL,0);
  printf("Created server socket with path: %s\n", sbuf);

  if( fork() == 0 ) {
    int cu_sock = connect_to_unix_server(path);

    send(cu_sock, "Hei, fra clienten", 17, 0);
    close(cu_sock);
    return;
  }

  int client = accept(su_sock,NULL,NULL);

  char buf[20];

  int n = recv(client, buf, 20,0);
  buf[n] = 0;

  printf("Recv from client: %s\n", buf);
  close(su_sock);

  unlink(path);
}
