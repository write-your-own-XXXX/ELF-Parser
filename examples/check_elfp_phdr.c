/*
 * File: check_elfp_phdr.c
 *
 * Description: To test elfp_phdr's API: elfp_phdr_dump()
 *
 * Compilation:
 * 	1. Install the library using "make install"
 * 	2. Do "make examples" in 'src' directory.
 *
 * Result: It prints all the Program Headers present.
 */

#include <stdio.h>

#include "../src/include/elfp.h"
#include "../src/include/elfp_err.h"

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stdout, "Usage: $ %s <elf-file-path>\n", argv[0]);
		return -1;
	}

	int ret;
	const char *path = argv[1];
	int fd;

	/* Init the library */
	ret = elfp_init();
	if(ret == -1)
	{
		elfp_err_exit("main", "elfp_init() failed");
	}
	
	/* Lets open up the file */
	ret = elfp_open(path);
	if(ret == -1)
	{
		elfp_err_exit("main", "elfp_open() failed");
	}
	fd = ret;

	/* Since we don't know the number of Program Headers present, 
	 * we'll iterate from 1-50. */
	for(unsigned i = 0; i < 50; i++)
	{
		printf("Entry %02u: \n", i);
		elfp_phdr_dump(fd, i);
	}
	
	/* Close the library */
	elfp_fini();

	return 0;
}




