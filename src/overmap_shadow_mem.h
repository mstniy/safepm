#ifndef OVERMAP_SHADOW_MEM_H
#define OVERMAP_SHADOW_MEM_H

#include <sys/types.h>

void overMapShadowMem(volatile void* start, volatile void* end, int new_shadow_fd, off_t fd_offset);

#endif
