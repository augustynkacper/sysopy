#ifndef LAB1_ZAD1_LIB
#define LAB1_ZAD1_LIB

typedef struct Block{
    int lines;
    int words;
    int chars;
    char* filename;
} Block;

typedef struct BlocksArray{
    int max_length;
    int current_length;
    Block** blocks;
} BlocksArray;


BlocksArray* create_array(int);

void count_file_stats(BlocksArray*, char*);

Block* get_block(BlocksArray*, int);

void free_blocks_array(BlocksArray*);

#endif