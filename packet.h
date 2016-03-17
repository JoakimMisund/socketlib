#ifndef PACKET
#define PACKET

#include <inttypes.h>
#include <stddef.h>
#include "headers.h"

uint8_t* construct_ether_packet(const uint8_t *dst_addr, const uint8_t *src_addr, const uint16_t ether_proto, const uint8_t *packet, const size_t packet_size);
uint8_t* construct_ipv4_packet(const uint8_t ihl,
                                 const uint8_t tos,
                                 const uint8_t flags,
                                 const uint8_t *src_addr,
                                 const uint8_t *dst_addr,
                                 const uint32_t options,
                                 const char *payload,
                                 const size_t payload_length);

#endif
