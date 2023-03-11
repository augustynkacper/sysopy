#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h> 


int main(){
    
    printf("\n");

    

    DIR *open_dir = opendir(".");

    struct dirent* dir;
    struct stat file_stat;

    long long total_size=0;

    while ( (dir=readdir(open_dir)) != NULL){

        // save file info
        stat(dir->d_name, &file_stat);

        // check if is directory
        if ( !S_ISDIR(file_stat.st_mode)){
            printf("%s  %lld\n", dir->d_name, file_stat.st_size);
            total_size += file_stat.st_size;
        }
    }

    printf("\nTotal size: %lld\n\n", total_size);



    return 0;
}