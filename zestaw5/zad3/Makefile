CC=gcc
CFLAGS=-Wall

all:
	@make integral
	@make main 
	@echo "./main [rectangle_width] [number_of_children]"
	@echo "to run" 

integral:
	$(CC) $(CFLAGS) integral.c -o integral

main:
	$(CC) $(CFLAGS) main.c -o main

clean:
	rm -rf integral main pipe