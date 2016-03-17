#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
  if( argc != 2 ){
    fprintf(stderr, "Wrong usage!: USAGE: %s [ip addr]\n",argv[0]);
    return -1;
  }

  if(inet_aton(argv[1],NULL) == 0)
    printf("%s is an invalid ipv4 addr\n", argv[1]);
  else
    printf("%s is a valid ipv4 addr\n", argv[1]);

  return 0;
}
