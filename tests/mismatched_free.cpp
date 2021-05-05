#include <libpmemobj_secure.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>

POBJ_LAYOUT_BEGIN(spmo_test);
POBJ_LAYOUT_ROOT(spmo_test, struct root);
POBJ_LAYOUT_TOID(spmo_test, struct dummy);
POBJ_LAYOUT_END(spmo_test);

struct root {
	TOID(struct dummy) obj;
};

struct dummy {
	uint64_t x;
};

int main()
{
	unlink("spmo_test.pool");
	PMEMobjpool* pool = spmo::spmemobj_create("spmo_test.pool", "spmo_test", 32*1024*1024, 0660);
	assert(pool != NULL);

	PMEMoid proot_ = spmo::spmemobj_root(pool, sizeof(struct root));
	assert(OID_IS_NULL(proot_) == false);
	struct root* proot = (struct root*)pmemobj_direct(proot_);

	TX_BEGIN(pool) {
		PMEMoid oid = spmo::spmemobj_tx_alloc(sizeof(struct dummy), TOID_TYPE_NUM(struct dummy));
		pmemobj_tx_add_range_direct(&proot->obj, 16);
		TOID_ASSIGN(proot->obj, oid);
	} TX_ONABORT {
		std::cerr << "Faild to allocate a dummy object" << std::endl;
		abort();
	}
	TX_END
	
	TX_BEGIN(pool) {
		PMEMoid oid = proot->obj.oid;
		oid.off += 8;
		spmo::spmemobj_tx_free(oid);
	} TX_ONCOMMIT {
		std::cerr << "Mismatched free was not cought" << std::endl;
		abort();
	} TX_END
	
	spmo::spmemobj_close(pool);
	return 0;
}
