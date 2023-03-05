

static:
	gcc -c lib.c 
	ar rcs staticlib.a lib.o 
	gcc -c main.c 
	gcc main.o staticlib.a -o run

clean:
	rm -rf *.o *.a *.so