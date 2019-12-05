/*
 * File: check_elfp_main.c
 *
 * Description:
 *
 * 1. Tests the elfp_main implementation.
 *
 * 2. Compilation.
 * 	$ gcc check_elfp_main.c ../src/elfp_int.c -o check_elfp_main
 *
 * 3. How do you verify the output?
 * 	a. The file size can be obtained with "ls -l". That file size
 * 	and the one this program shows should be the same.
 *
 * 	b. You should see that the path is same.
 *
 * 	c. Go to /proc/PID/. Checkout the "fd" directory.
 * 		* Make sure the file descriptor displayed by this
 * 		program points to the file you passed to this program.
 *
 * 	d. Go to /proc/PID/maps. Make sure there is a "r--p" mapping
 * 	which matches your file.
 *
 * XXX: I am assuming that the free_vector member is functioning properly.
 * 	You can checkout check_free_list.c example for that.
 *
 * 	* Not bothering about the handle now.
 *
 * Example:
 *
 * $ ELF-Parser/examples$ gcc check_elfp_main.c ../src/elfp_int.c -o check_elfp_main
 * $ ELF-Parser/examples$ ./check_elfp_main ./check_elfp_main
 * fd = 3
 * Path = ./check_elfp_main
 * File size = 18064
 * Start address = 0x7fa383554000
 * PID = 8912
 *
 * a. File size: Note that they match.
 * ELF-Parser/examples$ ls -l check_elfp_main
 * -rwxr-xr-x 1 dell dell 18064 Dec  6 01:07 check_elfp_main
 *
 * b. Path is the same
 *
 * c. Go to /proc/8912 directory.
 *
 * i. Go to /proc/8912/fd and checkout descriptor 3.
 * /proc/8912/fd$ ls -l 3
 * lr-x------ 1 dell dell 64 Dec  6 01:11 3 -> /home/dell/Documents/projects/write-your-own-XXXX/ELF-Parser/examples/check_elfp_main
 *
 * ii. Checkout /proc/8912/maps file. The following mapping is the output of our mmap().
 * 7fa383554000-7fa383559000 r--p 00000000 08:05 7211572 			/home/dell/Documents/projects/write-your-own-XXXX/ELF-Parser/examples/check_elfp_main
 * 
 *
 * 4. Making sure there are no memory leaks.
 * 	* Comment the while(1); loop, 
 *	* Comment the elfp_main_vec_inform() line of code in elfp_main_fini().
 *	* And then recompile the program and run it again.
 *
 * * I ran it for a bunch of different ELF files. There were no memory leaks.
 *
 * 	* Don't forget to uncomment elfp_main_vec_inform()!
 *
 * The above helps you test the happy path. If you want to take a look
 * at the error handling, give wierd inputs.
 *
 * $ ./check_elfp_main does_not_exist.txt
 * $ ./check_elfp_main check_elfp_main.c
 *
 * and more!
 */

#include "../src/include/elfp_int.h"
#include "../src/include/elfp_err.h"

#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	
	if(argc != 2)
	{
		fprintf(stdout, "Usage: $ %s <elf-file-path>\n", argv[0]);
		return -1;
	}

	elfp_main *main = NULL;
	int ret;
	const char *file_path = argv[1];

	main = elfp_main_create(file_path);
	if(main == NULL)
	{
		elfp_err_warn("main", "elfp_main_create() failed");
		return -1;
	}

	/* Now, let us inspect the members and verify them */

	/* 1. File descriptor */
	printf("fd = %d\n", main->fd);

	/* 2. Path */
	printf("Path = %s\n", main->path);

	/* 3. File size */
	printf("File size = %lu\n", main->file_size);

	/* 4. Start address */
	printf("Start address = %p\n", main->start_addr);
	
	/* I am assuming that the free_vector is working properly.
	 * It has been tested already */
	
	/* Let us checkout /proc/PID/maps. It'll help us verify
	 * all the details */
	printf("PID = %d\n", getpid());
	
	/* Comment the below infinite loop while checking for memory
	 * leaks */
	/* while(1); */

	elfp_main_fini(main);

	return 0;
}
