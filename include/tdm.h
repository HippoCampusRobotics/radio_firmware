#ifndef INCLUDE_TDM
#define INCLUDE_TDM

#include <stdint.h>

struct tdm_trailer_s {
    uint8_t node_id;
};

void tdm_init();
void tdm_run();

#endif /* INCLUDE_TDM */
