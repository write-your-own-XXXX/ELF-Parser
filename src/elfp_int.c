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
