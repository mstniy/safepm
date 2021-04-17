#include <libpmemobj_secure.h>
#include <iostream>

int main()
{
	spmo::spmemobj_create("pool", "example1", 8*1024*1024, 0660);
	return 0;
}
