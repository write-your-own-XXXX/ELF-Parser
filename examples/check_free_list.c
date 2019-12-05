/*
 * File: check_free_list.c
 *
 * Description: 
 *
 * 1. This is a simple demonstration of the elfp_free_addr_vector structure.
 * 2. Compile this in the following manner.
 * 	$ gcc check_free_list.c ../src/elfp_int.c -o free_list
 *
 * 3. Main idea is to make sure the functions (init, add, fini) are 
 * 	implemented properly.
 *
 * 4. To make sure there are no memory leaks, we'll use valgrind to test it.
 *	$ valgrind ./free_list
 *
 * 5. For $ valgrind ./free_list 3599
 * 	a. count = 3599
 * 	b. total = 4000
 * 	c. All addresses were listed
 * 	d. Valgrind: All heap blocks were freed - No leaks are possible.
 */

#include <stdio.h>
#include <stdlib.h>

#include "../src/include/elfp_int.h"
#include "../src/include/elfp_err.h"

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stdout, "Usage: $ %s <Number of Addresses>\n", argv[0]);
		return -1;
	}

	elfp_free_addr_vector vec;
	int ret;
	void *addr = NULL;
	unsigned long int n_addrs;
	unsigned long int i;

	n_addrs = atoi(argv[1]);

	/* Initialize it */
	ret = elfp_free_addr_vector_init(&vec);
	if(ret != 0)
	{
		elfp_err_exit("main", "elfp_free_addr_vector_init() failed");
	}

	for(i = 0; i < n_addrs; i++)
	{	
		addr = calloc(1, 1);
		if(addr == NULL)
		{
			elfp_err_warn("main", "calloc() failed");
			break;
		}

		/* Add it to the list */
		ret = elfp_free_addr_vector_add(&vec, addr);
		if(ret != 0)
		{
			elfp_err_warn("main", "elfp_free_addr_vector_add() failed");
			break;
		}
	}

	/* Check vec's members */
	printf("total = %lu\n", vec.total);
	printf("count = %lu\n", vec.count);
	for(i = 0; i < vec.count; i++)
	{
		printf("%lu. %p\n", i, vec.addrs[i]);
	}

	/* Free it all */
	elfp_free_addr_vector_fini(&vec);
	
	return 0;
}
