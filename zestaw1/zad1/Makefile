

all: static shared

static:
	gcc -c lib.c
	ar rcs staticlib.a lib.o
	@echo "static library compiled..."

shared:
	gcc -fPIC -c lib.c
	gcc -shared lib.o -o sharedlib.so
	@echo "shared library compiled..."



clean:
	rm -rf *.o *.a *.so

