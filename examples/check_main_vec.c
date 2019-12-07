/*
 * File: check_main_vec.c
 *
 * Description:
 *
 * 1. Program which tests the functionality of main_vec.
 *
 * 2. Go to ELF-Parser/src and run "$ make examples". Check the examples/build   
 *      directory.
 *
 * 3. Valgrind didn't detect any leaks.
 */


#include "../src/include/elfp_int.h"
#include "../src/include/elfp_err.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stdout, "Usage: $ %s <Number of objects>\n", argv[0]);
		return -1;
	}

	unsigned long int i;
	int handle;
	const char *file_path = "/bin/bash";
	elfp_main *main = NULL;

	/* Initialize the vector */
	elfp_main_vec_init();

	for(i = 0; i < atoi(argv[1]); i++)
	{
		main = elfp_main_create(file_path);
		if(main == NULL)
		{
			elfp_err_warn("main", "elfp_main_create() failed");
			break;
		}

		handle = elfp_main_vec_add(main);
		if(handle == -1)
		{
			elfp_err_warn("main", "elfp_main_vec_add() failed");
			break;
		}
		
		elfp_main_update_handle(main, handle);

		printf("handle = %d\n", main->handle);
	}

	/* Let us check the members */
	for(i = 0; i < main_vec.latest; i++)
	{
		printf("%lu. %p, %d, %p\n", i, main_vec.vec[i], main_vec.vec[i]->fd, main_vec.vec[i]->start_addr);
	}
	printf("total = %lu\n", main_vec.total);
	printf("latest = %lu\n", main_vec.latest);

	/* Free the vector in the end */
	elfp_main_vec_fini();

	return 0;
}












