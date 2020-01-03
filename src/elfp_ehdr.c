/*
 * File: elfp_ehdr.c
 *
 * Description: Definitions to all functions related to ELF Header will be
 * 		present here.
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

#include <elf.h>

#include "./include/elfp_err.h"
#include "./include/elfp_int.h"
#include "./include/elfp.h"

/*
 * The below functions are internal to the library.
 * These won't be exposed to the programmer.
 */

static void
elfp_dump_e64hdr(void *start_addr)
{
	Elf64_Ehdr *ehdr = start_addr;
	unsigned int i = 5;
	
	/*
	 * 1. e_type
	 */
	printf("%02u. ELF Type: %s\n", i++, elfp_ehdr_decode_type(ehdr->e_type));

	/*
	 * 2. e_machine
	 */
	printf("%02u. Architecture: %s\n", i++, elfp_ehdr_decode_machine(ehdr->e_machine));

	/*
	 * 3. e_entry
	 */
	printf("%02u. Entry Address: 0x%lx\n", i++, ehdr->e_entry);

	/*
	 * 4. e_phoff
	 */
	printf("%02u. Program Header Table file offset: %lu bytes\n", 
							i++,ehdr->e_phoff);
	
	/*
	 * 5. e_shoff
	 */
	printf("%02u. Section Header Table file offset: %lu bytes\n",
							i++, ehdr->e_shoff);
	
	/*
	 * 6. e_flags
	 */
	printf("%02u. Flags: %u\n", i++, ehdr->e_flags);

	/*
	 * 7. e_ehsize
	 */
	printf("%02u. ELF header size: %u bytes\n", i++, ehdr->e_ehsize);

	/*
	 * 8. e_phentsize
	 */
	printf("%02u. Program Header size: %u bytes\n", i++, ehdr->e_phentsize);

	/*
	 * 9. e_phnum
	 */
	printf("%02u. Program Headers count: %u\n", i++, ehdr->e_phnum);

	/*
	 * 10. e_shentsize
	 */
	printf("%02u. Section Header size: %u bytes\n", i++, ehdr->e_shentsize);

	/*
	 * 11. e_shnum
	 */
	printf("%02u. Section Header count: %u\n", i++, ehdr->e_shnum);

	/*
	 * 12. e_shstrndx
	 */
	printf("%02u. Section Header String Table index: %u\n", i++, ehdr->e_shstrndx);

	return;
}

static void
elfp_dump_e32hdr(void *start_addr)
{
	Elf32_Ehdr *ehdr = start_addr;
	unsigned int i = 5;
	
	/*
	 * 1. e_type
	 */
	printf("%02u. ELF Type: %s\n", i++, elfp_ehdr_decode_type(ehdr->e_type));

	/*
	 * 2. e_machine
	 */
	printf("%02u. Architecture: %s\n", i++, elfp_ehdr_decode_machine(ehdr->e_machine));

	/*
	 * 3. e_entry
	 */
	printf("%02u. Entry Address: 0x%x\n", i++, ehdr->e_entry);

	/*
	 * 4. e_phoff
	 */
	printf("%02u. Program Header Table file offset: %u bytes\n", 
							i++,ehdr->e_phoff);
	
	/*
	 * 5. e_shoff
	 */
	printf("%02u. Section Header Table file offset: %u bytes\n",
							i++, ehdr->e_shoff);
	
	/*
	 * 6. e_flags
	 */
	printf("%02u. Flags: %u\n", i++, ehdr->e_flags);

	/*
	 * 7. e_ehsize
	 */
	printf("%02u. ELF header size: %u bytes\n", i++, ehdr->e_ehsize);

	/*
	 * 8. e_phentsize
	 */
	printf("%02u. Program Header size: %u bytes\n", i++, ehdr->e_phentsize);

	/*
	 * 9. e_phnum
	 */
	printf("%02u. Program Headers count: %u\n", i++, ehdr->e_phnum);

	/*
	 * 10. e_shentsize
	 */
	printf("%02u. Section Header size: %u bytes\n", i++, ehdr->e_shentsize);

	/*
	 * 11. e_shnum
	 */
	printf("%02u. Section Header count: %u\n", i++, ehdr->e_shnum);

	/*
	 * 12. e_shstrndx
	 */
	printf("%02u. Section Header String Table index: %u\n", i++, ehdr->e_shstrndx);

	return;
}

