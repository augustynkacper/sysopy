#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define BLOCK_SIZE 1024;

typedef int pid;
pid x = 4;

char *read_file;
char *write_file;

void print_time(clock_t start, clock_t end, FILE *f, char* v){
    //printf("%s execution time: %f\n", v, (double)(end - start) / CLOCKS_PER_SEC);
    fprintf(f,"%s execution time: %f\n", v, (double)(end - start) / CLOCKS_PER_SEC);
}

int parse_input(int argc, char *argv[]){
    if ( argc != 3){
        printf("Program accepts exactly 2 arguments!\n");
        return 0;
    }
    read_file = argv[1];
    write_file = argv[2];
    return 1;
}

void reverse(int block_size){

    FILE *read, *write;
    read = fopen(read_file, "r");
    write = fopen(write_file, "w");

    if (read==NULL){
        fclose(write);
    }

    if (read==NULL || write==NULL){
        printf("Couldn't open a file!\n");
        return;
    }


    

    char buffer[block_size+1];

    // move pointer to the end of file
    // and get file size
    fseek(read, 0L, SEEK_END);

    fpos_t pos;
    fgetpos(read, &pos);

    // get number of blocks of given size
    int blocks = (pos+1)/block_size ;
    int size_read;

    for (int i=blocks; i>=0; i--){

        // move pointer
        fseek(read, i*block_size, SEEK_SET);

        // count how many chars were read, and 
        // mark and of string
        size_read = fread(buffer, sizeof(char), block_size, read);

        // read from end
        for(int j=size_read-1; j>=0; j--){
            //printf("%c", buffer[j]);
            fwrite(&buffer[j], 1, 1, write);
        }
    }

    fclose(read);
    fclose(write);
    //printf("\n\n");
}


int main(int argc, char *argv[]){

    if (parse_input(argc, argv)!=1)
        return 0;

    clock_t size_1_start, size_1_end;
    clock_t size_1024_start, size_1024_end;


    size_1_start = clock();
    reverse(1);
    size_1_end = clock();

    size_1024_start = clock();
    reverse(1024);
    size_1024_end = clock();


    FILE* results_file;
    results_file = fopen("pomiar_zad_2.txt", "w");

    print_time(size_1_start, size_1_end, results_file, "blocks of size 1");
    print_time(size_1024_start, size_1024_end, results_file, "blocks of size 1024");

    fclose(results_file);
    
    return 0;
}


