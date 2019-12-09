/*
 * File: elfp_phdr.c
 *
 * Description: Definitions to all functions related to Program Header 
 * 		and Program Header table are present here.
 *
 *              * Refer elfp.h for functions declarations and description.
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

#include <elf.h>
#include "./include/elfp_err.h"
#include "./include/elfp_int.h"
#include "./include/elfp.h"


/*
 * The below functions are related to elfp_phdr.
 */

/*
 * The below functions are internal to the library.
 */

int
elfp_p64hdr_dump(elfp_main *main, int index)
{
	Elf64_Ehdr *ehdr = NULL;
	unsigned int phnum;
	Elf64_Phdr *phdr = NULL;
	unsigned int i;

	ehdr = (Elf64_Ehdr *)elfp_main_get_staddr(main);
	phnum = ehdr->e_phnum;

	/* Sanitize the index */
	if(index < 0 || index >= phnum)
	{
		elfp_err_warn("elfp_p64hdr_dump", "Index failed the sanity test");
		return -1;
	}
	
	/* Get the header */
	phdr = (Elf64_Phdr *)((unsigned char *)ehdr + ehdr->e_phoff);
	phdr = phdr + index;
	
	/* Dump them */
	i = 0;
	
	/* Type */
	printf("%02u. Type: %s\n", i++, elfp_phdr_decode_type(phdr->p_type));

	/* Flags */
	printf("%02u. Flags: %s\n", i++, elfp_phdr_decode_flags(phdr->p_flags));

	/* Segment file offset */
	printf("%02u. Segment file offset: %lu bytes\n", i++, phdr->p_offset);

	/* Virtual Address */
	printf("%02u. Virtual Address: 0x%lx\n", i++, phdr->p_vaddr);

	/* Physical Address */
	printf("%02u. Physical Address: 0x%lx\n", i++, phdr->p_paddr);

	/* Segment size on file */
	printf("%02u. Segment size in file: %lu bytes\n", i++, phdr->p_filesz);

	/* Segment size in memory */
	printf("%02u. Segment size in memory: %lu bytes\n", i++, phdr->p_memsz);

	/* Alignment */
	printf("%02u. Segment Alignment: 0x%lx\n", i++, phdr->p_align);

	return 0;
}


int
elfp_p32hdr_dump(elfp_main *main, int index)
{
	Elf32_Ehdr *ehdr = NULL;
	unsigned int phnum;
	Elf32_Phdr *phdr = NULL;
	unsigned int i;

	ehdr = (Elf32_Ehdr *)elfp_main_get_staddr(main);
	phnum = ehdr->e_phnum;

	/* Sanitize the index */
	if(index < 0 || index >= phnum)
	{
		elfp_err_warn("elfp_p64hdr_dump", "Index failed the sanity test");
		return -1;
	}
	
	/* Get the header */
	phdr = (Elf32_Phdr *)((unsigned char *)ehdr + ehdr->e_phoff);
	phdr = phdr + index;
	
	/* Dump them */
	i = 0;
	
	/* Type */
	printf("%02u. Type: %s\n", i++, elfp_phdr_decode_type(phdr->p_type));

	/* Flags */
	printf("%02u. Flags: %s\n", i++, elfp_phdr_decode_flags(phdr->p_flags));

	/* Segment file offset */
	printf("%02u. Segment file offset: %u bytes\n", i++, phdr->p_offset);

	/* Virtual Address */
	printf("%02u. Virtual Address: 0x%x\n", i++, phdr->p_vaddr);

	/* Physical Address */
	printf("%02u. Physical Address: 0x%x\n", i++, phdr->p_paddr);

	/* Segment size on file */
	printf("%02u. Segment size in file: %u bytes\n", i++, phdr->p_filesz);

	/* Segment size in memory */
	printf("%02u. Segment size in memory: %u bytes\n", i++, phdr->p_memsz);

	/* Alignment */
	printf("%02u. Segment Alignment: 0x%x\n", i++, phdr->p_align);

	return 0;
}

/*
 * All functions defined below are exposed to programmers.
 *
 * Refer to elfp.h for more details.
 */

int
elfp_phdr_dump(int handle, int index)
{
	int ret;
	unsigned long int class;
	elfp_main *main = NULL;

	/* Sanitize the handle */
	ret = elfp_sanitize_handle(handle);
	if(ret == -1)
	{
		elfp_err_warn("elfp_phdr_dump", "Handle failed sanity test");
		return -1;
	}
	
	/* To sanitize the index, we should know the total number of
	 * Program Headers in the Program Header table. This info is present
	 * in the ELF header. To get this into, we should know the class.
	 * If it is a 32-bit object, we typecast the start address into 
	 * Elf32_Ehdr and get e_phnum.
	 */
	main = elfp_main_vec_get_em(handle);
	if(main == NULL)
	{
		elfp_err_warn("elfp_phdr_dump", "elfp_main_vec_get_em() failed");
		return -1;
	}

	class = elfp_main_get_class(main);

	switch(class)
	{
		case ELFCLASS32:
			elfp_p32hdr_dump(main, index);
			break;

		case ELFCLASS64:
			elfp_p64hdr_dump(main, index);
			break;

		/* All invalid cases are considered as 64-bit objects */
		default:
			elfp_p64hdr_dump(main, index);
	}

	return 0;
}

/*
 * Decode functions.
 */
const char*
elfp_phdr_decode_type(unsigned long int type)
{
	switch(type)
	{
		case PT_NULL:
			return "NULL (Program Header unused)";

		case PT_LOAD:
			return "LOAD (Loadable program segment)";

		case PT_DYNAMIC:
			return "DYNAMIC (Dynamic Linking information)";
		
		case PT_INTERP:
			return "INTERP (Program Interpreter)";

		case PT_NOTE:
			return "NOTE (Auxillary Information)";

		case PT_PHDR:
			return "PHDR (Entry for header table)";

		case PT_TLS:
			return "TLS (Thread-Local storage segment)";

		case PT_GNU_EH_FRAME:
			return "GNU_EH_FRAME (GCC .eh_frame_hdr segment)";

		case PT_GNU_STACK:
			return "GNU_STACK (Indicates stack executability)";

		case PT_GNU_RELRO:
			return "GNU_RELRO (Read-only after relocation)";

		case PT_SUNWBSS:
			return "SUNWBSS (Sun Specific segment)";

		case PT_SUNWSTACK:
			return "SUNWSTACK (Stack segment)";
		
		/* Anything else is invalid */
		default:
			return "Invalid type";
	}
}

const char*
elfp_phdr_decode_flags(unsigned long int flags)
{
	switch(flags)
	{
		case PF_X:
			return "--x";

		case PF_W:
			return "-w-";

		case PF_R:
			return "r--";

		case PF_X | PF_W:
			return "-wx";

		case PF_X | PF_R:
			return "r-x";

		case PF_W | PF_R:
			return "rw-";

		case PF_X | PF_W | PF_R:
			return "rwx";
		
		case 0:
			return "---";

		/* Everything else is invalid */
		default:
			return "Invalid security permissions";
	}
}