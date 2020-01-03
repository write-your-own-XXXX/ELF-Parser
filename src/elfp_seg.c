/*
 * File: elfp_seg.c
 *
 * Description: Many segments have dumpable information. This sourcefile has
 * 		code to dump all those segments in human-readable form.
 *
 *		* Refer elfp.h for functions declarations and description.
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

#include "./include/elfp.h"
#include "./include/elfp_int.h"
#include "./include/elfp_err.h"
#include <elf.h>
#include <string.h>

/*
 * The following functions are internal to the library.
 */

static int
elfp_seg_get_type(const char *seg_name)
{
	/* All Valid entries first */
	if(strcmp(seg_name, "NULL") == 0)
		return PT_NULL;
	
	if(strcmp(seg_name, "LOAD") == 0)
		return PT_LOAD;
	
	if(strcmp(seg_name, "DYNAMIC") == 0)
		return PT_DYNAMIC;
	
	if(strcmp(seg_name, "INTERP") == 0)
		return PT_INTERP;
	
	if(strcmp(seg_name, "NOTE") == 0)
		return PT_NOTE;
	
	if(strcmp(seg_name, "SHLIB") == 0)
		return PT_SHLIB;

	if(strcmp(seg_name, "PHDR") == 0)
		return PT_PHDR;

	if(strcmp(seg_name, "TLS") == 0)
		return PT_TLS;

	if(strcmp(seg_name, "GNU_EH_FRAME") == 0)
		return PT_GNU_EH_FRAME;

	if(strcmp(seg_name, "GNU_STACK") == 0)
		return PT_GNU_STACK;

	if(strcmp(seg_name, "GNU_RELRO") == 0)
		return PT_GNU_RELRO;

	/* Sun related types */
	if(strcmp(seg_name, "SUNWBSS") == 0)
		return PT_SUNWBSS;

	if(strcmp(seg_name, "SUNWSTACK") == 0)
		return PT_SUNWSTACK;

	/* Anything else is invalid! */
	return -1;
}

