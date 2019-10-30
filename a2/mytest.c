#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"


#define SIZE 2 * 64


/* My test for smalloc to see if smalloc will continue allocating memories if there is no enough space.
 * (That's why SIZE is defined as 2*64 = 128 here. A smaller size of freelist makes it easier for testing.)
 * If smalloc stop allocation when there is not enough space, an error message like "Not enough space for
 * new block of size 56" is printed to standard error.
 * And it is also a general test for smalloc and sfree to see if they work correctly.
 * (This is done by free one more block from allocated_list than in simpletest, to see how will sfree do with
 * that block. After that, a request of adding new block of same size as the one of what is just been free is
 * sent to smalloc, to see how smalloc will find the correct place of free space and reallocate the spaces.)
 */

int main(void) {

    mem_init(SIZE);

    char *ptrs[10];
    int i;

    /* Call smalloc 5 times, it should not have enough space for allocation in the 5th time.*/
    for(i = 0; i < 5; i++) {
        int num_bytes = (i+1) * 10;
        ptrs[i] = smalloc(num_bytes);
        if (ptrs[i] != NULL) {
            write_to_mem(num_bytes, ptrs[i], i);
        }
    }

    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();

    /* Free space for the second block in ptrs. A new block should be added to the top of freelist.*/
    printf("freeing %p result = %d\n", ptrs[1], sfree(ptrs[1]));

    /* Free space for the third block in ptrs. A new block should be inserted after the top of freelist.*/
    printf("freeing %p result = %d\n", ptrs[2], sfree(ptrs[2]));

    /* Allocate space for same size as the third block in ptrs. If smalloc and sfree runs perfectly,
     * then the size of second block in freelist should turns 0 and a new block of size 32 is added
     * to allocated_list*/
    ptrs[5] = smalloc(32);
    if (ptrs[5] != NULL) {
        write_to_mem(32, ptrs[5], 5);
    }



    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();

    mem_clean();
    return 0;
}
