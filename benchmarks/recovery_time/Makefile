GCC=gcc
CLG=clang
# Add debug info and enable ASan
ifndef NO_ASAN
CFLAGS=-g -fsanitize=address
else
CFLAGS=-g
endif

all: build/recovery_run

build/recovery_run: ./source/recovery_run.c
	mkdir -p ./build
	${GCC} ${CFLAGS} ./source/recovery_run.c -o ./build/recovery_run -lpmemobj -lpmem

clean:
	rm -f ./build/*
