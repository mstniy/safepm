cmake_minimum_required (VERSION 3.1)

function (spmo_test target)
	add_executable("${target}.exe" "${target}.cpp")
	add_dependencies("${target}.exe" spmo)
	target_compile_options("${target}.exe" PRIVATE "-fsanitize=address")
	target_link_options("${target}.exe" PRIVATE "-fsanitize=address")
	target_link_libraries("${target}.exe" spmo)
endfunction()

spmo_test(double_free)
spmo_test(use_after_free)
spmo_test(mismatched_free)
spmo_test(overflow)
