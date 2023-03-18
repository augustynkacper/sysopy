#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>



char to_find[255];
int to_find_size;

void search_in_file(char *path){
    FILE* file = fopen(path, "r");

    if (file == NULL){
        printf("  Couldnt open a file!\n");
        return;
    }

    char buffer[to_find_size];
    size_t size_read; 
    size_read=fread(buffer, sizeof(char), to_find_size, file);

    if (size_read <0 ){
        printf("Could read a file %s\n", path);
        return;
    }
    
    for(int i=0; i<to_find_size; i++){
        if (buffer[i]!=to_find[i])
            return;
    }
    printf("%s %d\n", path, getpid());
}

void search(char* path){

    DIR* dirp;
    struct dirent* dir;

    struct stat statbuf;

    dirp = opendir(path);
    //printf("%s\n", path); fflush(stdout);

    while ((dir = readdir(dirp))!=NULL){


        char new_path[1024] = "";
        strcat(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, dir->d_name);

        if (stat(new_path, &statbuf)==0 && S_ISDIR(statbuf.st_mode)){
            if (strcmp(dir->d_name, ".")==0 || strcmp(dir->d_name, "..")==0)
                continue;

            pid_t process_id = fork();

            if (process_id ==0){

                // call searching for new directory
                search(new_path);

                // end function
                return;

            } else{
                wait( NULL );
            }
        } else {

            //printf("    file %s\n", dir->d_name);  fflush(stdout);
            search_in_file(new_path);
        }

    }

    closedir(dirp);
    
}


int main(int argc, char* argv[]){


    if (argc != 3){
        printf("Program accepts exactly 2 arguments!\n");
        return 0;
    }

    if (strlen(argv[2]) > 255){
        printf("Entered string is too long!\n");
        return 0;
    }

    strcpy(to_find, argv[2]);
    to_find_size = strlen(to_find);

    search(argv[1]);
    return 0;
}