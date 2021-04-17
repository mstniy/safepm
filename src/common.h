#ifndef PMDK_ASAN_COMMON_H
#define PMDK_ASAN_COMMON_H

#include <stdint.h>

/*

shadow memory location: 0x7fff7000..0x10007FFF8000
 location in shadow memory: 0x8FFF6E00..0x2008FFF7000
 thus, the address range    0x8fff7000..0x2008fff7000 is marked unaccessible at the paging level
 this ensures that no instrumented code can try to read/write the shadow memory itself
 the range bw. 6E00 to 7000 is not protected, unfortunately, because the unaccessible region has to be 4kb-aligned.
 
 "The malloc call stack is stored in the left redzone (the larger the redzone, the larger the number of frames that can be stored) while the free call stack is stored in the beginning of the memory region itself."
    ->storing the call stack, which is composed of volatile/randomized pointers, is probably not helpful in the context of persistent memory
*/

namespace spmo {
	namespace detail {
	
		inline uint8_t* get_shadow_mem_location(void* _p) {
		  uint64_t p = (uint64_t)_p;
		  return (uint8_t*)((p>>3)+0x7fff8000);
		}

	};
};

#endif