/*
 * The below functions are API exposed to programmers.
 *
 * Refer elfp.h for functions' description.
 */

void*
elfp_ehdr_get(int handle)
{
	/* First sanitize it */
	if(elfp_sanitize_handle(handle) == -1)
	{
		elfp_err_warn("elfp_ehdr_get", "Handle failed sanity test");
		return NULL;
	}

	int ret;
	void *addr = NULL;
	elfp_main *main = NULL;

	main = elfp_main_vec_get_em(handle);
	if(main == NULL)
	{
		elfp_err_warn("elfp_ehdr_get", "elfp_main_vec_get_em() failed");
		return NULL;
	}

	addr = elfp_main_get_staddr(main);
	if(addr == NULL)
	{
		elfp_err_warn("elfp_ehdr_get", "elfp_main_get_staddr() failed");
		return NULL;
	}
	return addr;
}


int
elfp_ehdr_dump(int handle)
{
	void *ehdr = NULL;
	int ret;
	unsigned char *e_ident = NULL;
	char *decoded_str = NULL;
	unsigned int i;
	
	/* Get the header */
	ehdr = elfp_ehdr_get(handle);
	if(ehdr == NULL)
	{
		elfp_err_warn("elfp_ehdr_dump", "elfp_ehdr_get() failed");
		return -1;
	}

	/* Now that we have the header, let us parsing. */
	printf("==================================================\n");
	printf("ELF Header: \n");
	i = 0;

	/*
	 * e_ident array
	 */
	e_ident = (unsigned char *)ehdr;
	
	/* Print the entire array */
	printf("%02u. ELF Identifier: ", i++);
	for(unsigned j = 0; j < EI_NIDENT; j++)
	{
		printf("%02x ", e_ident[j]);
	}
	printf("\n");
	

	/* 1. Class */
	printf("%02u. Class: %s\n", i++, elfp_ehdr_decode_class(e_ident[EI_CLASS]));

	/* 2. Data encoding */
	printf("%02u. Data Encoding: %s\n", i++, elfp_ehdr_decode_dataenc(e_ident[EI_DATA]));

	/* 3. Version */
	printf("%02u. ELF Version: %s\n", i++, elfp_ehdr_decode_version(e_ident[EI_VERSION]));

	/* 4. OS-ABI */
	printf("%02u. OS/ABI: %s\n", i++, elfp_ehdr_decode_osabi(e_ident[EI_OSABI]));

	/* Rest of e_ident is 0s */

	/* Based on e_ident, I know if this is a 32-bit or a
	 * 64-bit ELF object. */
	
	switch(e_ident[EI_CLASS])
	{
		case ELFCLASS32:
			elfp_dump_e32hdr(ehdr);
			break;

		case ELFCLASS64:
			elfp_dump_e64hdr(ehdr);
			break;

		/* Invalid cases are considered to be 64-bit
		 * objects */
		default:
			elfp_dump_e64hdr(ehdr);
	}
	
	printf("==================================================\n");

	return 0;
}

unsigned long int
elfp_ehdr_class_get(int handle)
{
	if(elfp_sanitize_handle(handle) == -1)
	{
		elfp_err_warn("elfp_ehdr_class_get",
				"Handle failed the sanity test");
		return ELFCLASSNONE;
	}

	int ret;
	elfp_main *main = NULL;
	unsigned long int class;

	main = elfp_main_vec_get_em(handle);
	if(main == NULL)
	{
		elfp_err_warn("elfp_ehdr_class_get",
				"elfp_main_vec_get_em() failed");
		return ELFCLASSNONE;
	}

	class = elfp_main_get_class(main);
	return class;
}


/*
 * Decode functions.
 *
 * Refer elfp.h for function declaration and description.
 */
