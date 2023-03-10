#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


char *read_file;
char *write_file;

int parse_input(int argc, char *argv[]){
    if ( argc != 3){
        printf("Program accepts exactly 2 arguments!\n");
        return 0;
    }
    read_file = argv[1];
    write_file = argv[2];
    return 1;
}

void read_by_1028blocks(){

    FILE *read, *write;
    read = fopen(read_file, "r");
    write = fopen(write_file, "w");

    if (read==NULL || write==NULL){
        printf("Couldn't open a file!\n");
        return;
    }


    char buffer[1024];

    // move pointer to the end of file
    fseek(read, 0L, SEEK_END);

    // set position of a pointer
    fpos_t pos;
    fgetpos(read, &pos);

    // count number of blocks of size 1024
    int blocks = (pos + 1)/1024;

    for (int i=blocks; i>=0; i--){
        fseek(read, i*blocks, SEEK_SET);
        fread(buffer, sizeof(char), 1024, read);

        for (int j=1023;i>=0;j--){
            if(buffer[j] != '\n' && buffer[j] != '\r' && buffer[j] != '\0') {
                printf("%c", buffer[j]);
            }
        }
    }




}

void read_one_by_one(){
    FILE *read, *write;
    read = fopen(read_file, "r");
    write = fopen(write_file, "w");

    if (read==NULL || write==NULL){
        printf("Couldn't open a file!\n");
        return;
    }

    fseek(read, -1, SEEK_END);
    fpos_t pos;
    fgetpos(read, &pos);

    char c;
    fread(&c, 1, 1, read);

    for (long int i=pos; i>=0; i--){
        fseek(read, i, SEEK_SET);
        fread(&c, 1, 1, read);

        fwrite(&c, 1, 1, write);
    }

}

int main(int argc, char *argv[]){

    if (parse_input(argc, argv)!=1)
        return 0;
    
    read_by_1028blocks();
    //read_one_by_one();

    return 0;
}


