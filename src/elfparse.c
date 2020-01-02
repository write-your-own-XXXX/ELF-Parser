/*
 * File: elfparse.c
 *
 * Description: Sample ELF parsing application.
 *
 * 1. How to build this?
 * 	* In 'src' directory, run "make app". Check 'src/app' directory.
 *
 * License: 
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                  Version 2, December 2004
 *  
 * Copyright (C) 2019 Adwaith Gautham <adwait.gautham@gmail.com>
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 *          DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 * TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 * 0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#include <stdio.h>
#include <elfp/elfp.h>

int 
main(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stdout, "Usage: $ %s <elf-file-path>\n", argv[0]);
		return -1;
	}

	int ret;
	int fd;
	const char *path = argv[1];
	
	/* Initialize the library */
	ret = elfp_init();
	if(ret == -1)
	{
		fprintf(stderr, "Unable to initialize libelfp.\nExiting..\n");
		return -1;
	}

	/* Open up the ELF file for processing */
	ret = elfp_open(path);
	if(ret == -1)
	{
		fprintf(stderr, "Unable to open file using libelfp.\nExiting..\n");
		elfp_fini();
		return -1;
	}

	fd = ret;
	
	/* Dump the ELF header */
	elfp_ehdr_dump(fd);

	/* Dump the PHT */
	elfp_pht_dump(fd);
 
	/* Let us dump the INTERP segment */
	ret = elfp_seg_dump(fd, "INTERP");
	if(ret == -1)
	{
		fprintf(stderr, "elfp_seg_dump() failed\n");
		elfp_fini();
		return -1;
	}

	ret = elfp_seg_dump(fd, "GNU_STACK");
	if(ret == -1)
	{
		fprintf(stderr, "elfp_seg_dump() failed\n");
		elfp_fini();
		return -1;
	}

	/* Close this file */
	elfp_close(fd);

	/* De-init the library */
	elfp_fini();

	return 0;
}