const char*
elfp_ehdr_decode_class(unsigned long int class)
{
	switch(class)
	{
		case ELFCLASS32:
			return "32-bit object";

		case ELFCLASS64:
			return "64-bit object";

		/* Everything else is invalid */
		default:
			return "Invalid class";
	}
}

const char*
elfp_ehdr_decode_dataenc(unsigned long int data_enc)
{
	switch(data_enc)
	{
		case ELFDATA2LSB:
			return "2's complement, little endian";

		case ELFDATA2MSB:
			return "2's complement, big endian";

		/* Anything else is invalid */
		default:
			return "Invalid Data Encoding";
	}
}

const char*
elfp_ehdr_decode_version(unsigned long int version)
{
	switch(version)
	{
		case EV_CURRENT:
			return "Current version";
		
		/* Everything else is invalid */
		default:
			return "Invalid ELF version";
	}
}

const char*
elfp_ehdr_decode_osabi(unsigned long int osabi)
{
	switch(osabi)
	{
		case ELFOSABI_SYSV: /* ELFOSABI_NONE */
			return "Unix System V ABI";

		case ELFOSABI_HPUX:
			return "HP-UX";

		case ELFOSABI_NETBSD:
			return "NetBSD";
		
		case ELFOSABI_GNU: /* ELFOSABI_LINUX */
			return "Object uses GNU ELF extensions";

		case ELFOSABI_SOLARIS:
			return"Sun Solaris";

		case ELFOSABI_AIX:
			return "IBM AIX";

		case ELFOSABI_IRIX:
			return "SGI Irix";

		case ELFOSABI_FREEBSD:
			return "FreeBSD";

		case ELFOSABI_TRU64:
			return "Compaq TRU64 UNIX";

		case ELFOSABI_MODESTO:
			return "Novell Modesto";

		case ELFOSABI_OPENBSD:
			return "OpenBSD";

		case ELFOSABI_ARM_AEABI:
			return "ARM EABI";

		case ELFOSABI_ARM:
			return "ARM";

		case ELFOSABI_STANDALONE:
			return "Standalone(embedded) application";

		/* Anything else is invalid */
		default:
			return "Invalid OSABI";
	}
}

const char*
elfp_ehdr_decode_type(unsigned long int type)
{
	switch(type)
	{
		case ET_REL:
			return "(REL) Relocatable file";

		case ET_EXEC:
			return "(EXEC) Executable file";

		case ET_DYN:
			return "(DYN) Shared object file";

		case ET_CORE:
			return "(CORE) Core file";
		
		/* Anything else is trash */
		default:
			return "Invalid ELF file type";
	}
}

