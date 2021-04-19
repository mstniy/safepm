cmake_minimum_required (VERSION 3.1)

add_executable(example1 example1.cpp)
add_dependencies(example1 spmo)
target_compile_options(example1 PRIVATE "-fsanitize=address")
target_link_options(example1 PRIVATE "-fsanitize=address")
target_link_libraries(example1 spmo)
