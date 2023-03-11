


compile:
	@gcc -Wall main.c -o reverse 
	@echo "To run:"
	@echo "./reverse [in_file] [out_file]"



test:
	@echo "testing file reversing"
	@gcc -Wall main.c -o reverse 
	@./reverse base_file.txt out.txt
	@./reverse out.txt comp.txt
	@diff -s comp.txt base_file.txt 


clean:
	rm -rf *.a *.o