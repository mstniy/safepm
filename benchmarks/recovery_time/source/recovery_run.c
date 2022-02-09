#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <libpmemobj.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

#define POOL_SIZE 10 * PMEMOBJ_MIN_POOL
#define MAX_BUF_LEN_ROOT 32 /* maximum length of our root buffer */
#define MAX_BUF_LEN_OBJ 1000 /* maximum length of our object buffer */
#define KB 1024
#define MB (1024 * 1024)

void *mmap_helper(size_t size) {
 	return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
}

void munmap_helper(void * addr, size_t size) {
	munmap(addr, size);
}

bool file_exists (char *filename) {
  struct stat buffer;   
  return (stat (filename, &buffer) == 0);
}

enum op {
    START_UP,
    RECOVERY,
    MAX_OP
};

struct my_root {
	size_t len; /* = strlen(buf) */
	char buf[MAX_BUF_LEN_ROOT];
	PMEMoid first_object;
};

struct my_object {
	size_t len; /* = strlen(buf) */
	char buf[MAX_BUF_LEN_OBJ];
	PMEMoid next_object;
};

/* Default values */
int ulog_size = 10;
int objects_num = 10;
int repeats = 1;
char *pool_path = "/mnt/recovery/arraypool";

int safe_remove(const char* path);
void print_recovery_results 
(int obj_number, uint64_t log_size, uint64_t pool_size, double time);

int main(int argc, char *argv[]){

    if (argc != 2) {
        printf("Usage: ./startup_recovery_test [ulog size (KB)]\n");
        exit(1);
    }

    ulog_size = atoi(argv[1]);
    objects_num = ulog_size;

    double total_time = 0;
	PMEMobjpool* pop = NULL;

    if (file_exists(pool_path)) {
        safe_remove(pool_path);
    }

    if ((pop = pmemobj_create(pool_path, POBJ_LAYOUT_NAME(recovery),
                                POOL_SIZE, (S_IWRITE | S_IREAD))) == NULL) {
        printf("failed to create pool\n");
        return 1;
    }        

    double time_taken = 0;
    struct timespec start, end;
    PMEMoid *oid;

    uint64_t snapshot_obj_size = 0; //sanity check var
    oid = (PMEMoid*)mmap_helper(objects_num * sizeof(PMEMoid));

    /* allocate objects_num number of objects */
    TX_BEGIN(pop) {
        for (int j = 0; j < objects_num; j++) {                    
            oid[j] = pmemobj_tx_alloc(sizeof(struct my_object), 0);
            void* temp = pmemobj_direct(oid[j]);
            assert(temp != NULL);
        }
    } TX_ONABORT{
        printf("transaction aborted\n ");
    } TX_END
    
    /* snapshot snapshot_obj number of objects each being 1Kb */
    pmemobj_tx_begin(pop, NULL, TX_PARAM_NONE);
    for (int i = 0; i < objects_num; i++) {
        if (pmemobj_tx_add_range(oid[i], 0, sizeof(struct my_object))!=0) {
            printf("error in adding range\n");
        }
        else {
            snapshot_obj_size += sizeof(struct my_object);
        }
    }
    assert(snapshot_obj_size == ulog_size * KB); //sanity check

    /* 
    * close the pool before the finalisation of a tx
    * to trigger recovery after the next open
    * The undo log apply can be verified by checking
    * src/libpmemobj/ulog.c:ulog_entry_apply in the
    * ULOG_OPERATION_BUF_SET & ULOG_OPERATION_BUF_CPY
    * operations.
    */
    pmemobj_close(pop);

    munmap_helper(oid, objects_num * sizeof(PMEMoid));

    clock_gettime(CLOCK_MONOTONIC, &start);

    if ((pop = pmemobj_open(pool_path, POBJ_LAYOUT_NAME(recovery)))
                            == NULL) {
        printf("failed to open pool\n");
        return 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    time_taken = (end.tv_sec - start.tv_sec) * 1e9;
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;

    total_time += time_taken;
        
    pmemobj_close(pop);
    
    print_recovery_results(objects_num, ulog_size * KB,
                        POOL_SIZE, total_time);

    return 0;
}

int safe_remove(const char* path) {
   int del = remove(path);
   if (del)
      printf("the file %s is not Deleted\n", path);
    return del;
}

void print_recovery_results
(int obj_number, uint64_t log_size, uint64_t pool_size, double time) {
    // header printing is moved to inner_run.sh
    // printf("number_of_objects;log_size(KB);pool_size(MB);recovery_time(s)\n"); 
    printf("%d;%.2lf;%.2lf;%f\n", 
            obj_number, 
            (double)log_size/(double)KB, 
            (double)pool_size/(double)MB,
            time);
}