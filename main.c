#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
//#include "readFiles.c"

int main(){
    DIR* FD;
    struct dirent* s_archivo;
    FILE* archivo;
    char nombre_carpeta[PATH_MAX];
    char* nombre_archivo;
    char* text = ".txt";
    char* ret;

    //int make = mkdir("prueba", S_IRWXU | S_IRWXG | S_IRWXO);

    getcwd(nombre_carpeta, sizeof(nombre_carpeta));
    FD = opendir(nombre_carpeta);

    while ((s_archivo = readdir(FD))){

        if (!strcmp (s_archivo->d_name, "."))
            continue;
        if (!strcmp (s_archivo->d_name, ".."))
            continue;

        archivo = fopen(s_archivo->d_name, "r");
        if (archivo == NULL){
            fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
            return 1;
        }

        // Reconocer archivos .txt
        nombre_archivo = s_archivo->d_name;
        printf("%s", nombre_archivo);
        ret = strstr(nombre_archivo, text);
        if (ret != NULL){
            printf("%s", " xd");
        }

        printf("%s", "\n");
        fclose(archivo);
    }

    return 0;
}