static void**
elfp_seg64_get(int handle, const char *seg_type, unsigned long int *ptr_count)
{
	/* No sanity checks */

        int ret;
        Elf64_Ehdr *ehdr = NULL;
        Elf64_Phdr *ph = NULL;
        Elf64_Phdr *pht = NULL;
	elfp_main *main = NULL;
        void *start_addr = NULL;
        int phnum;
        int enc_seg_type;
        int i;
	unsigned long int count, total;
	void **ptr_arr = NULL;
	elfp_ds_vector *free_vec = NULL;
	void **temp = NULL;

	/* Get the PHT */
	pht = elfp_pht_get(handle);
	if(pht == NULL)
	{
		elfp_err_warn("elfp_seg64_get", "elfp_pht_get() failed");
		goto fail_err;
	}
		
	/* Get the total number of program headers */    
        ehdr = elfp_ehdr_get(handle);    
        if(ehdr == NULL)    
        {    
                elfp_err_warn("elfp_seg64_get", "elfp_ehdr_get() failed");    
                goto fail_err;   
        }    
        phnum = ehdr->e_phnum;    
            
        /* In case of corrupted headers:    
         * The upper bound is simply based on the datatype used to store the    
         * number of program headers in ELF header. A uint16 is used. */    
        if(phnum == 0 || phnum > UINT16_MAX)    
        {    
                elfp_err_warn("elfp_seg64_get", "Invalid number of Program Headers");    
                goto fail_err;
        }    
	
	/* Get the starting address of the ELF file mapped to memory */
        main = elfp_main_vec_get_em(handle);
        if(main == NULL)
        {
                elfp_err_warn("elfp_seg64_get", "elfp_main_vec_get_em() failed");
                goto fail_err;
        }

        start_addr = elfp_main_get_staddr(main);
        if(start_addr == NULL)
        {
                elfp_err_warn("elfp_seg64_get", "elfp_main_get_staddr() failed");
                goto fail_err;
        }
	
	/* The user/programmer gives the segment name in string form.
         * We need to convert it into PT_XXXX form, so that it'll be easy
         * for us to iterate and compare */
        enc_seg_type = elfp_seg_get_type(seg_type);
        if(enc_seg_type == -1)
        {
                elfp_err_warn("elfp_seg64_get", 
			"Invalid Segment entered / I don't know how to parse it");
		goto fail_err;
        }
	
	/* Now comes the core part.
	 *
	 * 1. Allocate memory for the pointers.
	 * 2. Add that pointer to the free-vector.
	 * 3. Iterate through PHT and add the pointers.
	 * 4. Update ptr_count
	 */
	
	/* A naive way: Allocate memory for 5 segments.
	 * realloc() to the rescue! */
	ptr_arr = calloc(5, sizeof(void *));
	if(ptr_arr == NULL)
	{
		elfp_err_warn("elfp_seg64_get", "malloc() failed");
		goto fail_err;
	}
	
	count = 0;
	total = 5;
	for(i = 0; i < phnum; i++)
	{
		ph = pht + i;
		if(ph->p_type == enc_seg_type)
		{	
			/* If it has hit our limit, then */
			if(count == total)
			{	
				temp = realloc(ptr_arr, (total + 5) * sizeof(void *));
				if(temp == NULL)
				{
					elfp_err_warn("elfp_seg64_get", 
					"realloc() failed. Unable to accomodate all segments of the given type");
					/* We need to decide what to do here. Should we simply send
					 * the segments collected so far to the caller, or should we abort
					 * this operation, free up the memory and return an error.
					 *
					 * We are returning an error */
					goto fail_free;
				}
				ptr_arr = temp;
				total = total + 5;
			}
			/* If it is GNU_STACK, then p_flags is to be sent back
			 * to the caller.*/
			if(ph->p_type == PT_GNU_STACK)
			{
				ptr_arr[count] = (void *)(ph->p_flags);
				count++;
			}
			else
			{
				ptr_arr[count] = (void *)(start_addr + ph->p_offset);
				count++;
			}
		}
	}
	
	/* What if there are no such segments?
	 *
	 * This case should be different from an erroneous case.
	 * This should be treated as a normal case */
	if(count == 0)
	{
		elfp_err_warn("elfp_seg64_get", "No segments of the requested type are present");
		
		/* For this case, count will be 0 */
		*ptr_count = 0;
		free(ptr_arr);
		return NULL;
	}

	/* All set. We may have allocated more memory. Let us cut it */
	if(count < total)
	{
		temp = realloc(ptr_arr, count * sizeof(void *));
		if(temp == NULL)
		{
			elfp_err_warn("elfp_seg64_get",
			"realloc() failed. Unable to remove the extra memory allocated");
			/* This shouldn't happen. But it may happen.
			 * Let us just leave it. */
		}
		ptr_arr = temp;
	}

	/* Now, we have a pointer which we should add to the free address vector */
	free_vec = elfp_main_get_freevec(main);
	if(free_vec == NULL)
	{
		elfp_err_warn("elfp_seg64_get",
				"elfp_main_get_freevec() failed");
		goto fail_free;
	}
	
	ret = elfp_ds_vector_add(free_vec, ptr_arr);
	if(ret == -1)
	{
		elfp_err_warn("elfp_seg64_get",
				"elfp_ds_vector_add() failed");
		goto fail_free;
	}
	
	/* At this point, we have an array of pointers, each pointer
	 * pointing to a segment of requested type.
	 *
	 * It is time to return */
	*ptr_count = count;
	
	return ptr_arr;



fail_free:
	free(ptr_arr);
	
/* For all erroneous cases, let us keep count to be 1.
 * This is how the caller identifies between no segments of that type
 * and an error which has occured here */
fail_err:
	*ptr_count = 1;
	return NULL;
}


