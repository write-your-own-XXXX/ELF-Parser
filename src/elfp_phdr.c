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
 * The below functions are related to elfp_pht.
 * They are API exposed to programmers. Refer elfp.h for more
 * details.
 */
void*
elfp_pht_get(int handle)
{
        /* Sanity check */
        if(elfp_sanitize_handle(handle) == -1)
        {
                elfp_err_warn("elfp_pht_get", "Handle failed the sanity test");
                return NULL;
        }

        int ret;
        elfp_main *main = NULL;
        void *pht = NULL;
        Elf64_Ehdr *e64hdr = NULL;
        Elf32_Ehdr *e32hdr = NULL;
	unsigned long int class;

        /* Get the class */
        main = elfp_main_vec_get_em(handle);
	if(main == NULL)
	{
		elfp_err_warn("elfp_pht_get", "elfp_main_vec_get_em() failed");
		return NULL;
	}

        /* Return based on class */
        switch(class)
        {
                case ELFCLASS32:
                        e32hdr = (Elf32_Ehdr *)elfp_main_get_staddr(main);
			/* Check if this has no Program Headers */
			if(e32hdr->e_phnum == 0)
			{
				elfp_err_warn("elfp_pht_get",
				"This ELF file has no Program Headers");
				return NULL;
			}
                        pht = ((unsigned char *)e32hdr + e32hdr->e_phoff);
                        return pht;

                /*ELFCLASS64, anything else will be treated as 64-bit objects */
                default:                                         
			e64hdr = (Elf64_Ehdr *)elfp_main_get_staddr(main);
			/* Check if this has no Program Headers */
			if(e64hdr->e_phnum == 0)
			{
				elfp_err_warn("elfp_pht_get",
				"This ELF file has no Program Headers");
				return NULL;
			}
			pht = ((unsigned char *)e64hdr + e64hdr->e_phoff);
			return pht;
	}
}

int
elfp_pht_dump(int handle)
{
        /* Sanity check */
        if(elfp_sanitize_handle(handle) == -1)
        {
                elfp_err_warn("elfp_pht_dump", "Handle failed the sanity test");
                return -1;
        }

        int ret;
        elfp_main *main = NULL;
        unsigned long int class;
        unsigned long int phnum;
        Elf64_Ehdr *e64hdr = NULL;
        Elf32_Ehdr *e32hdr = NULL;
        unsigned int i;
        
        /* Get the class */
        main = elfp_main_vec_get_em(handle);
        class = elfp_main_get_class(main);
        /* Get the total number of headers.
         *
         * There are 2 ways to get the number of headers.
         * 1. Get it from ELF header: The e_phnum member has it.
         * 
         * 2. The PT_PHDR Program Header has the total size of the Table.
         *      * We know size of each entry - sizeof(ElfN_Phdr)
         *      * From these, we can calculate number of headers.
         *      * phnum = (phdr->p_filesz) / sizeof(ElfN_Phdr);
         *
         * Following method 1.
         */

        switch(class)
        {
                case ELFCLASS32:
                        e32hdr = (Elf32_Ehdr *)elfp_main_get_staddr(main);
                        phnum = e32hdr->e_phnum;
                        break;

                case ELFCLASS64:
                        e64hdr = (Elf64_Ehdr *)elfp_main_get_staddr(main);
                        phnum = e64hdr->e_phnum;
                        break;

		/* Anything else is also considered as 64-bit object */
                default:
                        e64hdr = (Elf64_Ehdr *)elfp_main_get_staddr(main);
                        phnum = e64hdr->e_phnum;
        }
        
        /* Check if there are any program headers */
        if(phnum == 0)
        {
                printf("There are no Program Headers in this file\n");
                return 0;
        }

        /* Now we have everything. Let us dump everything */
        i = 0;
        printf("\n==================================================\n");
        printf("Program Header Table: \n\n");
        
        while (i < phnum)
        {
                printf("Entry %02u: \n", i);
                elfp_phdr_dump(handle, i);
                printf("---------------------------------------------\n");
                i = i + 1;
        }
	
	return 0;
}


/*
 * The below functions are internal to the library.
 */

static int
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


static int
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
	/* Sanitize the handle */
	if(elfp_sanitize_handle(handle)== -1)
	{
		elfp_err_warn("elfp_phdr_dump", "Handle failed sanity test");
		return -1;
	}

	int ret;
	unsigned long int class;
	elfp_main *main = NULL;

	
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
