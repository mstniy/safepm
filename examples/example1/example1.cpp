#include <libpmemobj_secure.h>
#include <iostream>
#include <assert.h>

POBJ_LAYOUT_BEGIN(example1);
POBJ_LAYOUT_ROOT(example1, struct root);
POBJ_LAYOUT_END(example1);

struct root {
	uint64_t arr[2];
};

int main()
{
	PMEMobjpool* pool = spmo::spmemobj_open("pool", "example1");
	if (pool == NULL) {
		 // Try to create it
		pool = spmo::spmemobj_create("pool", "example1", 32*1024*1024, 0660);
		assert(pool != NULL);
	}
	
	PMEMoid proot_ = spmo::spmemobj_root(pool, sizeof(struct root));
	assert(OID_IS_NULL(proot_) == false);
	struct root* proot = (struct root*)pmemobj_direct(proot_);
	proot->arr[0] = 1;
	proot->arr[1] = 1;
	proot->arr[2] = 1; // This line should crash
	
	spmo::spmemobj_close(pool);
	return 0;
}
