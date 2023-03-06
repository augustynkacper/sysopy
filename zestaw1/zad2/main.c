
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>

#include "../zad1/lib.h"


#ifdef DYNAMIC
    #include <dlfcn.h>
#endif


// converting user's input to command and argument
void get_input(char* input, char* command, char* arg){

    input[strlen(input)-1]='\0';

    strcpy(command, strtok(input, " "));

    char* p = strtok(NULL, " ");

    if(p!=NULL){
        strcpy(arg, p);
    } else{
        arg = NULL;
    }
}

// argument can be an integer or a char* depending on command
// this function handles this
int format_argument(char* command, char* arg, int* argument){

    if ( strcmp("init", command)==0 || 
        strcmp("delete",command)==0 ||
        strcmp("show", command)==0 ){

            if (arg==NULL){
                printf("  Enter integer only argument for this function!\n");
                return 0;
            }

            for(int i=0; i<strlen(arg);i++){
                if (!isdigit(arg[i])){
                    printf("  Function argument is not valid format!\n  Enter integer value only!\n");
                    return 0;
                }
            }

            char *p;
            *argument = strtol(arg, &p, 10); 
    }

    return 1;
}


// displaying execution times
void print_exec_time(clock_t st_time, clock_t en_time, struct tms *buff_st_time, struct tms *buff_en_time){
    printf("\n   Operation execution times:\n");
    printf("   REAL_TIME: %fs\n", (double)(en_time-st_time)/(double) sysconf(_SC_CLK_TCK));
    printf("   USER_TIME: %fs\n", (double)(buff_en_time->tms_utime-buff_st_time->tms_utime)/(double) sysconf(_SC_CLK_TCK));
    printf("   SYSTEM_TIME: %fs\n", (double)(buff_en_time->tms_stime-buff_st_time->tms_stime)/(double) sysconf(_SC_CLK_TCK));
}


int main(){

    #ifdef DYNAMIC
        void *library = dlopen("./lib_dynamic.so", RTLD_LAZY);

        if (!library) {
            printf("   Couldnt open the library!\n");
            return 0;
        }   

        BlocksArray* (*create_blocks_array)(int);
        void (*count_file_stats)(BlocksArray*, char*);
        Block* (*get_block) (BlocksArray*, int);
        void (*free_blocks_array )(BlocksArray*);
        void (*free_block) (BlocksArray*, int);

        create_blocks_array = dlsym(library, "create_blocks_array");
        count_file_stats = dlsym(library, "count_file_stats");
        get_block = dlsym(library, "get_block");
        free_blocks_array = dlsym(library, "free_blocks_array");
        free_block = dlsym(library, "free_block");

        if (dlerror() != NULL) { 
		    printf("   Error while opening functions!\n"); 
            return 0; 
        }

    #endif

    BlocksArray* blocks = NULL;

    clock_t real_time_start, real_time_end;
    struct tms buffer_time_start, buffer_time_end;

    
    
    char* command=calloc(100, sizeof(char));
    char* arg = calloc(100, sizeof(char));
    int argument;

    char input[100];

    while ( strcmp("exit", command)!=0 ){
        printf("\n> ");

        fgets(input, 100, stdin);

        if (strlen(input)==1){
            printf("   Enter correct command!\n");
            continue;
        }

        get_input(input, command, arg);
     
        if (format_argument(command, arg, &argument)==0)
            continue;

        real_time_start = times(&buffer_time_start);

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
            if (blocks == NULL){
                printf("  BlocksArray wasn't initialized!\n");
                continue;
            }

            free_block(blocks, argument);
        } else if(strcmp("exit", command)!=0){
            printf("   Enter correct command!\n");
            continue;
        }
        
        real_time_end = times(&buffer_time_end);
  
        if (strcmp("exit", command)!=0)
            print_exec_time(real_time_start, real_time_end, 
                            &buffer_time_start, &buffer_time_end);
        else {
            printf("   Program stopped...\n\n\n-------------------");
            exit(0);
        }
    }


    return 0; 
}