#ifndef INCLUDE_PKT
#define INCLUDE_PKT

#include "tdm.h"
#include <stdbool.h>
#include <stdint.h>

#define PKT_HEADER_LENGTH sizeof(pkt_header_t)
#define PKT_CRC_LENGTH 2

#define PKT_BUFFER_SIZE 256

#define PKT_MAX_NO 4

#define PKT_COBS_IDX 0
#define PKT_LEN_IDX 1
#define PKT_NODEID_IDX 2
#define PKT_MSG_ID_IDX 3

void pkt_send_packets(struct tdm_trailer_s *trailer);
void pkt_update_buffer();


typedef struct {
    uint8_t n_packets;
    uint8_t ids[PKT_MAX_NO];
    uint8_t offsets[PKT_MAX_NO];
    uint8_t sizes[PKT_MAX_NO];
} pkt_manager_t;


extern __pdata pkt_manager_t _pkt_manager;

typedef struct pkt_header_s {
    uint8_t length;
    uint8_t node_id;
    uint8_t msg_id;
} pkt_header_t;

#endif /* INCLUDE_PKT */
