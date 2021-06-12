#include <libpmemobj.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>

POBJ_LAYOUT_BEGIN(spmo_test);
POBJ_LAYOUT_TOID(spmo_test, struct dummy);
POBJ_LAYOUT_END(spmo_test);

struct dummy {
	uint64_t x[1024];
};

int main()
{
	unlink("spmo_test.pool");
	PMEMobjpool* pool = pmemobj_create("spmo_test.pool", "spmo_test", 10*1024*1024, 0660);
	assert(pool != NULL);
	
	TOID(struct dummy) ptr;
	
	for (int i=0; i<1300; i++) {

		TX_BEGIN(pool) {
			ptr = TX_ZNEW(struct dummy);
		} TX_ONABORT {
			std::cerr << "Faild to allocate a dummy object" << std::endl;
			abort();
		}
		TX_END
		
		TX_BEGIN(pool) {
			TX_FREE(ptr);
		} TX_ONABORT {
			std::cerr << "Faild to free the dummy object" << std::endl;
			abort();
		}
		TX_END
	
	}
	
	pmemobj_close(pool);
	return 0;
}
