#include <libpmemobj_secure.h>
#include <iostream>
#include <assert.h>

POBJ_LAYOUT_BEGIN(example1);
POBJ_LAYOUT_ROOT(example1, struct root);
POBJ_LAYOUT_TOID(example1, struct test);
POBJ_LAYOUT_END(example1);

struct root {
	TOID(struct test) ptest;
};

struct test {
	uint64_t a,b;
};

int main()
{
	PMEMobjpool* pool = spmo::spmemobj_open("pool", "example1");
	if (pool == NULL) {
		 // Try to create it
		pool = spmo::spmemobj_create("pool", "example1", 8*1024*1024, 0660);
		assert(pool != NULL);
	}
	
	PMEMoid proot_ = spmo::spmemobj_root(pool, sizeof(struct root));
	assert(OID_IS_NULL(proot_) == false);
	struct root* proot = (struct root*)pmemobj_direct(proot_);
	
	volatile struct test* ptest = NULL;
	
	TX_BEGIN(pool) {
		PMEMoid res = spmo::spmemobj_tx_alloc(sizeof(struct test), TOID_TYPE_NUM(struct test));
		pmemobj_tx_add_range_direct(&proot->ptest.oid, 16);
		proot->ptest.oid = res;
		ptest = (struct test*) pmemobj_direct(res);
	} TX_END
	
	ptest->a = 1;
	ptest->b = 2;
	
	spmo::spmemobj_close(pool);
	return 0;
}