static void**
elfp_seg32_get(int handle, const char *seg_type, unsigned long int *ptr_count)
{
	/* No sanity checks */

        int ret;
        Elf32_Ehdr *ehdr = NULL;
        Elf32_Phdr *ph = NULL;
	Elf32_Phdr *pht = NULL;
        elfp_main *main = NULL;
        void *start_addr = NULL;
        int phnum;
        int enc_seg_type;
        int i;
	unsigned long int count, total;
	void **ptr_arr = NULL;
	elfp_ds_vector *free_vec = NULL;
	void **temp = NULL;
	
	/* Get the PHT */
	pht = elfp_pht_get(handle);
	if(pht == NULL)
	{
		elfp_err_warn("elfp_seg32_get", "elfp_pht_get() failed");
		goto fail_err;
	}

	/* Get the total number of program headers */    
        ehdr = elfp_ehdr_get(handle);    
        if(ehdr == NULL)    
        {    
                elfp_err_warn("elfp_seg32_get", "elfp_ehdr_get() failed");    
                goto fail_err;   
        }    
        phnum = ehdr->e_phnum;    
            
        /* In case of corrupted headers:    
         * The upper bound is simply based on the datatype used to store the    
         * number of program headers in ELF header. A uint16 is used. */    
        if(phnum == 0 || phnum > UINT16_MAX)    
        {    
                elfp_err_warn("elfp_seg32_get", "Invalid number of Program Headers");    
                goto fail_err;  
        }    
	
	/* Get the starting address of the ELF file mapped to memory */
        main = elfp_main_vec_get_em(handle);
        if(main == NULL)
        {
                elfp_err_warn("elfp_seg32_get", "elfp_main_vec_get_em() failed");
        	goto fail_err;
        }

        start_addr = elfp_main_get_staddr(main);
        if(start_addr == NULL)
        {
                elfp_err_warn("elfp_seg32_get", "elfp_main_get_staddr() failed");
                goto fail_err;
        }
	
	/* The user/programmer gives the segment name in string form.
         * We need to convert it into PT_XXXX form, so that it'll be easy
         * for us to iterate and compare */
        enc_seg_type = elfp_seg_get_type(seg_type);
        if(enc_seg_type == -1)
        {
                elfp_err_warn("elfp_seg32_get", 
			"Invalid Segment entered / I don't know how to parse it");
		goto fail_err;
        }
	
	/* Now comes the core part.
	 *
	 * 1. Allocate memory for the pointers.
	 * 2. Add that pointer to the free-vector.
	 * 3. Iterate through PHT and add the pointers.
	 * 4. Update ptr_count
	 */
	
	/* A naive way: Allocate memory for 5 segments.
	 * realloc() to the rescue! */
	ptr_arr = calloc(5, sizeof(void *));
	if(ptr_arr == NULL)
	{
		elfp_err_warn("elfp_seg32_get", "malloc() failed");
		goto fail_err;
	}
	
	count = 0;
	total = 5;
	for(i = 0; i < phnum; i++)
	{
		ph = pht + i;
		if(ph->p_type == enc_seg_type)
		{	
			/* If it has hit our limit, then */
			if(count == total)
			{	
				temp = realloc(ptr_arr, (total + 5) * sizeof(void *));
				if(temp == NULL)
				{
					elfp_err_warn("elfp_seg32_get", 
					"realloc() failed. Unable to accomodate all segments of the given type");
					/* We need to decide what to do here. Should we simply send
					 * the segments collected so far to the caller, or should we abort
					 * this operation, free up the memory and return an error.
					 *
					 * We are returning an error */
					goto fail_free;
				}
				ptr_arr = temp;
				total = total + 5;
			}
                        /* If it is GNU_STACK, then p_flags is to be sent back
                         * to the caller.*/
                        if(ph->p_type == PT_GNU_STACK)
                        {
                                ptr_arr[count] = (void *)(ph->p_flags);
                                count++;
                        }
                        else
                        {
                                ptr_arr[count] = (void *)(start_addr + ph->p_offset);
                                count++;
                        }
		}
	}
	
	/* What if there are no such segments?
	 *
	 * This case should be different from an erroneous case.
	 * This should be treated as a normal case */
	if(count == 0)
	{
		elfp_err_warn("elfp_seg32_get", "No segments of the requested type are present");
		
		/* For this case, count will be 0 */
		*ptr_count = 0;
		goto fail_free;
	}

	/* All set. We may have allocated more memory. Let us cut it */
	if(count < total)
	{
		temp = realloc(ptr_arr, count * sizeof(void *));
		if(temp == NULL)
		{
			elfp_err_warn("elfp_seg32_get",
			"realloc() failed. Unable to remove the extra memory allocated");
			/* This shouldn't happen. But it may happen.
			 * Let us just leave it. */
		}
		ptr_arr = temp;
	}

	/* Now, we have a pointer which we should add to the free address vector */
	free_vec = elfp_main_get_freevec(main);
	if(free_vec == NULL)
	{
		elfp_err_warn("elfp_seg32_get",
				"elfp_main_get_freevec() failed");
		goto fail_free;
	}
	
	ret = elfp_ds_vector_add(free_vec, ptr_arr);
	if(ret == -1)
	{
		elfp_err_warn("elfp_seg32_get",
				"elfp_ds_vector_add() failed");
		goto fail_free;
	}
	
	/* At this point, we have an array of pointers, each pointer
	 * pointing to a segment of requested type.
	 *
	 * It is time to return */
	*ptr_count = count;
	
	return ptr_arr;


fail_free:
	free(ptr_arr);
	
/* For all erroneous cases, let us keep count to be 1.
 * This is how the caller identifies between no segments of that type
 * and an error which has occured here */
fail_err:
	*ptr_count = 1;
	return NULL;
}



