#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

#include "common.h"

namespace spmo {
	namespace detail {
	
		// *start* and *end* need to be 4kb*8 aligned
		// 
		void overMapShadowMem(volatile void* start, volatile void* end, int new_shadow_fd, off_t fd_offset)
		{
		    int prot = PROT_READ | PROT_WRITE;
		    const uint64_t size = ((uint64_t)end - (uint64_t)start)/8; // This size is passed to munmap. So it need not be a multiple of the page size
		    uint8_t* shadow_start = get_shadow_mem_location((void*)start);
		    assert((uint64_t)shadow_start % 4096 == 0);
		    assert((uint64_t)get_shadow_mem_location((void*)end) % 4096 == 0);
		    void* res = mmap((void*)shadow_start, size, prot, MAP_SHARED | MAP_FIXED, new_shadow_fd, fd_offset);
		    assert(res == (void*)shadow_start);
		}

	};
};
