/*

Tool to empirically measure the space overhead of pmemobj, as well as the safepm (if linked against the safepm fork)
Set POOL_SIZE to the size of the pool to be created and run to measure the user-available area in the pool.

*/

#include <libpmemobj.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>

bool test(PMEMobjpool* pool, size_t size) {
	volatile bool success = true; // Volatile because set inside ONABORT
	TX_BEGIN(pool) {
		PMEMoid ptr = pmemobj_tx_alloc(size, 1);
		pmemobj_tx_free(ptr);
	}
	TX_ONABORT {
		success = false;
	}
	TX_ONCOMMIT {
		// Nothing
	}
	TX_END
	
	return success;
}

int main()
{
	if (NULL == getenv("POOL_SIZE")) {
		std::cerr << "Please set POOL_SIZE first." << std::endl;
		return 1;
	}

	unlink("spmo_test.pool");
	PMEMobjpool* pool = pmemobj_create("spmo_test.pool", "spmo_test", atoi(getenv("POOL_SIZE")), 0660);
	assert(pool != NULL);
	
	size_t lb=1;
	size_t ub=lb;
	
	while (1) {
		if (ub > (1ULL<<48)) {
			std::cout << "Could not determine an upper limit for the user-available area" << std::endl;
			return 1;
		}
		if (! test(pool, ub))
			break;
		ub *= 2;
	}
	
	while ((ub-lb) > lb/1000) { // An error tolerance of .1%
		size_t middle = (lb+ub)/2;
		if (test(pool, middle)) {
			lb=middle+1;
		}
		else {
			ub=middle;
		}
	}
	
	std::cout << "Estimated user-available area: " << lb << " bytes." << std::endl;
	
	pmemobj_close(pool);
	return 0;
}