static void
elfp_seg_dump_interp(void **ptr_arr, unsigned long int ptr_count)
{
	int i;
	for(i = 0; i < ptr_count; i++)
	{
		printf("The interpreter requested by this executable: %s\n", (char *)ptr_arr[i]);
	}
}

static void
elfp_seg_dump_gnu_stack(void **ptr_arr, unsigned long int ptr_count)
{
	int i;
	unsigned int flags;

	for(i = 0; i < ptr_count; i++)
	{	
		flags = (unsigned int)ptr_arr[i];
		printf("Stack permissions: %s\n", elfp_phdr_decode_flags(flags));
	}
}


static int
elfp_seg32_dump(int handle, const char *seg_type)
{
	/* No need for basic checks because they are all sanitized inputs */
	
	int ret;
	void **ptr_arr = NULL;
	unsigned long int ptr_count;
	int enc_seg_type;

	/* We have the elfp_seg64_get() function. Let us use it and quickly
	 * get our segment pointer array */
	ptr_arr = elfp_seg32_get(handle, seg_type, &ptr_count);
	if(ptr_arr == NULL)
	{
		elfp_err_warn("elfp_seg32_dump", "elfp_seg32_get() failed");
		return -1;
	}
	
	/* Depending on the segment type, we need to dump it */
	enc_seg_type = elfp_seg_get_type(seg_type);
	if(enc_seg_type == -1)
	{
		elfp_err_warn("elfp_seg32_dump", "elfp_seg_get_type() failed");
		return -1;
	}
	
	/* Now, let us start */
	switch(enc_seg_type)
	{
		case PT_INTERP:
			elfp_seg_dump_interp(ptr_arr, ptr_count);
			return 0;

		case PT_PHDR:
                        ret = elfp_pht_dump(handle);
                        if(ret == -1)
                        {       
                                elfp_err_warn("elfp_seg64_dump",
                                                "elfp_pht_dump() failed");
                                return -1;
                        }
                        return 0;

		case PT_GNU_STACK:
			elfp_seg_dump_gnu_stack(ptr_arr, ptr_count);
			return 0;

		default:
			elfp_err_warn("elfp_seg32_dump",
			"Still have to write parse code");
			return -1;
	}
}

