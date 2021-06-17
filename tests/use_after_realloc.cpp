#include <libpmemobj.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include "common.h"

POBJ_LAYOUT_BEGIN(spmo_test);
POBJ_LAYOUT_TOID(spmo_test, struct dummy);
POBJ_LAYOUT_END(spmo_test);

struct dummy {
	uint64_t x[2];
};

int main()
{
	unlink("spmo_test.pool");
	PMEMobjpool* pool = pmemobj_create("spmo_test.pool", "spmo_test", 32*1024*1024, 0660);
	assert(pool != NULL);
	
	PMEMoid oid1, oid2;

	TX_BEGIN(pool) {
		oid1 = pmemobj_tx_alloc(sizeof(struct dummy), TOID_TYPE_NUM(struct dummy));
	} TX_ONABORT {
		std::cerr << "Failed to allocate a dummy object" << std::endl;
		abort();
	}
	TX_END
	
	TX_BEGIN(pool) {
		oid2 = pmemobj_tx_realloc(oid1, sizeof(struct dummy)*300, TOID_TYPE_NUM(struct dummy));
		if (oid2.off == oid1.off) {
			std::cerr << "realloc did not move the object. Test inconclusive." << std::endl;
			abort();
		}

		((struct dummy*)pmemobj_direct(oid2))->x[0] = 1;
		print_pass_flag();
		((struct dummy*)pmemobj_direct(oid1))->x[0] = 1; // This line should crash
		print_fail_flag();
	} TX_ONABORT {
		std::cerr << "Failed to reallocate the dummy object" << std::endl;
		print_fail_flag();
		abort();
	} TX_END
	
	pmemobj_close(pool);
	return 0;
}
