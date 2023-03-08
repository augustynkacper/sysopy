
all: test_static test_shared test_dynamic

static:
	gcc -c ../zad1/lib.c 
	ar rcs staticlib.a lib.o 
	gcc -c ../zad2/main.c 
	gcc -Wall main.o staticlib.a -o run_static

shared:
	gcc -fPIC -c ../zad1/lib.c
	gcc -shared lib.o -o libshared.so
	gcc -c ../zad2/main.c
	gcc -Wall main.o -L. -Wl,-rpath=. -lshared  -o run_shared 

dynamic:
	gcc -fPIC -c ../zad1/lib.c
	gcc -shared lib.o -o lib_dynamic.so
	gcc -Wall ../zad2/main.c -o run_dynamic -ldl -D DYNAMIC



test_static:
	make static
	@echo "*** STATIC LIBRARY TEST ***" > results_static.txt
	./run_static < commands.txt >> results_static.txt
	@echo "dynamic libary test in file results_static.txt"

test_shared:
	make shared
	@echo "*** SHARED LIBRARY TEST ***" > results_shared.txt
	./run_shared < commands.txt >> results_shared.txt
	@echo "dynamic libary test in file results_shared.txt"

test_dynamic:
	make dynamic
	@echo "*** DYNAMIC LIBRARY TEST ***" > results_dynamic.txt
	./run_dynamic < commands.txt >> results_dynamic.txt

	@echo "dynamic libary test in file results_dynamic.txt"



clean:
	rm -rf *.o *.a *.o *.so