/*
 * File: elfp_ds.h
 *
 * Description: Few general data structures which can be used
 * 		by the library.
 *		* It can also be used by programmers.
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

#ifndef _ELFP_DS_H
#define _ELFP_DS_H

/******************************************************************************
 * Structure: elfp_ds_vector
 *
 * Description: 
 *	* During file parsing, new objects are created using malloc, calloc.
 *	* In many places, we needed a vector of addresses. So, this DS.
 *****************************************************************************/

#define ELFP_DS_VECTOR_INIT_SIZE 1000

typedef struct elfp_ds_vector
{
	void **addrs;
	unsigned long int total;
	unsigned long int count;
} elfp_ds_vector;

/*
 * elfp_ds_vector_init: Initializes a free list
 * 
 * @arg0: A reference to an elfp_ds_vector structure.
 *
 * @return: Returns -1 on error, 0 on successful initialization.
 */
int
elfp_ds_vector_init(elfp_ds_vector *vec);

/*
 * elfp_ds_vector_fini: Deinitializes a free list.
 * 	* It frees up all the stored addresses and
 * 	frees up the memory allocated for the list.
 *
 * @arg0: A reference to an elfp_ds_vector structure.
 */
void
elfp_ds_vector_fini(elfp_ds_vector *vec);

/*
 * elfp_ds_vector_add: Adds a given address to the free list.
 *
 * @arg0: Reference to a elfp_ds_vector object.
 * @arg1: Address to be stored in the vector.
 *
 * @return: -1 on failure, 0 on success.
 */
int
elfp_ds_vector_add(elfp_ds_vector *vec, void *addr);

#endif /* _ELFP_DS_H */