static int
elfp_seg64_dump(int handle, const char *seg_type)
{
	/* No need for basic checks because they are all sanitized inputs */
	
	int ret;
	void **ptr_arr = NULL;
	unsigned long int ptr_count;
	int enc_seg_type;

	/* We have the elfp_seg64_get() function. Let us use it and quickly
	 * get our segment pointer array */
	ptr_arr = elfp_seg64_get(handle, seg_type, &ptr_count);
	if(ptr_arr == NULL)
	{
		elfp_err_warn("elfp_seg64_dump", "elfp_seg64_get() failed");
		return -1;
	}
	
	/* Depending on the segment type, we need to dump it */
	enc_seg_type = elfp_seg_get_type(seg_type);
	if(enc_seg_type == -1)
	{
		elfp_err_warn("elfp_seg64_dump", "elfp_seg_get_type() failed");
		return -1;
	}
	
	/* Now, let us start */
	switch(enc_seg_type)
	{
		case PT_INTERP:
			elfp_seg_dump_interp(ptr_arr, ptr_count);
			return 0;
		
		case PT_PHDR:
			ret = elfp_pht_dump(handle);
			if(ret == -1)
			{
				elfp_err_warn("elfp_seg64_dump",
						"elfp_pht_dump() failed");
				return -1;
			}
			return 0;
		
                case PT_GNU_STACK:
                        elfp_seg_dump_gnu_stack(ptr_arr, ptr_count);
                        return 0;

		default:
			elfp_err_warn("elfp_seg64_dump",
					"Still have to write parse code");
			return -1;
	}
}

/*
 * The following functions are exposed API.
 */

void** 
elfp_seg_get(int handle, const char *seg_type, unsigned long int *ptr_count)
{
	/* Basic checks */
	if(seg_type == NULL || ptr_count == NULL || 
				elfp_sanitize_handle(handle) == -1)
	{
		elfp_err_warn("elfp_seg_get", "Invalid argument(s) passed");
		goto fail_err;
	}

	int ret;
	void **ptr_arr = NULL;
	elfp_main *main = NULL;
	unsigned long int class;

	/* Get the class */
	main = elfp_main_vec_get_em(handle);
	if(main == NULL)
	{
		elfp_err_warn("elfp_seg_get",
			"elfp_main_vec_get_em() failed");
		goto fail_err;
	}

	class = elfp_main_get_class(main);

        switch(class)
        {
                case ELFCLASS32:
                        ptr_arr = elfp_seg32_get(handle, seg_type, ptr_count);
                        if(ptr_arr == NULL)
                        {
                                elfp_err_warn("elfp_seg_get", 
				"elfp_seg32_get() failed / no segment of requested type found");           
                        }
			return ptr_arr;
                
                /* 64-bit ELF files and any other case is analyzed as 64-bit
                 * ELF files */
                default:
                        ptr_arr = elfp_seg64_get(handle, seg_type, ptr_count);
                        if(ptr_arr == NULL)
                        {
                                elfp_err_warn("elfp_seg_get", 
				"elfp_seg64_get() failed / no segment of requested type found");
                        }
			return ptr_arr;
        }

fail_err:
	*ptr_count = 1;
	return NULL;
}

int
elfp_seg_dump(int handle, const char *seg_type)
{
	/* Basic check */
	if(seg_type == NULL || elfp_sanitize_handle(handle) == -1)
	{
		elfp_err_warn("elfp_seg_dump", "Invalid argument(s) passed");
		return -1;
	}

	int ret;
	elfp_main *main = NULL;
	unsigned long int class;

	/* Get the class */
	main = elfp_main_vec_get_em(handle);
	if(main == NULL)
	{
		elfp_err_warn("elfp_seg_dump",
			"elfp_main_vec_get_em() failed");
		return -1;
	}
	
	class = elfp_main_get_class(main);

	switch(class)
	{
		case ELFCLASS32:
			ret = elfp_seg32_dump(handle, seg_type);
			if(ret == -1)
			{
				elfp_err_warn("elfp_seg_dump", "elfp_seg32_dump() failed");
				return -1;
			}
			return 0;
		
		/* 64-bit ELF files and any other case is analyzed as 64-bit
		 * ELF files */
		default:
			ret = elfp_seg64_dump(handle, seg_type);
			if(ret == -1)
			{
				elfp_err_warn("elfp_seg_dump", "elfp_seg64_dump() failed");
				return -1;
			}
			return 0;
	}
}
