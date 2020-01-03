CC = gcc
CFLAGS = -fstack-protector-all -O2

help:
	$(info "Welcome to libelfp's build system.")
	$(info ==================================)
	$(info The following can be make's arguments: )
	$(info )
	$(info 1. build: Builds the library )
	$(info ---------------------------- )
	$(info a. Build the library)
	$(info b. Puts the shared object (libelfp.so) and other object files in src/build directory.)
	$(info )
	$(info 2. install: Installs the library )
	$(info -------------------------------- )
	$(info a. make build )
	$(info b. Puts libelfp.so in /usr/lib/x86_64-linux-gnu)
	$(info c. Puts the main header file elfp.h in /usr/include/elfp)
	$(info )
	$(info 3. app: Builds the app )
	$(info ---------------------- )
	$(info a. make install)
	$(info b. Build the app )
	$(info )
	$(info 4. examples: Builds examples )
	$(info ---------------------------- )
	$(info a. You can find examples in examples/build directory )
	$(info )
	$(info 5. clean: Removes src/build, src/app, examples/build )
	$(info )
	$(info 6. uninstall: Uninstalls the library )
	$(info )

# All the built files(object files and library) will be found in the build directory.
# Finally, check src/build directory.
build: 
	# Building the library
	$(CC) elfp_ds.c elfp_int.c elfp_basic_api.c elfp_ehdr.c elfp_phdr.c elfp_seg.c -c -fPIC $(CFLAGS)
	$(CC) elfp_ds.o elfp_int.o elfp_basic_api.o elfp_ehdr.o elfp_phdr.o elfp_seg.o -shared $(CFLAGS) -o libelfp.so
	mkdir build
	mv libelfp.so *.o build

# Cleans up everything.
clean:
	rm -rf build 
	rm -rf app
	rm -rf ../examples/build

# Builds the library and makes it usable.
install:
	make build
	sudo cp build/libelfp.so /usr/lib/x86_64-linux-gnu
	sudo mkdir /usr/include/elfp
	sudo cp ./include/elfp.h /usr/include/elfp
	sudo cp ./include/elfp_ds.h /usr/include/elfp

# Uninstalls the library and cleans up everything.
uninstall:
	sudo rm /usr/lib/x86_64-linux-gnu/libelfp.so
	sudo rm /usr/include/elfp/elfp.h
	sudo rm /usr/include/elfp/elfp_ds.h
	sudo rmdir /usr/include/elfp
	make clean

# Builds the app - elfparse.
app:
	make install
	gcc elfparse.c -o elfparse -lelfp $(CFLAGS)
	mkdir app
	mv elfparse app

# Builds all the examples and dumps it in examples/build directory.
examples:
	mkdir ../examples/build
	make install
	gcc ../examples/check_elfp_main.c -o ../examples/build/check_elfp_main -lelfp
	gcc ../examples/check_free_list.c -o ../examples/build/check_free_list -lelfp
	gcc ../examples/check_main_vec.c -o ../examples/build/check_main_vec -lelfp
	gcc ../examples/check_basic_api.c -o ../examples/build/check_basic_api -lelfp
	gcc ../examples/check_elfp_phdr.c -o ../examples/build/check_elfp_phdr -lelfp
	gcc ../examples/dump_gnu_stack.c -o ../examples/build/dump_gnu_stack -lelfp
	gcc ../examples/dump_interp.c -o ../examples/build/dump_interp -lelfp
