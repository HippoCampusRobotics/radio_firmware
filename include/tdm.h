#ifndef INCLUDE_TDM
#define INCLUDE_TDM

#include <stdint.h>

#define TDM_MAX_RSSI_STATS 10

struct tdm_trailer_s {
    uint8_t sync_hops;
    uint8_t node_id;
};

void tdm_init();
void tdm_run();

#endif /* INCLUDE_TDM */
