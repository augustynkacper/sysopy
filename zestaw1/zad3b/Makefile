

all: static_opt shared_opt dynamic_opt

static:
	gcc -c ../zad1/lib.c 
	ar rcs staticlib.a lib.o 
	gcc -c ../zad2/main.c 

shared:
	gcc -fPIC -c ../zad1/lib.c
	gcc -shared lib.o -o libshared.so
	gcc -c ../zad2/main.c
	

dynamic:
	gcc -fPIC -c ../zad1/lib.c
	gcc -shared lib.o -o lib_dynamic.so


static_opt:
	make static 

	for opt in O0 O1 O2 O3 Os; do \
		gcc -Wall -$$opt main.o staticlib.a -o run;  \
		./run < commands.txt > results_static_$$opt.txt; \
	done

shared_opt:
	make shared 

	for opt in O0 O1 O2 O3 Os; do \
		gcc -Wall -$$opt main.o -L. -Wl,-rpath=. -lshared  -o run_shared ;  \
		./run < commands.txt > results_shared_$$opt.txt; \
	done


dynamic_opt:
	make dynamic

	for opt in O0 O1 O2 O3 Os; do \
		gcc -Wall -$$opt ../zad2/main.c -o run_dynamic -ldl -D DYNAMIC ; \
		./run < commands.txt > results_dynamic_$$opt.txt; \
	done


clean:
	rm -rf *.o *.a *.so