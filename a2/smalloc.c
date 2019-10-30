#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"



void *mem;
struct block *freelist;
struct block *allocated_list;


void *smalloc(unsigned int nbytes) {
	//TODO

	//adjust nbytes to be divisible by 8
	if (nbytes % 8 != 0){
	    nbytes = ((nbytes/ 8) + 1) * 8;
	}

	//free_temp points to the block in freelist where memory spaces will be taken from
	struct block *free_temp = find_free_space(nbytes, freelist);

	/*Take memory space from where free_temp points to and add it to allocated_list
	* print to standard error if there is no enough space in freelist to be reserved
	*/
	if (free_temp != NULL) {
        allocated_list = allocate_memory(nbytes, allocated_list, free_temp);
        free_temp->addr += nbytes;
        free_temp->size -= nbytes;
        return allocated_list->addr;
    } else {
	    fprintf(stderr, "Not enough space for new block of size %u\n", nbytes);
        return NULL;
    }
}

/* Helper function for smalloc: used to take memory space from where free_temp points to and add it to allocated list.
 * Return the new allocated list.
 */
 struct block *allocate_memory(unsigned int nbytes, struct block *allocated_list, struct block *free_temp) {
    if (allocated_list == NULL) {
        allocated_list = malloc(sizeof(struct block));
        allocated_list->addr = free_temp->addr;
        allocated_list->size = nbytes;
    } else {
        struct block *new_node  = malloc(sizeof(struct block));
        new_node->size = nbytes;
        new_node->addr = ((allocated_list->addr) + nbytes);
        new_node->next = allocated_list;
        allocated_list = new_node;
    }
    return allocated_list;
}

/* Helper function for smalloc: used to find the first block in freelist that contains enough memory space
 * as stated in nbytes.
 * Return the block.
 */
struct block *find_free_space(unsigned int nbytes, struct block *free) {
    struct block *freetemp = free;
    while ((freetemp != NULL) && (freetemp->size < nbytes)) {
        freetemp = freetemp->next;
    }
    return freetemp;
}


int sfree(void *addr) {
	//TODO

	/* Find the block in allocated_list that is needed to be free.
	 * temp_temp_block is the pointer to block needed to be free.
	 * temp_block is initialized as NULL; if it has value other than NULL, it's next is temp_temp_block.
	 */
	struct block *temp_block = NULL;
    struct block *temp_temp_block = allocated_list;
    while ((temp_temp_block->addr != addr) && (temp_temp_block != NULL)) {
        temp_block = temp_temp_block;
        temp_temp_block = temp_block->next;
    }

    /* "Delete" block pointed by temp_temp_block in allocated_list, and add it to freelist
     * Return 0 if we successfully find the block and free it.
    */
	if (temp_temp_block != NULL) {
	    if (temp_block == NULL) {
	        allocated_list = temp_temp_block -> next;
	    } else {
            temp_block->next = temp_temp_block->next;
        }
	    freelist = insert_into_free(temp_temp_block, freelist);
	    return 0;
	}
    return -1;
}

/* Helper function for sfree: used to insert the block needed to be free into freelist.
 * temp_temp_block is pointer to the block needed to be free.
 * free_temp_block is initialized as NULL; if it has value other than NULL, then it's the block
 * whose next is free_temp_temp_block.
 * free_temp_temp_block is the value what temp_temp_block's next should be.
 * Return the modified freelist.
*/
struct block *insert_into_free(struct block *temp_temp_block, struct block *freelist) {
    // find the right place to insert the block so that freelist has blocks in increasing order of memory.
    struct block *free_temp_block = NULL;
    struct block *free_temp_temp_block = freelist;
    while ((free_temp_temp_block->addr) < (temp_temp_block->addr)) {
        free_temp_block = free_temp_temp_block;
        free_temp_temp_block = free_temp_temp_block->next;
    }
    // insert temp_temp_block in freelist
    if (free_temp_block == NULL) {
        temp_temp_block->next = freelist;
        freelist = temp_temp_block;
    } else {
        free_temp_block->next = temp_temp_block;
        temp_temp_block->next = free_temp_temp_block;
    }
    return freelist;
}


/* Initialize the memory space used by smalloc,
 * freelist, and allocated_list
 * Note:  mmap is a system call that has a wide variety of uses.  In our
 * case we are using it to allocate a large region of memory. 
 * - mmap returns a pointer to the allocated memory
 * Arguments:
 * - NULL: a suggestion for where to place the memory. We will let the 
 *         system decide where to place the memory.
 * - PROT_READ | PROT_WRITE: we will use the memory for both reading
 *         and writing.
 * - MAP_PRIVATE | MAP_ANON: the memory is just for this process, and 
 *         is not associated with a file.
 * - -1: because this memory is not associated with a file, the file 
 *         descriptor argument is set to -1
 * - 0: only used if the address space is associated with a file.
 */
void mem_init(int size) {
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
    }

	//TODO
	freelist = malloc(sizeof(struct block));
    freelist->addr = mem;
    freelist->size = size;
    freelist->next = NULL;
    allocated_list = NULL;
}

void mem_clean(){

	//TODO
	//create a temp block for later use.
	struct block *temp_block;

	// free blocks in allocated_list
	while (allocated_list != NULL) {
        temp_block = allocated_list->next;
	    free(allocated_list);
	    allocated_list = temp_block;
	}
	// free blocks in freelist
	while (freelist != NULL) {
	    temp_block = freelist->next;
	    free(freelist);
	    freelist = temp_block;
	}
}

