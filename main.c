#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv){
    DIR* FD;
    struct dirent* in_file;
    FILE    *entry_file;

    //int make = mkdir("prueba", S_IRWXU | S_IRWXG | S_IRWXO);

    // Cambiar path
    FD = opendir("/home/sofia/github/SO");
    while ((in_file = readdir(FD))){

        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))
            continue;

        entry_file = fopen(in_file->d_name, "r");
        if (entry_file == NULL){
            fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
            return 1;
        }

        printf("%s", in_file->d_name);
        printf("%s", "\n");
        fclose(entry_file);
    }

    return 0;
}
