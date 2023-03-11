#include <stdio.h>
#include <ftw.h>

long long total_size = 0;


int check_file(const char *fpath, const struct stat *sb, int typeflag){

    if ( !S_ISDIR(sb->st_mode)) {
        total_size += sb->st_size;
        printf("%s %lld\n", fpath, sb->st_size);
    }

    return 0;
}


int main(int argc, char* argv[]){

    if (argc != 2){
        printf("Program accepts exactly 1 argument!");
        return 0;
    }

    int error = ftw(argv[1], check_file, 16);

    if (error==-1){
        printf("Error while searching given directory!");
        return 0;
    }

    printf("total size of files: %lld\n", total_size);


    return 0;
}