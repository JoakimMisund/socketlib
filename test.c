#include "socketlib.h"

#include <stdio.h>
#include <unistd.h>

void test_unix_sock();

#define BUF_SIZE 100
#define SERV_ADDR "192.168.1.1"

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


	int raw_sock = create_raw_ip_socket();
	int icmp_sock = create_icmp_socket();
	int recvd;
	char buf[BUF_SIZE];
	struct sockaddr_in sender_addr = {0};
	socklen_t addr_len = sizeof(struct sockaddr_in);

	if (raw_sock == -1 || icmp_sock == -1)
		return 1;


	fprintf(stderr,"Sending echo to %s\n", SERV_ADDR);
	if (0 != send_echo_msg(raw_sock, SERV_ADDR, "192.168.1.6", 67)) {
		fprintf(stderr,"error in send_echo\n");
	}


	fprintf(stderr,"Waiting for echo response\n");
	if ((recvd = recvfrom(icmp_sock, buf, BUF_SIZE, 0, (SA*)&sender_addr, &addr_len)) <= 0) {
		perror("error recvfrom");
		return 2;
	}

	char ip_buf[15];
	printf("Received a message from ip: %s", inet_ntop(AF_INET, &sender_addr.sin_addr, ip_buf, 15));

	print_icmp_message((struct iphdr*) buf);

	close(raw_sock);
	close(icmp_sock);
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
