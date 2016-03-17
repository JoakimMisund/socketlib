#include "packet.h"
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include <stdio.h>


/* Function: construct_ether_packet
 * Description: Constructs an ethernet packet with the provided information.
 *              Allocates space and puts the information into it, before return a pointer to it.
 * Input: uint8_t *dst_addr Mac address of the destination
 *        uint16_t ether_proto Describs the ethernet protocol type.
 *        uint8_t *packet A pointer to the packet to be put into the frame.
 * Output: uint8_t* A pointer to the resulting frame.
 * Global dependencies: uint8_t if_hwaddr
 */
uint8_t* construct_ether_packet( const uint8_t *dst_addr, const uint8_t *src_addr, const uint16_t ether_proto, const uint8_t *packet, const size_t packet_size )
{
  int frame_length = packet_size + sizeof( ether_header );

  uint8_t *frame = malloc( frame_length );
  if( frame == NULL ) {
    perror( "malloc" );
    return NULL;
  }


  ether_header *hdr = ( ether_header *) frame;
  memcpy( hdr->dst_addr, dst_addr, MAC_ADDR_LEN );
  memcpy( hdr->src_addr, src_addr, MAC_ADDR_LEN );

  hdr->eth_proto[0] = (ether_proto & 0xFF);
  hdr->eth_proto[1] = (uint8_t)(ether_proto>>2 & 0xFF );

  memcpy( hdr->payload, packet, packet_size );

  return frame;
}

uint8_t* construct_ipv4_packet(const uint8_t ihl,
                                 const uint8_t tos,
                                 const uint8_t flags,
                                 const uint8_t *src_addr,
                                 const uint8_t *dst_addr,
                                 const uint32_t options,
                                 const char *payload,
                                 const size_t payload_length)
{

  
}
