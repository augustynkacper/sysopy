#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


char *char_to_convert;
char *target_char;
char *read_file;
char *write_file;

int parse_input(int argc, char *argv[]){
    if ( argc != 5){
        printf("Program accepts exactly 4 arguments!\n");
        return 0;
    }
    
    if (strlen(argv[1])!=1 || strlen(argv[2])!=1){
        printf("One of first 2 arguments are in wrong format!\n");
        return 0;
    }

    char_to_convert = argv[1];
    target_char = argv[2];
    read_file = argv[3];
    write_file = argv[4];

    return 1;
}

void lib_version(){

    FILE *r_f, *w_f;
    r_f = fopen(read_file, "r");
    w_f = fopen(write_file, "w");

    if (r_f == NULL || w_f == NULL){
        printf("Couldn't open a file!\n");
        exit(0);
    }

    char c;
    while(fread(&c, 1, 1, r_f)){
        if (c == *char_to_convert)
            fwrite(target_char, 1, 1, w_f);
        else 
            fwrite(&c, 1, 1, w_f);   
    }

    fclose(w_f);
    fclose(r_f);
}

void sys_version(){

    int r_f, w_f;
    r_f = open(read_file, O_RDONLY);
    w_f = open(write_file, O_WRONLY);

    if ( r_f<0 || w_f<0){
        printf("Couldn't open a file!\n");
        return;
    }

    char c;
    while ( read(r_f, &c, 1) ){
        if ( c == *char_to_convert )
            write(w_f, target_char, 1);
        else 
            write(w_f, &c, 1);
    }


}

void print_time(clock_t start, clock_t end, FILE *f){
    printf("Execution time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);
    fprintf(f,"Execution time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);
}

int main(int argc, char *argv[]){

    if (parse_input(argc, argv)==0)
        return 0;
    
    clock_t sys_start, sys_end;
    clock_t lib_start, lib_end;
    
    FILE *results_file;
    results_file = fopen("pomiar_zad_1.txt", "w");

    // sys version
    sys_start = clock();
    sys_version();
    sys_end = clock();
    print_time(sys_start, sys_end, results_file);

    // lib version
    lib_start = clock();
    lib_version();
    lib_end = clock();
    print_time(lib_start, lib_end, results_file);

    
    fclose(results_file);

    return 0;
}