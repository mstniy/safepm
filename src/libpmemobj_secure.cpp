#include "../include/libpmemobj_secure.h"
#include "common.h"
#include "overmap_shadow_mem.h"

#include <sys/stat.h>
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <string>
#include <assert.h>
#include <iostream>

namespace spmo {

	namespace detail {
		const size_t RED_ZONE_SIZE = 256;

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

		// Even with no_sanitize, calls to memset get intercepted by ASan,
		//  which is uncomfortable with us directly modifying the shadow memory
		__attribute__((no_sanitize("address")))
		void mymemset(uint8_t* start, uint8_t byt, size_t len) {
			while (len) {
				*start = byt;
				start++;
				len--;
			}
		}
		
		// len in bytes
		__attribute__((no_sanitize("address")))
		void mark_mem(void* start, size_t len, uint8_t tag) {
			assert(int8_t(tag) <= 0);
			if (uint64_t(start)%8) {
				int misalignment = uint64_t(start)%8;
				uint8_t* shadow_pos = get_shadow_mem_location(start);
				if (tag)
					*shadow_pos = misalignment; // We don't really need to check the previous value of *shadow_start here, because pmemobj would not distribute the same 8-byte chunk to multiple objects.
				else
					*shadow_pos = 0;				
				start = (void*)(uint64_t(start)+8-misalignment);
				len -= 8-misalignment;
			}
			mymemset(get_shadow_mem_location(start), tag, len/8);
			if (len%8) {
				int prot = len%8;
				uint8_t* shadow_pos = get_shadow_mem_location((uint8_t*)start+len);
				if (tag)
					*shadow_pos = prot;
				else
					*shadow_pos = 0;
			}
		}
		
		PMEMoid alloc_additional_work(PMEMoid orig, size_t size) {
			if (OID_IS_NULL(orig)) {
				return orig;
			}
			uint8_t* direct = (uint8_t*)pmemobj_direct(orig);
			mark_mem(direct, RED_ZONE_SIZE, TAG::LEFT_REDZONE);
			mark_mem(direct+RED_ZONE_SIZE, size, TAG::ADDRESSABLE); // In case it was poisoned by an earlier free
			mark_mem(direct+RED_ZONE_SIZE+size, RED_ZONE_SIZE, TAG::RIGHT_REDZONE);
			PMEMobjpool* pool = pmemobj_pool_by_oid(orig);
			TOID(struct root) rootp_ = POBJ_ROOT(pool, struct root); // TODO: A tighter integration with libpmemobj could let us avoid re-retrieving the persistent shadow memory location for each operation
			const struct root* rootp = D_RO(rootp_);

			int res = pmemobj_tx_add_range(rootp->shadow_mem.oid, orig.off/8, (2*RED_ZONE_SIZE+size+7)/8);
			if (res) {
				return OID_NULL;
			}
			orig.off += RED_ZONE_SIZE;
			return orig;
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
		//pmemobj_memset_persist(pool, vmem_shadow_mem_start + rootp->shadow_mem.oid.off/8, detail::TAG::INTERNAL, shadow_size/8); // Note that because of the overmapping, the change will be mirrored to the overmapped shadow memory.
		
		// This is disabled for now, because:
		// During transactional memory operations we need to add parts of the shadow memory to the undo log.
		// Adding the shadow memory to the transaction undo log causes libpmemobj
		//  to copy it, using memcpy. This call gets intercepted and instrumented by ASan.
		//  Because the persistent shadow memory (in the original file mapping) is marked inaccessible,
		//    ASan memcpy raises an error.
		//detail::mymemset(vmem_shadow_mem_start + rootp->shadow_mem.oid.off/8, detail::TAG::INTERNAL, shadow_size/8);
		//pmemobj_persist(pool, vmem_shadow_mem_start + rootp->shadow_mem.oid.off/8, shadow_size/8);
		
		return pool;
	}

	void spmemobj_close(PMEMobjpool *pop) {
		detail::undo_overmap_pool(pop);
		pmemobj_close(pop);
	}
	
	PMEMoid spmemobj_root(PMEMobjpool *pool, size_t size) {
		TOID(struct detail::root) roott = POBJ_ROOT(pool, struct detail::root);
		assert(TOID_IS_NULL(roott) == false);

		detail::root* rootp = D_RW(roott);
		if (OID_IS_NULL(rootp->real_root)) {
			PMEMoid real_root;
			TX_BEGIN(pool) {
				real_root = spmemobj_tx_alloc(size, POBJ_ROOT_TYPE_NUM);

				pmemobj_tx_add_range_direct(&rootp->real_root, 24);

				rootp->real_root = real_root;
				rootp->real_root_size = size;
			} TX_ONABORT {
				return OID_NULL;
			} TX_END
			
			return real_root;
		}
		else {
			// TODO: Remove this condition once we implement realloc
			assert(rootp->real_root_size >= size);
			return rootp->real_root;
		}
	}
	PMEMoid spmemobj_root_construct(PMEMobjpool *pop, size_t size, pmemobj_constr constructor, void *arg);
	size_t spmemobj_root_size(PMEMobjpool *pop);
	
	PMEMoid spmemobj_tx_alloc(size_t size, uint64_t type_num) {
		//TODO: A custom allocator could save us one redzone per object.
		PMEMoid orig = pmemobj_tx_alloc(size+2*detail::RED_ZONE_SIZE, type_num+TOID_TYPE_NUM(struct detail::end));
		return detail::alloc_additional_work(orig, size);
	}
	int spmemobj_tx_free(PMEMoid oid) {
		//TODO: Check for possible double-free. What if the user passes an object not allocated by spmemobj_* ? We must use a custom allocator/modify the existing one/look inside the internal state of the existing one.
		//  OR, we could store object size in the left redzone.
		//    ASAN stores alloc/free stack traces there, but they aren't immediately useful in the context of persistent memory.
		PMEMoid oid2 = oid;
		oid2.off -= detail::RED_ZONE_SIZE;
		int res = pmemobj_tx_free(oid2);
		if (res)
			return res;
		// TODO: Poison the memory. This requires us to know the size of the object being freed.
		return 0;
	}
	PMEMoid spmemobj_tx_zalloc(size_t size, uint64_t type_num) {
		PMEMoid orig = pmemobj_tx_zalloc(size+2*detail::RED_ZONE_SIZE, type_num+TOID_TYPE_NUM(struct detail::end));
		return detail::alloc_additional_work(orig, size);
	}
	PMEMoid spmemobj_tx_realloc(PMEMoid oid, size_t size, uint64_t type_num);
	PMEMoid spmemobj_tx_zrealloc(PMEMoid oid, size_t size, uint64_t type_num);
	PMEMoid spmemobj_tx_strdup(const char *s, uint64_t type_num);
};