const char*
elfp_ehdr_decode_machine(unsigned long int machine)
{
	switch(machine)
	{
		case EM_M32:
			return "AT&T WE 32100";

		case EM_SPARC:
			return "SUN SPARC";

		case EM_386:
			return "Intel 80386";
			break;

		case EM_68K:
	 		return "Motorola m68k family";

		case  EM_88K:
			return "Motorola m88k family";

		case EM_860:
			return "Intel 80860";

		case EM_MIPS:
			return "MIPS R3000 big-endian";

		case EM_S370:
			return "IBM System/370";

		case EM_MIPS_RS3_LE:
			return "MIPS R3000 little-endian";

		case EM_PARISC:
			return "HPPA";
			break;

		case EM_VPP500:
			return "Fujitsu VPP500";
			break;

		case EM_SPARC32PLUS:
			return "Sun's \"v8plus\"";

		case EM_960:
			return "Intel 80960";

		case EM_PPC:
			return "PowerPC";

		case EM_PPC64:
			return "PowerPC 64-bit";

		case EM_S390:
			return "IBM S390";

		case EM_V800:
			return "NEC V800 series";

		case EM_FR20:
			return "Fujitsu FR20";

		case EM_RH32:
			return "TRW RH-32";

		case EM_RCE:
			return "Motorola RCE";

		case EM_ARM:
			return "ARM";

		case EM_FAKE_ALPHA:
			return "Digital Alpha";
		
		case EM_SH:
			return "Hitachi SH";

		case EM_SPARCV9:
			return "SPARC v9 64-bit";

		case EM_TRICORE:
			return "Siemens Tricore";

		case EM_ARC:
			return "Argonaut RISC Core";

		case EM_H8_300:
			return "Hitachi H8/300";

		case EM_H8_300H:
			 return "Hitachi H8/300H";

		case EM_H8S:
			return "Hitachi H8S";

		case EM_H8_500:
			return "Hitachi H8/500";

		case EM_IA_64:
			return "Intel Merced";

		case EM_MIPS_X:
			return "Stanford MIPS-X";

		case EM_COLDFIRE:
			return "Motorola Coldfire";

		case EM_68HC12:
			return "Motorola M68HC12";

		case EM_MMA:
			return "Fujitsu MMA Multimedia Accelerator";
		
		case EM_PCP:
			return "Siemens PCP";

		case EM_NCPU:
			return "Sony nCPU embeeded RISC";

		case EM_NDR1:
			return "Denso NDR1 microprocessor";

		case EM_STARCORE:
			return "Motorola Start*Core processor";

		case EM_ME16:
			return "Toyota ME16 processor";

		case EM_ST100:
			return "STMicroelectronic ST100 processor";

		case EM_TINYJ:
			return "Advanced Logic Corp. Tinyj emb.fam";

		case EM_X86_64:
			return "AMD x86_64 architecture";

		case EM_PDSP:
			return "Sony DSP Processor";

		case EM_FX66:
			return "Siemens FX66 microcontroller";

		case EM_ST9PLUS:
			return "STMicroelectronics ST9+ 8/16 mc";

		case EM_ST7:
			return "STmicroelectronics ST7 8 bit mc";

		case EM_68HC16:
			return "Motorola MC68HC16 microcontroller";

		case EM_68HC11:
			return "Motorola MC68HC11 microcontroller";

		case EM_68HC08:
			return "Motorola MC68HC08 microcontroller";
		
		case EM_68HC05:
			return "Motorola MC68HC05 microcontroller";
		
		case EM_SVX:
			return "Silicon Graphics SVx";
		
		case EM_VAX:
			return "Digital VAX";
		
		case EM_CRIS:
			return "Axis Communications 32-bit embedded processor";
		
		case EM_JAVELIN:
			return "Infineon Technologies 32-bit embedded processor";

		case EM_FIREPATH:
			return "Element 14 64-bit DSP Processor";

		case EM_ZSP:
			return "LSI Logic 16-bit DSP Processor";

		case EM_MMIX:
			return "Donald Knuth's educational 64-bit processor";
	
		case EM_HUANY:
			return "Harvard University machine-independent object files";
		
		case EM_PRISM:
			return "SiTera Prism";
		
		case EM_AVR:
			return "Atmel AVR 8-bit microcontroller";

		case EM_FR30:
			return "Fujitsu FR30";
		
		case EM_D10V:
			return "Mitsubishi D10V";

		case EM_D30V:
			return "Mitsubishi D30V";

		case EM_V850:
			return "NEC v850";

		case EM_M32R:
			return "Mitsubishi M32R";

		case EM_MN10300:
			return "Matsushita MN10300";

		case EM_MN10200:
                	return "Matsushita MN10200";

		case EM_PJ:
			return "picoJava";
		
		case EM_OPENRISC:
			return "OpenRISC 32-bit embedded processor";

		case EM_ARC_A5:
			return "ARC Cores Tangent-A5";

		case EM_XTENSA:
			return "Tensilica Xtensa Architecture";

		case EM_ALTERA_NIOS2:
			return "Altera Nios II";

		case EM_AARCH64:
			return "ARM AARCH64";

		case EM_TILEPRO:
			return "Tilera TILEPro";

		case EM_MICROBLAZE:
			return "Xilinx MicroBlaze";

		case  EM_TILEGX:
			return "Tilera TILE-Gx";

		default:
			return "Unknown machine / architecture";
	}
}
