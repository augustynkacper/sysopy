

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


level_0:
	gcc -O0 main.o staticlib.a -o run
	./run < commands.txt > results

level_1:
	gcc -O1 main.o staticlib.a -o run

level_2:
	gcc -O2 main.o staticlib.a -o run

level_3:
	gcc -O3 main.o staticlib.a -o run

level_s:
	gcc -Os main.o staticlib.a -o run


static_opt:
	make static 

	for opt in O0 O1 O2 O3 Os; do \
		gcc -$$opt main.o staticlib.a -o run;  \
		./run < commands.txt > results_static_$$opt.txt; \
	done

shared_opt:
	make shared 

	for opt in O0 O1 O2 O3 Os; do \
		gcc -$$opt main.o -L. -lshared  -o run_shared ;  \
		./run < commands.txt > results_shared_$$opt.txt; \
	done


dynamic_opt:
	make dynamic

	for opt in O0 O1 O2 O3 Os; do \
		gcc -$$opt ../zad2/main.c -o run_dynamic -ldl -D DYNAMIC ; \
		./run < commands.txt > results_dynamic_$$opt.txt; \
	done

		