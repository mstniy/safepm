#include <libpmemobj.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include "common.h"

POBJ_LAYOUT_BEGIN(spmo_test);
POBJ_LAYOUT_ROOT(spmo_test, struct root);
POBJ_LAYOUT_END(spmo_test);

struct root {
	uint64_t x[3];
};

int main()
{
	unlink("spmo_test.pool");
	PMEMobjpool* pool = pmemobj_create("spmo_test.pool", "spmo_test", 32*1024*1024, 0660);
	assert(pool != NULL);

	PMEMoid proot_ = pmemobj_root(pool, sizeof(struct root));
	assert(OID_IS_NULL(proot_) == false);
	struct root* proot = (struct root*)pmemobj_direct(proot_);

	TX_BEGIN(pool) {
		pmemobj_tx_add_range_direct(proot, 24);
		print_pass_flag();
		pmemobj_tx_add_range_direct(proot, 25); // This line should crash
	} TX_END

	print_fail_flag();

	pmemobj_close(pool);
	return 0;
}
