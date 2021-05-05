cmake_minimum_required (VERSION 3.1)

function (spmo_test target)
	add_executable("${target}" "${target}.cpp")
	add_dependencies("${target}" spmo)
	target_compile_options("${target}" PRIVATE "-fsanitize=address")
	target_link_options("${target}" PRIVATE "-fsanitize=address")
	target_link_libraries("${target}" spmo)
endfunction()

spmo_test(double_free)
spmo_test(use_after_free)
spmo_test(mismatched_free)
