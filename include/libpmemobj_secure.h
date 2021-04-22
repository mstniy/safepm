#ifndef LIBPMEMOBJ_SECURE_H
#define LIBPMEMOBJ_SECURE_H

#include <libpmemobj.h>
namespace spmo {

	namespace detail {
		POBJ_LAYOUT_BEGIN(libpmemobj_secure); // The exact layout name here does not matter
		POBJ_LAYOUT_ROOT(libpmemobj_secure, struct root);
		POBJ_LAYOUT_TOID(libpmemobj_secure, struct shadowmem);
		POBJ_LAYOUT_END(libpmemobj_secure);

		struct shadowmem {}; // A dummy type, whose type number is used for the shadow memory.

		struct root {
			TOID(struct shadowmem) shadow_mem;
			uint64_t pool_size;
			PMEMoid real_root;
			uint64_t real_root_size;
		};
	};

PMEMobjpool *spmemobj_open(const char *path, const char *layout);
PMEMobjpool *spmemobj_create(const char *path, const char *layout, size_t poolsize, mode_t mode);
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

#endif
