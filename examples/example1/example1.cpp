#include <libpmemobj_secure.h>
#include <iostream>
#include <assert.h>

int main()
{
	PMEMobjpool* pool = spmo::spmemobj_create("pool", "example1", 8*1024*1024, 0660);
	assert(pool != NULL);
	spmo::spmemobj_close(pool);
	return 0;
}
