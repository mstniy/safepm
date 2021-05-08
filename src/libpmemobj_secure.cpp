#include "../include/libpmemobj_secure.h"

#include <sys/stat.h>
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <string>
#include <assert.h>
#include <iostream>

namespace spmo {

	namespace detail {
		const size_t RED_ZONE_SIZE = 256;

		__attribute__((no_sanitize("address")))
		PMEMoid alloc_additional_work(PMEMoid orig, size_t size) {
			if (OID_IS_NULL(orig)) {
				return orig;
			}
			uint8_t* direct = (uint8_t*)pmemobj_direct(orig);
			*(uint64_t*)direct = size; // Remember the size of the object in the first 8 bytes of the left redzone
			mark_mem(direct, RED_ZONE_SIZE, TAG::LEFT_REDZONE);
			mark_mem(direct+RED_ZONE_SIZE, size, TAG::ADDRESSABLE); // In case it was poisoned by an earlier free
			mark_mem(direct+RED_ZONE_SIZE+size, RED_ZONE_SIZE, TAG::RIGHT_REDZONE);
			PMEMoid shadow = get_persistent_shadow_mem_from_oid(orig);

			int res = pmemobj_tx_add_range(shadow, orig.off/8, (2*RED_ZONE_SIZE+size+7)/8);
			if (res) {
				return OID_NULL;
			}
			orig.off += RED_ZONE_SIZE;
			return orig;
		}
	};

	

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
	__attribute__((no_sanitize("address")))
	int spmemobj_tx_free(PMEMoid oid) {
		uint8_t* shadow_object_start = detail::get_shadow_mem_location(pmemobj_direct(oid));
		assert(int8_t(*shadow_object_start) >= 0 && "Invalid free");
		assert(*(shadow_object_start-1) == detail::TAG::LEFT_REDZONE && "Invalid free");
		PMEMoid redzone_start{.pool_uuid_lo = oid.pool_uuid_lo, .off = oid.off - detail::RED_ZONE_SIZE};
		int res;
		if (res = pmemobj_tx_free(redzone_start)) // TODO: Quarantine the region to provide additional temporal safety
			return res;

		uint64_t size = *(uint64_t*)pmemobj_direct(redzone_start); // Read the object size off the red zone
		size_t total_len = size+2*detail::RED_ZONE_SIZE;
		PMEMoid shadow_oid = detail::get_persistent_shadow_mem_from_oid(oid);
		if (res = pmemobj_tx_add_range(shadow_oid, redzone_start.off/8, (total_len+7)/8))
			return res;
		detail::mark_mem(pmemobj_direct(redzone_start), total_len, detail::TAG::FREED);

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
