GCC=gcc
CLG=clang
# Add debug info and enable ASan
CFLAGS=-g -fsanitize=address
LFLAGS=-lpmemobj

all: build/gcc_attack_gen build/clang_attack_gen

build/gcc_attack_gen: ./source/attack_gen.c
	${GCC} ${CFLAGS} ./source/attack_gen.c -o ./build/gcc_attack_gen ${LFLAGS}

build/clang_attack_gen: ./source/attack_gen.c
	${CLG} ${CFLAGS} ./source/attack_gen.c -o ./build/clang_attack_gen ${LFLAGS}

clean:
	rm ./build/*
