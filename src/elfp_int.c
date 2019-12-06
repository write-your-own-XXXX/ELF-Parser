/*
 * File: elfp_int.c
 *
 * Description: Definitions to all functions declared in elfp_int.h
 *
 * 		* Internal to the tool. Users should not use these functions.
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

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/mman.h>

/*
 * All definitions related to elfp_free_addr_vector structure.
 *
 * Refer to elfp_int.h for structure definition and function description.
 */

int
elfp_free_addr_vector_init(elfp_free_addr_vector *vec)
{
	/* Basic check */
	if(vec == NULL)
	{
		elfp_err_warn("elfp_free_addr_vector_init", "NULL argument passed");
        	return -1;
    	}     

    	/* Total number of addresses it can hold at first is 1000 */
    	vec->total = ELFP_FREE_ADDR_VECTOR_INIT_SIZE;

	/* Allocate the same amount of memory */
	vec->addrs = calloc(vec->total, sizeof(void *));
	if(vec->addrs == NULL)
	{
		elfp_err_warn("elfp_free_addr_vector_init", "calloc() failed");
		return -1;
	}
	
	/* Init the count */
	vec->count = 0;

	/* Good to go! */
	return 0;
}

void
elfp_free_addr_vector_fini(elfp_free_addr_vector *vec)
{
	/* Basic check */
	if(vec == NULL)
	{
		elfp_err_warn("elfp_free_addr_vector_fini", "NULL argument passed");
		return;
	}

	/* First, free up all the stored addresses */
	for(unsigned long i = 0; i < vec->count; i++)
		free(vec->addrs[i]);

	/* Now, free up addrs */
	free(vec->addrs);
	
	/* All set */
	return;
}

int
elfp_free_addr_vector_add(elfp_free_addr_vector *vec, void *addr)
{
	/* Basic check */
	if(vec == NULL || addr == NULL)
	{
		elfp_err_warn("elfp_free_addr_vector_add", 
				"NULL argument(s) passed");
		return -1;
	}
	
	void *new_addr = NULL;

	/* Check if the vector is full */
	if(vec->count == vec->total)
	{
		/* Allocate more memory */
		new_addr = realloc(vec->addrs, 
			(vec->total + ELFP_FREE_ADDR_VECTOR_INIT_SIZE) * sizeof(void *));
		if(new_addr == NULL)
		{
			elfp_err_warn("elfp_free_addr_vector_add",
					"realloc() failed");
			return -1;
		}
		
		/* Initialize the new memory */
		memset(((char *)new_addr) + vec->total * sizeof(void *), '\0', 
				ELFP_FREE_ADDR_VECTOR_INIT_SIZE * sizeof(void *));

		vec->addrs = new_addr;
		vec->total = vec->total + ELFP_FREE_ADDR_VECTOR_INIT_SIZE;
	}

	/* Now, put the address into the list */
	vec->addrs[vec->count] = addr;
	vec->count = vec->count + 1;

	/* Good to go! */
	return 0;
}

/*
 * All definitions related to elfp_main structure.
 *
 * Refer elfp_int.h for structure definition and functions' description.
 */

elfp_main*
elfp_main_create(const char *file_path)
{
	/* Basic check */
	if(file_path == NULL)
	{
		elfp_err_warn("elfp_main_create", "NULL argument passed");
		return NULL;
	}

	int ret;
	elfp_main *main = NULL;
	struct stat st;
	unsigned char magic[4] = {'\0', '\0', '\0', '\0'};
	void *start_addr = NULL;

	/* Allocate memory */
	main = calloc(1, sizeof(elfp_main));
	if(main == NULL)
	{
		elfp_err_warn("elfp_main_create", "calloc() failed");
		return NULL;
	}

	/* All the elfp_main's members are 0 now.
	 * We'll start filling them one by one */

	/*
	 * 1. File descriptor 
	 */
	
	/* Check if we can read the file or not */
	ret = access(file_path, R_OK);
	if(ret == -1)
	{
		elfp_err_warn("elfp_main_create", "access() failed");
		elfp_err_warn("elfp_main_create", "File doesn't exist / No read permissions");
		goto return_free;
	}

	/* Open the file */
	ret = open(file_path, O_RDONLY);
	if(ret == -1)
	{
		elfp_err_warn("elfp_main_create", "open() failed");
		goto return_free;
	}
	/* Update the file descriptor */
	main->fd = ret;

	/*
	 * 2. Check if the file is ELF or not.
	 */
	ret = read(main->fd, magic, 4);
	if(ret != 4)
	{
		elfp_err_warn("elfp_main_create", "read() failed");
		goto return_close;
	}

	if(strcmp(magic, ELFMAG) != 0)
	{
		elfp_err_warn("elfp_main_create", 
		"Not an ELF file according to the magic characters");
		goto return_close;
	}
	
	/* Now that we know that it IS an ELF file, let us continue
	 * populating */

	/*
	 * 3. File size
	 */
	ret = fstat(main->fd, &st);
	if(ret == -1)
	{
		elfp_err_warn("elfp_main_create", "fstat() failed");
		goto return_close;
	}
	/* Update size */
	main->file_size = st.st_size;

	/*
	 * 4. Update path
	 */
	strncpy(main->path, file_path, ELFP_FILEPATH_SIZE);

	/*
	 * 5. Update start address
	 */
	start_addr = mmap(NULL, main->file_size, PROT_READ, MAP_PRIVATE,
					main->fd, 0);
	if(start_addr == MAP_FAILED)
	{
		elfp_err_warn("elfp_main_create", "mmap() failed");
		goto return_close;
	}
	main->start_addr = (unsigned char *)start_addr;

	/* 
	 * 6. Initialize the free list *
	 */
	ret = elfp_free_addr_vector_init(&main->free_vec);
	if(ret != 0)
	{
		elfp_err_warn("elfp_main_create",
				"elfp_free_addr_vector_init() failed");

		goto return_munmap;
	}

	/* At this point, all members except 'handle' are populated.
	 *
	 * 'handle' is a member which a create() function cannot decide.
	 * It needs to be given by one of main_vec's functions.
	 *
	 * So, we are done here */
	
	return main;
		

return_munmap:
	munmap(main->start_addr, main->file_size);

return_close:
	close(main->fd);

return_free:
	free(main);
	return NULL;
}

