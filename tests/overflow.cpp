#include <libpmemobj_secure.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>

POBJ_LAYOUT_BEGIN(spmo_test);
POBJ_LAYOUT_ROOT(spmo_test, struct root);
POBJ_LAYOUT_END(spmo_test);

struct root {
	uint64_t arr[2];
};

int main()
{
	unlink("spmo_test.pool");
	PMEMobjpool* pool = spmo::spmemobj_create("spmo_test.pool", "spmo_test", 32*1024*1024, 0660);
	assert(pool != NULL);
	
	PMEMoid proot_ = spmo::spmemobj_root(pool, sizeof(struct root));
	assert(OID_IS_NULL(proot_) == false);
	struct root* proot = (struct root*)pmemobj_direct(proot_);
	proot->arr[0] = 1;
	proot->arr[1] = 1;
	proot->arr[2] = 1; // This line should crash
	
	spmo::spmemobj_close(pool);
	return 0;
}
