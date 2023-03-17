
CC = gcc
CFLAGS = -Wall

compile:
	@$(CC) $(CFLAGS) main.c -o reverse 
	@echo "To run:"
	@echo "./reverse [in_file] [out_file]"



test:
	@echo "testing file reversing"
	@$(CC) $(CFLAGS) main.c -o reverse 
	@./reverse Cat03.jpg out6.jpg
	@./reverse out6.jpg comp.jpg
	@diff -s comp.jpg Cat03.jpg 


clean:
	rm -rf *.a *.o