int
elfp_main_update_handle(elfp_main *main, int handle)
{
	if(main == NULL || handle < 0)
	{
		elfp_err_warn("elfp_main_update_handle", "Invalid argument(s) passed");
		return -1;
	}

	main->handle = handle;

	return 0;
}

int
elfp_main_fini(elfp_main *main)
{
	/* Basic check */
	if(main == NULL)
	{
		elfp_err_warn("elfp_main_fini", "NULL argument passed");
		return -1;
	}
	
	int handle = main->handle;

	/* unmap the file */
	munmap(main->start_addr, main->file_size);

	/* Close the file */
	close(main->fd);

	/* De-init the free vector */
	elfp_free_addr_vector_fini(&main->free_vec); 

	/* Now that we have cleaned up everything inside the object,
	 * it is time to clean the object itself */
	free(main);
	
	/* Inform main_vec about it */
	elfp_main_vec_inform(handle);

	/* All the above functions can present a runtime error.
	 * But they are ignored because nothing can be done to
	 * handle them properly. Best way is to leave it and let
	 * OS take care of it */
	
	return 0;
}



/*
 * All definitions related to elfp_main_vector(main_vec) are present below.
 *
 * Refer elfp_int.h for declarations and description.
 */

elfp_main_vector main_vec;

int
elfp_main_vec_init()
{
	
	/* Update initial size */
	main_vec.total = ELFP_MAIN_VECTOR_INIT_SIZE;

	/* Allocate memory */
	main_vec.vec = calloc(main_vec.total, sizeof(elfp_main *));
	if(main_vec.vec == NULL)
	{
		elfp_err_warn("elfp_main_vec_init", "calloc() failed");
		elfp_err_warn("elfp_main_vec_init", "Fatal Error. Library cannot be used");
		return -1;
	}
	
	/* Initially, there is nothing */
	main_vec.latest = 0;

	return 0;
}

int
elfp_main_vec_add(elfp_main *main)
{
	/* Basic check */
	if(main == NULL)
	{
		elfp_err_warn("elfp_main_vec_add", "NULL argument passed");
		return -1;
	}

	void *new_addr = NULL;
	int handle;

	/* Check if the vector is full */
	if(main_vec.latest == main_vec.total)
	{
		/* Allocate more memory */
		new_addr = realloc(main_vec.vec, 
			(main_vec.total + ELFP_MAIN_VECTOR_INIT_SIZE) * sizeof(elfp_main *));

		if(new_addr == NULL)
		{
			elfp_err_warn("elfp_main_vec_add", "realloc() failed");
			return -1;
		}

		/* Zeroize the new memory */
		memset(((char *)new_addr) + main_vec.total * sizeof(elfp_main *), '\0',
				ELFP_MAIN_VECTOR_INIT_SIZE * sizeof(elfp_main *));
		
		/* All set, change the members */
		main_vec.total = main_vec.total + ELFP_MAIN_VECTOR_INIT_SIZE;
		main_vec.vec = new_addr;
	}

	/* Add it */
	main_vec.vec[main_vec.latest] = main;
	handle = main_vec.latest;

	/* Then update it */
	main_vec.latest = main_vec.latest + 1;
	
	/* All good, we got the handle */
	return handle;
}

void
elfp_main_vec_fini()
{
	unsigned long int i;
	
	/* Iterate through the vector and free all the active objects */
	for(i = 0; i < main_vec.latest; i++)
	{	
		if(main_vec.vec[i] != NULL)
			elfp_main_fini(main_vec.vec[i]);
	}

	/* Free up the vector itself */
	free(main_vec.vec);
	
	return;
}

void
elfp_main_vec_inform(int handle)
{
	main_vec.vec[handle] = NULL;
}

elfp_main*
elfp_main_vec_get_em(int handle)
{
	return main_vec.vec[handle];
}

int
elfp_sanitize_handle(int handle)
{
	/* Basic boundary checks */
	if(handle < 0 || handle >= main_vec.latest)
	{
		elfp_err_warn("elfp_sanitize_handle", "Invalid Handle passed");
		return -1;
	}

	elfp_main *main = NULL;
	/* Now we know that the handle is in the valid range.
	 * Let us see if it has been closed before */
	main = elfp_main_vec_get_em(handle);
	if(main == NULL)
	{
		elfp_err_warn("elfp_sanitize_handle", "Handle already closed");
		return -1;
	}
	
	/* At this point, these checks are sufficient.
	 * We may add other checks later */
	return 0;
}
