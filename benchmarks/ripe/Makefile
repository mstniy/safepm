GCC=gcc
CLG=clang
# Add debug info and enable ASan
ifndef NO_ASAN
CFLAGS=-g -fsanitize=address
else
CFLAGS=-g
endif

all: build/gcc_attack_gen_pmemobj_heap build/clang_attack_gen_pmemobj_heap build/gcc_attack_gen_system_heap build/clang_attack_gen_system_heap

build/gcc_attack_gen_pmemobj_heap: ./source/attack_gen_pmemobj_heap.c
	${GCC} ${CFLAGS} ./source/attack_gen_pmemobj_heap.c -o ./build/gcc_attack_gen_pmemobj_heap -lpmemobj

build/clang_attack_gen_pmemobj_heap: ./source/attack_gen_pmemobj_heap.c
	${CLG} ${CFLAGS} ./source/attack_gen_pmemobj_heap.c -o ./build/clang_attack_gen_pmemobj_heap -lpmemobj
	
build/gcc_attack_gen_system_heap: ./source/attack_gen_system_heap.c
	${GCC} ${CFLAGS} ./source/attack_gen_system_heap.c -o ./build/gcc_attack_gen_system_heap

build/clang_attack_gen_system_heap: ./source/attack_gen_system_heap.c
	${CLG} ${CFLAGS} ./source/attack_gen_system_heap.c -o ./build/clang_attack_gen_system_heap

clean:
	rm ./build/*
