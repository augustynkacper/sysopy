
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "lib.h"



void get_input(char* input, char* command, char* arg){

    strcpy(command, strtok(input, " "));


    char* p = strtok(NULL, " ");

    if(p!=NULL){
        strcpy(arg, p);
    } else{
        arg = NULL;
    }

  
}



int main(){

    BlocksArray* blocks = NULL;

    printf("%d\n", !blocks);


    char input[100];
    char check[4]="exit";
    
    char* command=calloc(100, sizeof(char));
    char* arg = calloc(100, sizeof(char));
    int argument;

    while ( strcmp(check, input)!=0 ){
        printf("> ");
        fgets(input, 100, stdin);
        input[strlen(input)-1]='\0';
       
        get_input(input, command, arg);
     
        
        if ( strcmp("init", command)==0 || 
        strcmp("delete",command)==0 ||
        strcmp("show", command)==0 ){

            if (arg==NULL){
                printf("  Enter integer only argument for this function!\n");
                continue;
            }

            for(int i=0; i<strlen(arg);i++){
                if (!isdigit(arg[i])){
                    printf("  Function argument is not valid format!\n  Enter integer only value!\n");
                    continue;
                }
            }

            char *p;
            argument = strtol(arg, &p, 10);
        }

        if (strcmp("destroy", command)==0 ){
            if (blocks == NULL){
                printf("  BlocksArray wasn't initialized!\n");
                continue;
            }

            free_blocks_array(blocks);
            free(blocks);

            blocks = NULL;
        }

        else if ( strcmp("count", command)==0 ){
            if (blocks == NULL){
                printf("  BlocksArray wasn't initialized!\n");
                continue;
            }
            count_file_stats( blocks, arg);
        }

        else if ( strcmp("init", command)==0 ){
            blocks = create_blocks_array(argument);
        }

        else if ( strcmp("show", command)==0 ){
         
            Block* b = get_block(blocks, argument);
            
            if (b==NULL){
                continue;
            }
            printf("  filename: %s, words: %d, lines: %d, chars: %d\n", 
                        b->filename, b->words, b->lines, b->chars);
        }

        else if ( strcmp("delete", command)==0 ){
            free_block(blocks, argument);
        }
        
  
    }


    return 0; 
}