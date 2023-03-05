

all: static shared

static:
	gcc -c lib.c
	ar rcs staticlib.a lib.o

shared:
	gcc -fPIC -c lib.c
	gcc -shared lib.o -o sharedlib.so


clean:
	rm -rf *.o *.a *.so

