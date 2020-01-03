# libelfp: An ELF Parsing Library

**libelfp** is a simple ELF Parsing Library written to understand ELF better.

**NOTE**: This project is till under development.

## Installing the library

1. Clone the library.

```
$ git clone https://github.com/write-your-own-XXXX/ELF-Parser.git
Cloning into 'ELF-Parser'...
remote: Enumerating objects: 147, done.
remote: Counting objects: 100% (147/147), done.
remote: Compressing objects: 100% (94/94), done.
remote: Total 147 (delta 71), reused 120 (delta 47), pack-reused 0
Receiving objects: 100% (147/147), 43.86 KiB | 197.00 KiB/s, done.
Resolving deltas: 100% (71/71), done.
$
```

2. Installing the library.

```
ELF-Parser/src$ make install
```

You are good to go!

3. Building the library

If you just want to build the library, instead of installing it, you can do the same.

```
ELF-Parser/src$ make build
```

## Functionalities

1. You can open up multiple files at once and process it. You can write a standalone ELF parsing tool or use our library elsewhere where you would need ELF parsing capabilities.
2. The Library can parse the following data structures:
	* ELF header
	* Program Header
	* Program Header Table
	* Dump INTERP and GNU_STACK segment types.

The library is still a baby. Functionalities will be continuously added.

## Writing an ELF Parser

```libelfp``` offers a very simple and easy-to-use API. We can parse ELF files in no time. Let us write a simple parser.

API is available in ```elfp/elfp.h```. Lets write ```elfparse```!

**1.** Header files

It is essential that you include ```elfp/elfp.h```.

```c
#include <stdio.h>
#include <elfp/elfp.h>
```

**2.** Initial code

The application takes a file path as argument. That file path is given to our library for parsing.

```c
int 
main(int argc, char **argv)
{
        if(argc != 2)
        {
                fprintf(stdout, "Usage: $ %s <elf-file-path>\n", argv[0]);
                return -1;
        }

        int ret;
        int fd;
        const char *path = argv[1];
```

**3.** Initialize the library

```c
        /* Initialize the library */
        ret = elfp_init();
        if(ret == -1)
        {
                fprintf(stderr, "Unable to initialize libelfp.\nExiting..\n");
                return -1;
        }
```

This makes the library usable. ```elfp_init()``` should be called before any other library function is called.

**4.** Opening the file

```c
        /* Open up the ELF file for processing */
        ret = elfp_open(path);
        if(ret == -1)
        {
                fprintf(stderr, "Unable to open file using libelfp.\nExiting..\n");
                elfp_fini();
                return -1;
        }

        fd = ret;
```

Similar to Linux, ```libelfp``` returns a file descriptor (or handle) to manage that open file.

**5.** Let us dump the ELF header details.

```c
        /* Dump the ELF header */
        elfp_ehdr_dump(fd);
```

**6.** You can also dump the Program Header Table.

```c
	/* Dump the PHT */
        elfp_pht_dump(fd);
```

**7.** Finally, de-init the library and come out.

```c
        /* De-init the library */
        elfp_fini();

        return 0;
}
```

Our ELF parsing application is ready. Let us compile it.

```
ELF-Parser/src$ gcc elfparse.c -o elfparse -lelfp
ELF-Parser/src$ ls -l elfparse
-rwxr-xr-x 1 dell dell 8616 Dec  8 02:21 elfparse
```

Let us run it on a few ELF files.

