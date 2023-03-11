all: static shared dynamic

static:
	gcc -c ../zad1/lib.c 
	ar rcs staticlib.a lib.o 
	gcc -c main.c 
	gcc -Wall main.o staticlib.a -o run_static
	@echo "main.c compiled using shared library"
	@echo "./run_static to run..."

shared:
	gcc -fPIC -c ../zad1/lib.c
	gcc -shared lib.o -o libshared.so
	gcc -c main.c
	gcc -Wall main.o -L. -Wl,-rpath=. -lshared  -o run_shared 
	@echo "main.c compiled using shared library"
	@echo "./run_shared to run..."

dynamic:
	gcc -fPIC -c ../zad1/lib.c
	gcc -shared lib.o -o lib_dynamic.so
	gcc -Wall main.c -o run_dynamic -ldl -D DYNAMIC
	@echo "main.c compiled using shared library"
	@echo "./run_dynamic to run..."

clean:
	rm -rf *.o *.a *.so




