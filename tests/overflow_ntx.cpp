#include <libpmemobj.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include "common.h"

POBJ_LAYOUT_BEGIN(spmo_test);
POBJ_LAYOUT_ROOT(spmo_test, struct root);
POBJ_LAYOUT_TOID(spmo_test, struct dummy);
POBJ_LAYOUT_END(spmo_test);

struct root {
	TOID(struct dummy) obj;
};

struct dummy {
	uint64_t x[2];
};

int main()
{
	unlink("spmo_test.pool");
	PMEMobjpool* pool = pmemobj_create("spmo_test.pool", "spmo_test", 32*1024*1024, 0660);
	assert(pool != NULL);

	PMEMoid proot_ = pmemobj_root(pool, sizeof(struct root));
	if (OID_IS_NULL(proot_))
		abort();
	struct root* proot = (struct root*)pmemobj_direct(proot_);
	if(POBJ_NEW(pool, &proot->obj, struct dummy, NULL, NULL))
		abort();
	
	D_RW(proot->obj)->x[0] = 1;
	D_RW(proot->obj)->x[1] = 2;
	print_pass_flag();
	D_RW(proot->obj)->x[2] = 3; // This line should crash
	print_fail_flag();
	
	pmemobj_close(pool);
	return 0;
}