```
ELF-Parser/src$ ./elfparse /bin/ls
==================================================
ELF Header: 
00. ELF Identifier: 7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
01. Class: 64-bit object
02. Data Encoding: 2's complement, little endian
03. ELF Version: Current version
04. OS/ABI: Unix System V ABI
05. ELF Type: (DYN) Shared object file
06. Architecture: AMD x86_64 architecture
07. Entry Address: 0x5850
08. Program Header Table file offset: 64 bytes
09. Section Header Table file offset: 132000 bytes
10. Flags: 0
11. ELF header size: 64 bytes
12. Program Header size: 56 bytes
13. Program Headers count: 9
14. Section Header size: 64 bytes
15. Section Header count: 28
16. Section Header String Table index: 27
==================================================

==================================================
Program Header Table: 

Entry 00: 
00. Type: PHDR (Entry for header table)
01. Flags: r-x
02. Segment file offset: 64 bytes
03. Virtual Address: 0x40
04. Physical Address: 0x40
05. Segment size in file: 504 bytes
06. Segment size in memory: 504 bytes
07. Segment Alignment: 0x8
---------------------------------------------
Entry 01: 
00. Type: INTERP (Program Interpreter)
01. Flags: r--
02. Segment file offset: 568 bytes
03. Virtual Address: 0x238
04. Physical Address: 0x238
05. Segment size in file: 28 bytes
06. Segment size in memory: 28 bytes
07. Segment Alignment: 0x1
---------------------------------------------
Entry 02: 
00. Type: LOAD (Loadable program segment)
01. Flags: r-x
02. Segment file offset: 0 bytes
03. Virtual Address: 0x0
04. Physical Address: 0x0
05. Segment size in file: 124648 bytes
06. Segment size in memory: 124648 bytes
07. Segment Alignment: 0x200000
---------------------------------------------
Entry 03: 
00. Type: LOAD (Loadable program segment)
01. Flags: rw-
02. Segment file offset: 126960 bytes
03. Virtual Address: 0x21eff0
04. Physical Address: 0x21eff0
05. Segment size in file: 4728 bytes
06. Segment size in memory: 9584 bytes
07. Segment Alignment: 0x200000
---------------------------------------------
Entry 04: 
00. Type: DYNAMIC (Dynamic Linking information)
01. Flags: rw-
02. Segment file offset: 129592 bytes
03. Virtual Address: 0x21fa38
04. Physical Address: 0x21fa38
05. Segment size in file: 512 bytes
06. Segment size in memory: 512 bytes
07. Segment Alignment: 0x8
---------------------------------------------
Entry 05: 
00. Type: NOTE (Auxillary Information)
01. Flags: r--
02. Segment file offset: 596 bytes
03. Virtual Address: 0x254
04. Physical Address: 0x254
05. Segment size in file: 68 bytes
06. Segment size in memory: 68 bytes
07. Segment Alignment: 0x4
---------------------------------------------
Entry 06: 
00. Type: GNU_EH_FRAME (GCC .eh_frame_hdr segment)
01. Flags: r--
02. Segment file offset: 111008 bytes
03. Virtual Address: 0x1b1a0
04. Physical Address: 0x1b1a0
05. Segment size in file: 2180 bytes
06. Segment size in memory: 2180 bytes
07. Segment Alignment: 0x4
---------------------------------------------
Entry 07: 
00. Type: GNU_STACK (Indicates stack executability)
01. Flags: rw-
02. Segment file offset: 0 bytes
03. Virtual Address: 0x0
04. Physical Address: 0x0
05. Segment size in file: 0 bytes
06. Segment size in memory: 0 bytes
07. Segment Alignment: 0x10
---------------------------------------------
Entry 08: 
00. Type: GNU_RELRO (Read-only after relocation)
01. Flags: r--
02. Segment file offset: 126960 bytes
03. Virtual Address: 0x21eff0
04. Physical Address: 0x21eff0
05. Segment size in file: 4112 bytes
06. Segment size in memory: 4112 bytes
07. Segment Alignment: 0x1
---------------------------------------------
```

A run on a 32-bit Intel shared object.

```
ELF-Parser/src$ ./elfparse ./hello
==================================================
ELF Header: 
00. ELF Identifier: 7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00 
01. Class: 32-bit object
02. Data Encoding: 2's complement, little endian
03. ELF Version: Current version
04. OS/ABI: Unix System V ABI
05. ELF Type: (DYN) Shared object file
06. Architecture: Intel 80386
07. Entry Address: 0x3e0
08. Program Header Table file offset: 52 bytes
09. Section Header Table file offset: 6056 bytes
10. Flags: 0
11. ELF header size: 52 bytes
12. Program Header size: 32 bytes
13. Program Headers count: 9
14. Section Header size: 40 bytes
15. Section Header count: 29
16. Section Header String Table index: 28
==================================================
```

Your system would be full of ELF files. Use ```libelfp``` to learn more about them!

## 5. Bugs?

If you find any bug, if you find the library crashing for a wierd input, create an issue.
