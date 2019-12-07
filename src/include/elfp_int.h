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

/*
 * elfp_main_create: Creates a new elfp_main object and returns a
 * 	reference to it.
 *
 * @arg0: File path - A NULL terminated string.
 *
 * @return: NULL on failure, Reference to an empty elfp_main object on success.
 */
elfp_main*
elfp_main_create(const char *file_path);

/*
 * elfp_main_update_handle: Updates 'handle' of elfp_main object.
 *
 * @arg0: Reference to an elfp_main object.
 * @arg1: An integer.
 * 	* This MUST be a return value of elfp_main_vector_add().
 *
 * @return: 0 on success, -1 on failure.
 */
int
elfp_main_update_handle(elfp_main *main, int handle);

/*
 * elfp_main_fini: Cleans up everything related to a given elfp_main object.
 *
 * @arg0: Reference to an elfp_main object
 *
 * @return: 0 on success, -1 on failure.
 */
int
elfp_main_fini(elfp_main *main);

/*
 * elfp_main_get_fd: Gets the file descriptor.
 *
 * @arg0: Reference to an elfp_main object
 *
 * @return: -1 on failure, fd on success.
 */
int
elfp_main_get_fd(elfp_main *main);

/*
 * elfp_main_get_filesz: Gets file size
 *
 * @arg0: Reference to an elfp_main object
 *
 * @return: 0 on failure, file size on success.
 */
unsigned long int
elfp_main_get_filesz(elfp_main *main);

/*
 * elfp_main_get_staddr: Gets the start address
 *
 * @arg0: Reference to an elfp_main object
 *
 * @return: NULL on failure, start address on success.
 */
void*
elfp_main_get_staddr(elfp_main *main);

/*
 * elfp_main_get_handle: Gets the handle
 *
 * @arg0: Reference to an elfp_main object
 *
 * @return: -1 on failure, handle on success.
 */
int
elfp_main_get_handle(elfp_main *main);

/*
 * elfp_main_get_freevec: Gets the reference to the free vector.
 *
 * @arg0: Reference to an elfp_main object
 *
 * @return: NULL on failure, reference to the free vector on success.
 */
elfp_free_addr_vector*
elfp_main_get_freevec(elfp_main *main);



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

extern elfp_main_vector main_vec;

/*
 * elfp_main_vec_init: Initializes main_vec structure
 *
 * @return: 0 on success, -1 on failure.
 */
int
elfp_main_vec_init();

/*
 * elfp_main_vec_add: Adds an elfp_main reference to main_vec.
 *
 * @arg0: Reference to an elfp_main structure.
 *
 * @return: handle(a non-positive integer) on success,
 * 		-1 on failure.
 */
int
elfp_main_vec_add(elfp_main *main);

/*
 * elfp_main_vec_fini: Cleans up the main_vec.
 *	Should be called only if the library is about to be de-inited.
 */
void
elfp_main_vec_fini();

/*
 * elfp_main_vec_inform: Informs main_vec about de-initing of a 
 * 	elfp_main object.
 *
 * @arg0: Handle to that elfp_main object.
 */
void
elfp_main_vec_inform(int handle);

/*
 * elfp_main_vec_get_em: Returns the elfp_main object corresponding to 
 * 	a given user handle.
 *
 * @arg0: User handle, an integer.
 *
 * @return: Reference to an elfp_main object.
 */

elfp_main*
elfp_main_vec_get_em(int handle);

/*
 * elfp_sanitize_handle: Sanitizes the user fed handle.
 *
 * @arg0: User handle
 *
 * @return: 0 if the user handle is valid. -1 if user handle is invalid.
 */
int
elfp_sanitize_handle(int handle);

#endif /* _ELFP_INT_H */
