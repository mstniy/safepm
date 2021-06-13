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
	uint64_t x;
};

int main()
{
	unlink("spmo_test.pool");
	PMEMobjpool* pool = pmemobj_create("spmo_test.pool", "spmo_test", 32*1024*1024, 0660);
	assert(pool != NULL);

	PMEMoid proot_ = pmemobj_root(pool, sizeof(struct root));
	assert(OID_IS_NULL(proot_) == false);
	struct root* proot = (struct root*)pmemobj_direct(proot_);
	assert(0 == POBJ_NEW(pool, &proot->obj, struct dummy, NULL, NULL));
	TOID(struct dummy) toid = proot->obj;
	POBJ_FREE(&proot->obj);
	print_pass_flag();
	POBJ_FREE(&toid); // This line should crash. The first FREE call sets proot->obj to NULL, so we free the other copy here.
	print_fail_flag();
	
	pmemobj_close(pool);
	return 0;
}
