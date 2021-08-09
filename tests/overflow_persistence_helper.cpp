#include <libpmemobj.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include "common.h"

POBJ_LAYOUT_BEGIN(spmo_test);
POBJ_LAYOUT_ROOT(spmo_test, struct root);
POBJ_LAYOUT_END(spmo_test);

struct root {
	uint64_t arr[2];
};

void createPool() {
	unlink("spmo_test.pool");
	PMEMobjpool* pool = pmemobj_create("spmo_test.pool", "spmo_test", 32*1024*1024, 0660);
	assert(pool != NULL);
	
	PMEMoid proot_ = pmemobj_root(pool, sizeof(struct root));
	assert(OID_IS_NULL(proot_) == false);

	pmemobj_close(pool);
}

void attemptOverflow() {
	PMEMobjpool* pool = pmemobj_open("spmo_test.pool", "spmo_test");
	assert(pool != NULL);
	
	PMEMoid proot_ = pmemobj_root(pool, sizeof(struct root));
	assert(OID_IS_NULL(proot_) == false);
	struct root* proot = (struct root*)pmemobj_direct(proot_);

	proot->arr[0] = 1;
	proot->arr[1] = 1;
	print_pass_flag();
	proot->arr[2] = 1; // This line should crash
	print_fail_flag();
	
	pmemobj_close(pool);
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		std::cerr << "Usage: overflow_persistence_helper <stage: 1 or 2>" << std::endl;
		return 1;
	}
	if (strcmp(argv[1], "1") == 0) {
		createPool();
	}
	else {
		attemptOverflow();
	}

	return 0;
}
