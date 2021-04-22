#include "../include/libpmemobj_secure.h"
#include "common.h"
#include "overmap_shadow_mem.h"

#include <sys/stat.h>
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
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
			uint64_t pool_end = (uint64_t)pool + rootp->pool_size;
			overMapShadowMem((volatile void*)pool, (volatile void *)pool_end, fd, shadow_mem_off);
			close(fd);
		}

		void undo_overmap_pool(PMEMobjpool* pool) {
			TOID(struct root) roott = POBJ_ROOT(pool, struct root);
			const root *rootp = D_RO(roott);
			uint64_t pool_end = (uint64_t)pool + rootp->pool_size;
			undoOverMap((volatile void*)pool, (volatile void *)pool_end);
		}
		
		std::string add_layout_prefix(const char* real_layout) {
			return std::string("spmo_") + real_layout;
		}
	};

	PMEMobjpool *spmemobj_open(const char *path, const char *real_layout) {
		std::string layout = detail::add_layout_prefix(real_layout);
		PMEMobjpool* pool = pmemobj_open(path, layout.c_str());
		if (pool == NULL)
			return NULL;
		detail::overmap_pool(path, pool);
		
		return pool;
	}
	PMEMobjpool *spmemobj_create(const char *path, const char *real_layout, size_t poolsize, mode_t mode) {
		std::string layout = detail::add_layout_prefix(real_layout);
		if (poolsize%(4096*8)) // Poolsize needs to be 8*4kb-padded because the shadow memory needs to be 4kb-padded (for marking the red-zone)
			poolsize += (4096*8) - poolsize%(4096*8);
		PMEMobjpool* pool = pmemobj_create(path, layout.c_str(), poolsize, mode);
		if (pool == NULL)
			return NULL;
		TOID(struct detail::root) roott = POBJ_ROOT(pool, struct detail::root);
		assert(TOID_IS_NULL(roott) == false);

		detail::root* rootp = D_RW(roott);
		rootp->pool_size = poolsize;
		rootp->real_root_size = 0;
		size_t shadow_size = poolsize/8; // The shadow memory encompasses information about the whole pool: including the pmdk header and the shadow memory itself
		// Allocate and zero-initialize the persistent shadow memory
		TOID(struct detail::shadowmem) shadow_mem;
		
		assert(0 == POBJ_ZALLOC(pool, &shadow_mem, struct detail::shadowmem, shadow_size+4096)); // The offset of the shadow memory needs to be 4k aligned (because we want to be able to map it independently of the rest of the pool), so alloate some additional space
		if (shadow_mem.oid.off % (4096)) // Make sure the shadow mem offset is 4kb aligned. We don't need to store the original address returned by POBJ_ZALLOC because we won't free the shadow memory
			shadow_mem.oid.off += 4096 - shadow_mem.oid.off%(4096);
		rootp->shadow_mem = shadow_mem;

		pmemobj_persist(pool, rootp, sizeof(struct detail::root));

		// Overmap the persistent shadow memory on top of the (volatile) shadow memory created by ASan
		detail::overmap_pool(path, pool);
		
		uint8_t* vmem_shadow_mem_start = (uint8_t*)D_RW(rootp->shadow_mem);
		
		// Mark the red zone within the persistent shadow mem
		// The red zone corresponding to the volatile persistent memory range is marked non-accessible on a page permission level, because filling the red zone with -1 would allocate physical memory.
		// But we simply set it to -1
		// TODO: For portions of the persistent shadow memory that correspond to itself, avoid marking them -1 and instead mark them no read/write using page permissions.
		//       Just like the regular ASan
		void* shadow_shadow_mem_start = detail::get_shadow_mem_location(vmem_shadow_mem_start);
		pmemobj_memset_persist(pool, shadow_shadow_mem_start, -1, shadow_size/8); // Note that because of the overmapping, the change will be mirrored to the overmapped shadow memory.
		
		return pool;
	}

	void spmemobj_close(PMEMobjpool *pop) {
		detail::undo_overmap_pool(pop);
		pmemobj_close(pop);
	}
	
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
