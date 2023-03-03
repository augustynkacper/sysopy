#include "lib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

BlocksArray* create_block_array(int l){

    BlocksArray* res;
    res = calloc(1, sizeof(BlocksArray));

    res->max_length = l;
    res->current_length = 0;
    res->blocks = calloc(l, sizeof(Block));

    return res;
}

void count_file_stats(BlocksArray* ba, char* filename){

    char* c1 = "wc "; // first part of a command
    char* c2 = " > tmp/tmp.txt"; // second part of a command
    char* command = malloc(strlen(c1)+strlen(c2)+strlen(filename)+7);

    strcpy(command, c1);
    strcat(command, filename);
    strcat(command, c2);  

    // at this point we have wc command ready

    system(command);

    // reading from file
    FILE* file_stream;
    char* tmp_txt=calloc(100, sizeof(char));  // variable which stores wc result
    file_stream = fopen("tmp/tmp.txt", "r");
    fgets(tmp_txt, 100, file_stream);

    fclose(file_stream);

    char* p;

    int lines=strtol(strtok(tmp_txt, " "),&p,10), 
        words=strtol(strtok(NULL, " "),&p,10), 
        chars=strtol(strtok(NULL, " "),&p,10);

    Block* block = calloc(1, sizeof(Block));

    block->lines=lines;
    block->words=words;
    block->chars=chars;
    block->filename=filename;

    int index = ba->current_length;

    ba->blocks[index] = block;
    ba->current_length++;

    system("rm -r tmp/*");


}

/*
int main(){
    
    BlocksArray* ba = create_block_array(5);

    count_file_stats(ba, "file1.txt");
    count_file_stats(ba, "file2.txt");
    count_file_stats(ba, "file3.txt");

    printf("%s\n", ba->blocks[0]->filename);
    printf("%s\n", ba->blocks[1]->filename);
    printf("%s\n", ba->blocks[2]->filename);
    printf("%d\n", ba->current_length);


    return 0;
}

*/