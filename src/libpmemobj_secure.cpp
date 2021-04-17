#include "../include/libpmemobj_secure.h"
#include "common.h"
#include "overmap_shadow_mem.h"

#include <sys/stat.h>
#include <fcntl.h>           /* For O_* constants */
#include <string>
#include <assert.h>

namespace spmo {

	namespace detail {
		constexpr size_t LIBPMEMOBJ_HEADER_SIZE = 8192;

		void overmap_pool(const char* path, PMEMobjpool* pool) {
			TOID(struct root) roott = POBJ_ROOT(pool, struct root);
			const root *rootp = D_RO(roott);
			uint64_t shadow_mem_off = rootp->shadow_mem.oid.off;
			int fd = open(path, O_RDWR);
			assert(fd != -1); // TODO: Handle errors gracefully
			struct stat pool_stat;
			assert(fstat(fd, &pool_stat) == 0);
			off_t pool_size = pool_stat.st_size;
			// Because overMapShadowMem requires 8kb*4 alignment, and pmemobj_open does not guarantee such an alignment,
			//   we (possibly) forgo memory protection at the very start & end of the usable portion of the pool.
			// TODO: Solve this issue.
			uint64_t usable_start = (uint64_t)pool + LIBPMEMOBJ_HEADER_SIZE;
			uint64_t usable_start_misalignment = usable_start % (8*4096); // 8*4kb is the alignment required by overMapShadowMem
			uint64_t usable_start_aligned = (usable_start_misalignment == 0) ? usable_start : (usable_start + 8*4096 - usable_start_misalignment);
			uint64_t pool_end = (uint64_t)pool + pool_size;
			uint64_t pool_end_misalignment = pool_end % (8*4096); // 8*4kb is the alignment required by overMapShadowMem
			uint64_t pool_end_aligned = pool_end - pool_end_misalignment;
			overMapShadowMem((volatile void*)usable_start_aligned, (volatile void *)pool_end_aligned, fd, shadow_mem_off+(usable_start_aligned-usable_start)/8);
		}
	};

	PMEMobjpool *spmemobj_open(const char *path, const char *real_layout) {
		std::string layout = std::string("spmo_") + real_layout;
		PMEMobjpool* pool = pmemobj_open(path, layout.c_str());
		if (pool == NULL)
			return NULL;
		detail::overmap_pool(path, pool);
		
		return pool;
	}
	PMEMobjpool *spmemobj_create(const char *path, const char *layout, size_t poolsize, mode_t mode) {
		if (poolsize%64) // Poolsize needs to be 64-padded because shadow_size needs to be 8-padded (for marking the red-zone)
			poolsize += 64 - poolsize%64;
		PMEMobjpool* pool = pmemobj_create(path, layout, poolsize, mode);
		if (pool == NULL)
			return NULL;
		TOID(struct root) roott = POBJ_ROOT(pool, struct root);
		
		TX_BEGIN(pool) {
			root* rootp = D_RW(roott);
			size_t shadow_size = (poolsize-detail::LIBPMEMOBJ_HEADER_SIZE)/8;
			// Allocate and zero-initialize the persistent shadow memory
			assert(0 == POBJ_ZALLOC(pool, &rootp->shadow_mem, struct shadowmem, shadow_size));
			// Overmap the persistent shadow memory on top of the (volatile) shadow memory created by ASan
			detail::overmap_pool(path, pool);
			
			uint8_t* vmem_shadow_mem_start = (uint8_t*)D_RW(rootp->shadow_mem);
			uint8_t* vmem_shadow_mem_end = vmem_shadow_mem_start + shadow_size;
			
			void* shadow_shadow_mem_start = detail::get_shadow_mem_location(vmem_shadow_mem_start);
			
			// Mark the red zone within the persistent shadow mem
			// The red zone corresponding to the volatile persistent memory range is marked non-accessible on a page permission level, because filling the red zone with -1 would allocate physical memory.
			// But we simply set it to -1 
			pmemobj_tx_add_range_direct(shadow_shadow_mem_start, shadow_size/8);
			memset(shadow_shadow_mem_start, -1, shadow_size/8); // Note that because of the overmapping, this line will operate on persistent memory
		}
		TX_ONABORT {
			assert(false);
		} TX_END
		
		return pool;
	}
	void spmemobj_close(PMEMobjpool *pop);
	
	PMEMoid spmemobj_root(PMEMobjpool *pop, size_t size);
	PMEMoid spmemobj_root_construct(PMEMobjpool *pop, size_t size, pmemobj_constr constructor, void *arg);
	size_t spmemobj_root_size(PMEMobjpool *pop);
	
	PMEMoid spmemobj_tx_alloc(size_t size, uint64_t type_num);
	PMEMoid spmemobj_tx_zalloc(size_t size, uint64_t type_num);
	PMEMoid spmemobj_tx_realloc(PMEMoid oid, size_t size, uint64_t type_num);
	PMEMoid spmemobj_tx_zrealloc(PMEMoid oid, size_t size, uint64_t type_num);
	PMEMoid spmemobj_tx_strdup(const char *s, uint64_t type_num);
	int spmemobj_tx_free(PMEMoid oid);
};
