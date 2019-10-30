
/* Remember that header files must not contain code or declaration of 
 * variables that would require memory to be allocated.
 */


struct block {
    void *addr; /*start address of memory for this block */
    int size;
    struct block *next;
};

/****************************************************************************/
/* Implemented in smalloc.c */

/* Allocates size bytes of memory for the dynamically allocated memory 
 * algorithm to use */
void mem_init(int size);

/* Reserves nbytes of space from the memory region created by mem_init.  Returns
 * a pointer to the reserved memory. Returns NULL if memory cannot be allocated */    
void *smalloc(unsigned int nbytes);

/* Free the reserved space starting at addr.  Returns 0 if successful 
 * -1 if the address cannot be found in the list of allocated blocks */
int sfree(void *addr);

/* Free any dynamically used memory in the allocated and free list */
void mem_clean();

/* Helper function for smalloc - used to allocate memory for allocated_list*/
struct block *allocate_memory(unsigned int nbytes, struct block *allocated_list, struct block *free_temp);

/* Helper function for smalloc - find the block in freelist which has enough space for allocation*/
struct block *find_free_space(unsigned int nbytes, struct block *free);

/* Helper function for sfree*/
struct block *insert_into_free(struct block *temp_temp_block, struct block *freelist);

/****************************************************************************/
/* Implemented in testhelpers.c */
/* The remaining functions are for testing purposes*/

/* prints the list of allocated blocks */
void print_allocated(void);

/* prints the list of free blocks */
void print_free(void);

/* A function that writes to the memory allocated by smalloc */
void write_to_mem(int size, char *ptr, char value);

/* prints the contents of allocated memory */
void print_mem(void);
