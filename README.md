# libelfp: An ELF Parsing Library

**libelfp** is a simple ELF Parsing Library written to understand ELF better.

**NOTE**: This project is still under development.

## 1. Building the library

In the ```src``` directory,

```
ELF-Parser/src$ make build
# Building the library
gcc elfp_int.c elfp_basic_api.c elfp_ehdr.c -c -fPIC -fstack-protector-all -O2
gcc elfp_int.o elfp_basic_api.o elfp_ehdr.o -shared -fstack-protector-all -O2 -o libelfp.so
```

Have a look at ```src/build``` directory. It'll have the shared object.

```
ELF-Parser/src$ ls ./build
elfp_basic_api.o  elfp_ehdr.o  elfp_int.o  libelfp.so
```

In order to use the library, it is better to install it rather than just build it.

## 2. Installing the library

```
ELF-Parser/src$ make install
make build
make[1]: Entering directory '/home/dell/Documents/projects/write-your-own-XXXX/ELF-Parser/src'
make[1]: 'build' is up to date.
make[1]: Leaving directory '/home/dell/Documents/projects/write-your-own-XXXX/ELF-Parser/src'
sudo cp build/libelfp.so /usr/lib/x86_64-linux-gnu
sudo mkdir /usr/include/elfp
sudo cp ./include/elfp.h /usr/include/elfp
```

* The library's API is present in ```elfp/elfp.h```, which you can refer to.

## 3. Writing your own application

```libelfp``` offers a very simple and easy-to-use API. We can parse ELFs in no time. Let us write an application to experience it.

Let us refer to the header file ```elfp/elfp.h``` while writing it. Let us call the application ```elfparse```. Open up a C sourcefile ```elfparse.c```.

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

**6.** Finally, de-init the library and come out.

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

## 4. What are the library features?

1. Simple, easy-to-use API
2. Multiple ELF files can be opened for processing at once.
3. The library is still a baby. It can only parse both 32-bit and 64-bit ELF headers as of now. Code to parse other ELF-related data structures will be gradually added.

## 5. Bugs?

If you find any bug, if you find the library crashing for a wierd input, create an issue.
