/*
 * File: elfp_basic_api.c
 *
 * Description: Implements the 4 basic API
 * 	> elfp_init
 * 	> elfp_open
 * 	> elfp_close
 * 	> elfp_fini
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

#include "./include/elfp_err.h"
#include "./include/elfp_int.h"
#include "./include/elfp.h"

/*
 * Refer elfp.h for functions' description.
 */

int
elfp_init()
{
	int ret;

	/* Initialize main_vec */
	ret = elfp_main_vec_init();
	if(ret == -1)
	{
		elfp_err_warn("elfp_init", "elfp_main_vec_init() failed");
		return -1;
	}

	return 0;
}

int
elfp_open(const char *elfp_elf_path)
{
	/* Basic check */
	if(elfp_elf_path == NULL)
	{
		elfp_err_warn("elfp_open", "NULL argument passed");
		return -1;
	}

	int ret;
	int handle;
	elfp_main *main = NULL;

	/* Create the elfp_main object */
	main = elfp_main_create(elfp_elf_path);
	if(main == NULL)
	{
		elfp_err_warn("elfp_open", "elfp_main_create() failed");
		return -1;
	}

	/* Once created, it needs to be added to main_vec */
	ret = elfp_main_vec_add(main);
	if(ret == -1)
	{
		elfp_err_warn("elfp_open", "elfp_main_vec_add() failed");
		return -1;
	}

	handle = ret;

	/* Update the handle */
	elfp_main_update_handle(main, handle);

	/* At this point, the file is opened, elfp_main object
	 * for that file is created and is added into the vector.
	 * We also have the handle. 
	 * 
	 * At this point, the library should be able to handle
	 * any other parse requests.
	 */
	return handle;
}

int
elfp_close(int handle)
{
	
	if(elfp_sanitize_handle(handle) == -1)
	{
		elfp_err_warn("elfp_close", "elfp_sanitize_handle() failed");
		return -1;
	}

	elfp_main *main = NULL;
	int ret;
	
	/* Get the elfp_main object corresponding to the handle */
	main = elfp_main_vec_get_em(handle);
	
	/* If it has already been freed, then its cool! */
	if(main == NULL)
		return 0;

	/* Else, let us free it now */
	ret = elfp_main_fini(main);
	if(ret == -1)
	{
		elfp_err_warn("elfp_close", "elfp_main_fini() failed");
		return -1;
	}

	/* All good */
	return 0;
}

void
elfp_fini()
{
	elfp_main_vec_fini();
}
