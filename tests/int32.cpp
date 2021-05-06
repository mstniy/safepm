#include <libpmemobj_secure.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>

POBJ_LAYOUT_BEGIN(spmo_test);
POBJ_LAYOUT_ROOT(spmo_test, struct root);
POBJ_LAYOUT_TOID(spmo_test, struct dummy);
POBJ_LAYOUT_END(spmo_test);

struct root {
	uint32_t i;
};

int main()
{
	unlink("spmo_test.pool");
	PMEMobjpool* pool = spmo::spmemobj_create("spmo_test.pool", "spmo_test", 32*1024*1024, 0660);
	assert(pool != NULL);

	PMEMoid proot_ = spmo::spmemobj_root(pool, sizeof(struct root));
	assert(OID_IS_NULL(proot_) == false);
	struct root* proot = (struct root*)pmemobj_direct(proot_);
	
	proot->i = 1;
	*(uint64_t*)(&proot->i) = 2; // This line should crash

	
	spmo::spmemobj_close(pool);
	return 0;
}
