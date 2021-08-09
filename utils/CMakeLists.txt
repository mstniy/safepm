cmake_minimum_required (VERSION 3.1)

function (spmo_util target)
	add_executable("${target}.exe" "${target}.cpp")
	target_compile_options("${target}.exe" PRIVATE "-fsanitize=address" -ggdb)
	target_link_options("${target}.exe" PRIVATE "-fsanitize=address" -ggdb)
	target_link_libraries("${target}.exe" libpmemobj libpmem ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ndctl daxctl)
	target_include_directories("${target}.exe" PUBLIC "${PMDK_INSTALL_DIR}/include")
endfunction()

spmo_util(pool_overhead)
