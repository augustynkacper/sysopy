#include "lib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>

BlocksArray* create_blocks_array(int l){

    BlocksArray* res;
    res = malloc(sizeof(BlocksArray));

    res->max_length = l;
    res->current_length = 0;
    res->blocks = calloc(l, sizeof(Block));

    printf("   Initialized array of size %d.\n", l);
    return res;
}

int find_index(BlocksArray* ba){
    int i=0;
    while (i<ba->max_length && ba->blocks[i]!=NULL) { 
        i++;
    }
    return i;
}

void count_file_stats(BlocksArray* ba, char* filename){

    char* c1 = "wc "; // first part of a command
    char* c2 = " > tmp/tmp.txt"; // second part of a command
    char* command = malloc(strlen(c1)+strlen(c2)+strlen(filename)+7);

    strcpy(command, c1);
    strcat(command, filename);
    strcat(command, c2);  

    // if tmp directory doesnt exists, create one
    DIR* dir = opendir("tmp");
    if(!dir) {
        system("mkdir tmp");
    }

    // run command
    system(command);
    
    // reading from file
    FILE* file_stream;
    file_stream = fopen("tmp/tmp.txt", "r");

    char* tmp_txt=calloc(100, sizeof(char));  // variable which stores wc result
    fgets(tmp_txt, 100, file_stream);

    fclose(file_stream);

    // get info about files to relevant variables
    char* p;
    int lines=strtol(strtok(tmp_txt, " "),&p,10), 
        words=strtol(strtok(NULL, " "),&p,10), 
        chars=strtol(strtok(NULL, " "),&p,10);

    // create a new block
    Block* block = calloc(1, sizeof(Block));

    block->lines=lines;
    block->words=words;
    block->chars=chars;
    block->filename=calloc(100, sizeof(char));
    strcpy(block->filename, filename);

    // add block to blocks array
    int index = find_index(ba);

    ba->blocks[index] = block;
    ba->current_length++;

    // remove tmp file
    system("rm -r tmp/*");

    printf("   File info saved at index %d.\n", index);
}

void free_blocks_array(BlocksArray* ba){
    // free every block in blocks array
    for (int i=0; i<ba->current_length; i++){
        free(ba->blocks[i]);
        ba->blocks[i]=NULL;
    }
    // free the array pointer
    free(ba->blocks);
    ba->blocks = NULL;

    ba->current_length=0;

    printf("   Array memory freed.\n");
}

void free_block(BlocksArray* ba, int index){
    if (index<0 || index >= ba->max_length){
        printf("  Given index out of array bounds!");
        return;
    }
    if (ba->blocks[index]==NULL){
        printf("   Block doesn't exist!");
        return;
    }
    free(ba->blocks[index]);
    ba->blocks[index] = NULL;

    ba->current_length--;

    printf("   Block at index %d removed.\n", index);
}


Block* get_block(BlocksArray* ba, int i){
    if (i<0 || i >= ba->max_length){
        fprintf(stderr, "   Given index out of array bounds!");
        return NULL;
    }
    if (ba->blocks[i]==NULL) {
        printf("   Array is empty at this index!\n");
        return NULL;
    }
   
    printf("   Block at index %d found.\n", i);
    return ba->blocks[i];
}

/*
int main(){
    
    BlocksArray* ba = create_blocks_array(5);

    count_file_stats(ba, "file1.txt");
    count_file_stats(ba, "file2.txt");
    count_file_stats(ba, "file3.txt");

    printf("%s\n", ba->blocks[0]->filename);
    printf("%s\n", ba->blocks[1]->filename);
    printf("%s\n", ba->blocks[2]->filename);
    printf("%d\n", ba->current_length);

    

    free_blocks_array(ba);
    
    printf("%d\n", ba->current_length);
    return 0;
}

*/