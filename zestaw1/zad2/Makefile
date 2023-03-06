

static:
	gcc -c ../zad1/lib.c 
	ar rcs staticlib.a lib.o 
	gcc -c main.c 
	gcc main.o staticlib.a -o run_static

shared:
	gcc -fPIC -c ../zad1/lib.c
	gcc -shared lib.o -o libshared.so
	gcc -c main.c
	gcc main.o -L. -lshared  -o run_shared 

dynamic:
	gcc -fPIC -c ../zad1/lib.c
	gcc -shared lib.o -o lib_dynamic.so
	gcc main.c -o run_dynamic -ldl -D DYNAMIC

clean:
	rm -rf *.o *.a *.so