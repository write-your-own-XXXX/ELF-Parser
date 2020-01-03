/*
 * File: elfp.h
 *
 * Description: Listing of all API exposed to the programmer
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

#ifndef _ELFP_H
#define _ELFP_H

#include <elf.h>

/*
 * elfp_init: Initializes the library. 
 *      * MUST be called before any other library functions are called.
 *
 * @return: 0 on success, -1 on failure.
 */
int
elfp_init();


/*
 * elfp_fini: Cleans up everything and deinits the library.
 */
void
elfp_fini();


/*
 * elfp_open: Opens the specfied ELF file and returns a handle.
 *
 * @arg0: Path / name of ELF File
 *
 * @return: A non-negative integer - handle on success.
 *              (-1) on failure.
 * 	
 * 	* This handle should be used in all further functions.
 */
int
elfp_open(const char *elfp_elf_path);


/*
 * elfp_close: Closes everything about the specified handle.
 *
 * @arg0: File handle obtained from an elfp_open call.
 *
 * @return: 0 on success, -1 on failure.
 */
int
elfp_close(int handle);

/******************************************************************************
 * Parsing the ELF Header.
 *
 * 1. elfp_ehdr_dump: An in-built function which dumps the ELF Header in
 * 	human readable form.
 *
 * 2. elfp_ehdr_get: A function which returns a reference to an elfp_ehdr
 * 	instance, using which the programmer can write a dump function.
 *
 * 3. elfp_ehdr_class_get: A function which returns the class of this ELF.
 * 	* Class is essential in parsing almost all structures.
 *
 * 4. Decode functions for all members of header which are present in 
 * 	encoded form.
 *****************************************************************************/

/*
 * elfp_ehdr_dump:
 *
 * @arg0: File Handle returned by elfp_open()
 *
 * @return: 0 on success, -1 on failure.
 */
int
elfp_ehdr_dump(int handle);

/*
 * elfp_ehdr_get:
 *
 * @arg0: File Handle returned by elfp_open()
 *
 * @return: Pointer to ELF header.
 * 	* A void pointer is returned because we wouldn't know
 * 	if it is a 32-bit or 64-bit object till e_ident is parsed.
 */
void*
elfp_ehdr_get(int handle);

/*
 * elfp_ehdr_class_get:
 *
 * @arg0: File Handle returned by elfp_open()
 *
 * @return: Returns ELFCLASSNONE on failure,
 * 	class on success.
 */
unsigned long int
elfp_ehdr_class_get(int handle);

/*
 * Decode functions:
 *
 * If you want to parse the ELF header on your own, 
 * you can make use of these decode functions. 
 * 
 * All the members of ELF header present in encoded form have a 
 * decode function below. Other members can be printed as-is.
 */

/*
 * elfp_ehdr_decode_class: Decodes the class and returns a
 * decoded, human understandable string.
 *
 * @arg0: Class
 *
 * @return: A string.
 */
const char*
elfp_ehdr_decode_class(unsigned long int class);

/*
 * elfp_ehdr_decode_dataenc: Decodes the type of Data Encoding
 * used to pack this ELF file.
 *
 * @arg0: Data encoding byte
 *
 * @return: An equivalent meaningful string.
 */
const char*
elfp_ehdr_decode_dataenc(unsigned long int data_enc);

/*
 * elfp_ehdr_decode_version: Decodes the ELF version.
 *
 * @arg0: Version
 *
 * @return: A string
 */

const char*
elfp_ehdr_decode_version(unsigned long int version);

/*
 * elfp_ehdr_decode_osabi: Decodes the type of OSABI this ELF file
 * abides by.
 *
 * @arg0: The OSABI byte
 *
 * @return: A string
 */
const char*
elfp_ehdr_decode_osabi(unsigned long int osabi);

/*
 * elfp_ehdr_decode_type: Decodes the type of ELF file.
 *
 * @arg0: Type in encoded form
 *
 * @return: Type in decoded form.
 */
