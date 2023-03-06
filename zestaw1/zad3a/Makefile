


static:
	gcc -c ../zad1/lib.c 
	ar rcs staticlib.a lib.o 
	gcc -c ../zad2/main.c 
	gcc main.o staticlib.a -o run_static

shared:
	gcc -fPIC -c ../zad1/lib.c
	gcc -shared lib.o -o libshared.so
	gcc -c ../zad2/main.c
	gcc main.o -L. -lshared  -o run_shared 

dynamic:
	gcc -fPIC -c ../zad1/lib.c
	gcc -shared lib.o -o lib_dynamic.so
	gcc ../zad2/main.c -o run_dynamic -ldl -D DYNAMIC



test_static:
	@echo "*** STATIC LIBRARY TEST ***" > results_static.txt
	./run_static < commands.txt >> results_static.txt

test_shared:
	@echo "*** SHARED LIBRARY TEST ***" > results_static.txt
	./run_shared < commands.txt >> results_shared.txt

test_dynamic:
	@echo "*** DYNAMIC LIBRARY TEST ***" > results_static.txt
	./run_dynamic < commands.txt >> results_dynamic.txt



clean:
	rm -rf *.o *.a *.o