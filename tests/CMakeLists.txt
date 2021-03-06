cmake_minimum_required (VERSION 3.1)

function (spmo_test target)
	add_executable("${target}.exe" "${target}.cpp")
	target_compile_options("${target}.exe" PRIVATE "-fsanitize=address" -ggdb)
	target_link_options("${target}.exe" PRIVATE "-fsanitize=address" -ggdb)
	target_link_libraries("${target}.exe" libpmemobj libpmem ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ndctl daxctl)
	target_include_directories("${target}.exe" PUBLIC "${PMDK_INSTALL_DIR}/include")
endfunction()

spmo_test(double_free)
spmo_test(double_free_ntx)
spmo_test(use_after_free)
spmo_test(use_after_free_ntx)
spmo_test(use_after_realloc)
spmo_test(mismatched_free)
spmo_test(overflow)
spmo_test(tx_add_overflow)
spmo_test(overflow_ntx)
spmo_test(root_overflow)
spmo_test(root_underflow)
spmo_test(int32)
spmo_test(alloc_tx_abort)
spmo_test(free_tx_abort)
spmo_test(zalloc)
spmo_test(overflow_persistence_helper)
