
CC = gcc 
CFLAGS = -Wall

compile:
	@$(CC) $(CFLAGS) main.c -o replace 
	@echo "To run:"
	@echo "./replace [char_to_replace] [desired_char] [in_file] [out_file]"

	
test: 
	@echo "replace i to x in base_file.txt and comparison with tr command"
	@$(CC) $(CFLAGS) main.c -o replace 
	@./replace i x base_file.txt out.txt 
	@tr i x < base_file.txt > tmp
	@diff -s tmp out.txt

clean:
	rm -rf *.a *.o