const char*
elfp_ehdr_decode_type(unsigned long int type);

/*
 * elfp_ehdr_decode_machine: Decodes the type of architecture
 * this ELF file was built for.
 *
 * @arg0: machine in encoded form
 *
 * @return: Machine in decoded form.
 */
const char*
elfp_ehdr_decode_machine(unsigned long int machine);

/******************************************************************************
 * Parsing the Program Header Table(PHT)
 *
 * 1. elfp_pht_dump: Dumps the complete PHT
 *
 * 2. elfp_pht_get: Gives a pointer to the PHT to the programmer.
 *****************************************************************************/

/*
 * elfp_pht_get:
 * 
 * @arg0: Handle
 *	
 * @return: A pointer to PHT on success, NULL on failure.
 *
 * It should be noted that PHT cannot be parsed without class
 * of the ELF. You may use elfp_ehdr_class_get() to get the class.
 */
void*
elfp_pht_get(int handle);

/*
 * elfp_pht_dump: 
 *
 * @arg0: Handle
 *
 * @return: 0 on success, -1 on failure
 */
int
elfp_pht_dump(int handle);


/******************************************************************************
 * Parsing Program Header
 * 
 * 1. elfp_phdr_dump: Dumps the specified program header.
 *
 * 2. Deocode functions for members present in encoded form.
 *****************************************************************************/

/*
 * elfp_phdr_dump:
 *
 * @arg0: Handle
 * @arg1: Program Header's index in the Program Header table.
 *
 * @return: 0 on success, -1 on failure.
 */
int
elfp_phdr_dump(int handle, int index);


/*
 * Decode functions
 */

/*
 * elfp_phdr_decode_type: Decodes the Segment type
 *
 * @arg0: Segment type
 *
 * @return: Decoded string.
 */
const char*
elfp_phdr_decode_type(unsigned long int type);

/*
 * elfp_phdr_decode_flags: Decodes the security permissions.
 *
 * @arg0: Flags
 *
 * @return: Decoded string
 */
const char*
elfp_phdr_decode_flags(unsigned long int flags);

/******************************************************************************
 * Parsing individual segments.
 *
 * 1. elfp_seg_get(): Get pointers to segments of a specified type.
 * 2. elfp_seg_dump(): Dump the Segment(s) you want.
 *
 * The following are the valid segment types.
 *
 * INTERP, TLS, LOAD, DYNAMIC, PHDR, NOTE, GNU_EH_FRAME, GNU_STACK, GNU_RELRO,
 * SUNWBSS, SUNWSTACK.
 *****************************************************************************/

/*
 * elfp_seg_get(): 
 *
 * @arg0: elfp handle
 * @arg1: Segment name(string). Any segment type from the above list.
 * @arg2: A pointer to an unsigned long integer. This is the second
 * 		return value of this function.
 * 
 * @return: A pointer to an array of (void *) pointers, each pointer
 * 		pointing to a segment.
 *
 * Essentially, you'll be getting the following info.
 *
 * 1. An array of pointers, each of them pointing to a segment of the
 * 	specified type.
 * 2. Total number of pointers in the above array.
 *
 * 3 cases can occur:
 * a. One or more segments of requested type are present - Happy case.
 * 
 * b. Some internal error occurs and library returns an error.
 * 	* @return = NULL
 * 	* @arg2 = 1
 *
 * c. No segments of requested type are present.
 * 	* @return = NULL
 * 	* @arg2 = 0
 *
 * From @arg2, the programmer should know what actually happened
 * and why the function returned a NULL.
 */

void**
elfp_seg_get(int handle, const char *seg_type, unsigned long int *ptr_count);

/*
 * elfp_seg_dump: Dumps the specified segment, if it is dumpable.
 *
 * @arg0: elfp handle
 * @arg1: Segment name(string). Any type from the above list.
 *
 * @return: 0 on success, -1 on failure.
 */

int
elfp_seg_dump(int handle, const char *seg_type);

#endif /* _ELFP_H */
