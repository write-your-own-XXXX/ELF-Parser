CC = gcc
CFLAGS = -fstack-protector-all -O2

# All the built files(object files and library) will be found in the build directory.
# Finally, check src/build directory.
build: 
	# Building the library
	$(CC) elfp_int.c elfp_basic_api.c elfp_ehdr.c -c -fPIC $(CFLAGS)
	$(CC) elfp_int.o elfp_basic_api.o elfp_ehdr.o -shared $(CFLAGS) -o libelfp.so
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

# Uninstalls the library and cleans up everything.
uninstall:
	sudo rm /usr/lib/x86_64-linux-gnu/libelfp.so
	sudo rm /usr/include/elfp/elfp.h
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

# Cleans up examples directory.
examples_clean:
	rm -r ../examples/build
