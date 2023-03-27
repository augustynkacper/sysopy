#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


void search(char *path){

    struct stat statbuf;
    DIR *dirp;
    struct dirent* dir;

    printf(" %s\n", path);

    dirp = opendir(path);

    while ( (dir=readdir(dirp)) != NULL){
        stat(dir->d_name, &statbuf);

        if (!S_ISDIR(statbuf.st_mode)){
            if (strcmp(dir->d_name, ".")==0 || strcmp(dir->d_name, "..")==0 )
                continue;
            
            if ( fork() == 0 ){
               
            
                char new_path[1024]="";
                strcat(new_path, path);
                strcat(new_path, "/");
                strcat(new_path, dir->d_name);
                search(new_path);

                exit(0);
            }
            wait(NULL);
            
            //search(new_path);
        }
    }






}



int main(int argc, char* argv[]){

    if (argc!=3){
        printf("Program accepts exactly 2 arguments!\n");
        return 0;
    }

    if (strlen(argv[2]) > 255){
        printf("Second argument too long!\n");
        return 0;
    }

    search(argv[1]);

    




}