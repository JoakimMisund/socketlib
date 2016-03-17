#ifndef HEADERS
#define HEADERS

#include <inttypes.h>


struct ether_header
{
  uint8_t dst_addr[6];
  uint8_t src_addr[6];
  uint8_t eth_proto[2];
  uint8_t    payload[0];
} __attribute__((packed));

struct ipv4_header
{
  uint8_t v_ihl;
  uint8_t tos;
  uint16_t tot_len;
  uint16_t id;
  uint16_t flags_fragment_offset;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t header_checksum;
  uint8_t src_addr[4];
  uint8_t dst_addr[4];
  uint32_t opts_padding;
  char payload[0];

#define VERSION(ipv4_h) ((ipv4_h->v_ihl>>4) & 0x0f);
#define TOS(ipv4_h) (ipv4_h->tos);
#define IHL(ipv4_h) (ipv4_h->v_ihl & 0x0f);
#define TOTAL_LENGTH(ipv4_h) (ipv4_h->tot_len);
#define ID(ipv4_h) (ipv4_h->id);
#define FLAGS(ipv4_h) ((ipv4_h->flags_fragment_offset>>12) & 0x0007);
#define FRAGMENT_OFFSET(ipv4_h) (ipv4_h->flags_fragment_offset & 0x1fff);
#define TTL(ipv4_h) (ipv4_h->ttl);
#define PROTOCOL(ipv4_h) (ipv4_h->protocol);
#define HEADER_CHECKSUM(ipv4_h) (ipv4_h->header_checksum);
#define SOURCE_ADDR(ipv4_h) (ipv4_h->src_addr);
#define DESTINATION_ADDR(ipv4_h) (ipv4_h->dst_addr);
#define OPTIONS(ipv4_h) ((ipv4_h->opts_padding>>8) & 0x00ffffff)
} __attribute__((packed));


#define MAC_ADDR_LEN 48



typedef struct ipv4_header ipv4_header;
typedef struct ether_header ether_header;

#endif
