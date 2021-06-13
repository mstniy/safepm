#ifndef PMDK_ASAN_COMMON_H
#define PMDK_ASAN_COMMON_H

#include <iostream>

inline void print_pass_flag()
{
	std::cerr << "PMDK_ASAN_PASS_FLAG" << std::endl;
}

inline void print_fail_flag()
{
	std::cerr << "PMDK_ASAN_FAIL_FLAG" << std::endl;
}

#endif
