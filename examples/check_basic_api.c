/*
 * File: check_basic_api.c
 *
 * Description:
 *
 * 1. Uses the 4 basic API defined in elfp_basic_api.c and makes
 * sure they are functioning properly.
 *
 * 2. Go to ELF-Parser/src and run "$ make examples". Check the examples/build   
 *      directory.
 * 
 * 3. NOTE: I realized that there is a upper limit on the number of files a process can open.
 *
 * 4. The API works fine. Valgrind didn't detect memory leaks for various runs.
 */

#include <stdio.h>
#include <stdlib.h>

#include "../src/include/elfp_err.h"
#include "../src/include/elfp.h"
#include "../src/include/elfp_int.h"

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stdout, "Usage: $ %s <Number of ELF files>\n", argv[0]);
		return -1;
	}

	int ret;
	int n_elfs = atoi(argv[1]);
	int handle[n_elfs];
	unsigned long int i;
	const char *file_path = "/bin/bash";
	int h;

	/* Initialize the library */
	ret = elfp_init();
	if(ret == -1)
	{
		elfp_err_exit("main", "elfp_init() failed");
	}

	/* First test the happy path */
	for(i = 0; i < n_elfs; i++)
	{
		handle[i] = elfp_open(file_path);
		if(handle[i] == -1)
		{
			elfp_err_warn("main", "elfp_open() failed");
			break;
		}
		printf("handle[%lu] = %d\n", i, handle[i]);
	}
	
	/* Let us close a few files */
	for(i = 0; i < n_elfs; i = i + 2)
	{
		printf("Closing handle %d\n", handle[i]);
		ret = elfp_close(handle[i]);
		if(ret == -1)
		{
			elfp_err_warn("main", "elfp_close() failed");
		}
	}

	/* Finally de-init the library */
	elfp_fini();

	return 0;
}
