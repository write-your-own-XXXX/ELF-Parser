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
 * @return: A non-negative integer - handler on success.
 *              (-1) on failure.
 */
int
elfp_open(const char *elfp_elf_path);


/*
 * elfp_close: Closes everything about the specified handle.
 *
 * @arg0: User handle obtained from an elfp_open call.
 *
 * @return: 0 on success, -1 on failure.
 */
int
elfp_close(int user_handle);

#endif /* _ELFP_H */
