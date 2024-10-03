#include <lib/debug.h>
#include <lib/x86.h>
#include "import.h"

// NOTE these defines are for removing code errors
#define NUM_IDS      64
#define MagicNumber  1048577
#define MAX_CHILDREN 3

// NOTE this is a object blue print for the container
struct SContainer {
    int quota;      // maximum memory quota of the process
    int usage;      // the current memory usage of the process
    int parent;     // the id of the parent process
    int nchildren;  // the number of child processes
    int used;       // whether current container is used by a process
};

// mCertiKOS supports up to NUM_IDS processes
static struct SContainer CONTAINER[NUM_IDS]; // NOTE this section creates an array of containers

/**
 * Initializes the container data for the root process (the one with index 0).
 * The root process is the one that gets spawned first by the kernel.
 */
void container_init(unsigned int mbi_addr)
{
    unsigned int real_quota;
    // TODO: define your local variables here.
    unsigned int numberOfPages;
    unsigned int i;

    pmem_init(mbi_addr);
    real_quota = 0;

    numberOfPages = get_nps();

    /**
     * TODO: Compute the available quota and store it into the variable real_quota.
     * It should be the number of the unallocated pages with the normal permission
     * in the physical memory allocation table.
     */

    // NOTE this loop goes through all the pages and checks if the page is normal and not allocated
    for(i = 0; i < numberOfPages; i++){
        if(at_is_norm(i) && !at_is_allocated(i)){
            real_quota++;
        }
    }

    KERN_DEBUG("\nreal quota: %d\n\n", real_quota);

    // NOTE : this section initializes the root container which is for karnel
    CONTAINER[0].quota = real_quota;
    CONTAINER[0].usage = 0;
    CONTAINER[0].parent = 0;
    CONTAINER[0].nchildren = 0;
    CONTAINER[0].used = 1;
}

// Get the id of parent process of process # [id].
unsigned int container_get_parent(unsigned int id)
{
    // TODO : return the parent of the process
    // NOTE this function returns the parent of the process
    return CONTAINER[id].parent;
}

// Get the number of children of process # [id].
unsigned int container_get_nchildren(unsigned int id)
{
    // TODO : return the number of children of the process
    // NOTE this function returns the number of children of the process
    return CONTAINER[id].nchildren;
}

// Get the maximum memory quota of process # [id].
unsigned int container_get_quota(unsigned int id)
{
    // TODO : return the quota of the process
    // NOTE this function returns the quota of the process
    return CONTAINER[id].quota;
}

// Get the current memory usage of process # [id].
unsigned int container_get_usage(unsigned int id)
{
    // TODO : return the usage of the process
    // NOTE this function returns the usage of the process
    return CONTAINER[id].usage;
}

// Determines whether the process # [id] can consume an extra
// [n] pages of memory. If so, returns 1, otherwise, returns 0.
unsigned int container_can_consume(unsigned int id, unsigned int n)
{
    // TODO : return 1 if the process can consume n pages of memory, otherwise return 0
    // NOTE this function checks if the process can consume n pages of memory
    return CONTAINER[id].usage + n <= CONTAINER[id].quota;
}

/** FIXME
 * Dedicates [quota] pages of memory for a new child process.
 * You can assume it is safe to allocate [quota] pages
 * (the check is already done outside before calling this function).
 * Returns the container index for the new child process.
 */
unsigned int container_split(unsigned int id, unsigned int quota)
{
    unsigned int child, numberOfChildren;

    numberOfChildren = CONTAINER[id].nchildren;
    child = id * MAX_CHILDREN + 1 + numberOfChildren;  // container index for the child process

    if (NUM_IDS <= child) {
        return NUM_IDS;
    }

    /**
     * TODO: Update the container structure of both parent and child process appropriately.
     */
    CONTAINER[id].nchildren++;
    CONTAINER[id].usage += quota;

    CONTAINER[child].quota = quota;
    CONTAINER[child].usage = 0;
    CONTAINER[child].parent = id;
    CONTAINER[child].nchildren = 0;
    CONTAINER[child].used = 1;

    return child;
}

/**
 * Allocates one more page for process # [id], given that this will not exceed the quota.
 * The container structure should be updated accordingly after the allocation.
 * Returns the page index of the allocated page, or 0 in the case of failure.
 */
unsigned int container_alloc(unsigned int id)
{
    /*
     * TODO: Implement the function here.
     */

    unsigned int pid = palloc();
    if (pid == 0) {
        return 0;
    }

    CONTAINER[id].usage++;
    return pid;
}

// Frees the physical page and reduces the usage by 1.
void container_free(unsigned int id, unsigned int page_index)
{
    // TODO
    pfree(page_index);
    CONTAINER[id].usage--;
}
