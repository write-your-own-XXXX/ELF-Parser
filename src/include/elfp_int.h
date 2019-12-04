/*
 * File: elfp_int.h
 *
 * Description: Data structures needed to maintain open files, metadata 
 * 		related to files.
 *
 * 		* Internal to the tool. User should not touch these structures.
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

#ifndef _ELFP_INT_H
#define _ELFP_INT_H

/******************************************************************************
 * Structure: elfp_free_addr_vector
 *
 * Description: 
 *	* During file parsing, new objects are created using malloc, calloc.
 *	* We need to keep track of these objects(addresses) so that they can
 *		be freed later, once their use is over.
 *****************************************************************************/

#define ELFP_FREE_ADDR_VECTOR_INIT_SIZE 1000

typedef struct elfp_free_addr_vector
{
	void **addrs;
	unsigned long int total;
	unsigned long int count;
} elfp_free_addr_vector;

/*
 * elfp_free_addr_vector_init: Initializes a free list
 * 
 * @arg0: A reference to an elfp_free_addr_vector structure.
 *
 * @return: Returns -1 on error, 0 on successful initialization.
 */
int
elfp_free_addr_vector_init(elfp_free_addr_vector *vec);

/*
 * elfp_free_addr_vector_fini: Deinitializes a free list.
 * 	* It frees up all the stored addresses and
 * 	frees up the memory allocated for the list.
 *
 * @arg0: A reference to an elfp_free_addr_vector structure.
 */
void
elfp_free_addr_vector_fini(elfp_free_addr_vector *vec);

/*
 * elfp_free_addr_vector_add: Adds a given address to the free list.
 *
 * @arg0: Reference to a elfp_free_addr_vector object.
 * @arg1: Address to be stored in the vector.
 *
 * @return: -1 on failure, 0 on success.
 */
int
elfp_free_addr_vector_add(elfp_free_addr_vector *vec, void *addr);


/******************************************************************************
 * Structure: elfp_main
 *
 * Description: Per-file metadata
 * 	* When a new file is opened using elfp_open(), an instance of this
 * 	structure is created.
 *****************************************************************************/
#define ELFP_FILEPATH_SIZE 256

typedef struct elfp_main
{	
	/* File descriptor of the open file */
	int fd;

	/* File Path */
	char path[ELFP_FILEPATH_SIZE];

	/* Other details related to file */
	unsigned long int file_size;

	/* Starting address of mmap */
	unsigned char *start_addr;

	/* Handle sent to the user */
	int handle;

	/* Many functions allocate objects in heap and return the pointer 
	 * to it to the user.
	 *
	 * Need to keep track of all this memory and free all of it in the end.
	 */
	elfp_free_addr_vector free_vec;
		
} elfp_main;



/******************************************************************************
 * Structure: elfp_main_vector
 *
 * Description: It is a vector of elfp_main objects.
 * 	* Essential to handle multiple files.
 *****************************************************************************/

#define ELFP_MAIN_VECTOR_INIT_SIZE 1000

typedef struct elfp_vector_main
{
	/* Array of pointer to elfp_main structure */
	elfp_main **vec;

	/* Size of the vector */
	unsigned long int total;

	/* Handle of the latest opened file.
	 * This is NOT the total number of open files. */
	unsigned long int latest;

} elfp_main_vector;

#endif /* _ELFP_INT_H */
