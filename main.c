#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "lista.h"
#include "readFiles.h"

int main(){
    DIR* FD;
    struct dirent* s_archivo;
    FILE* archivo;
    char nombre_carpeta[PATH_MAX];
    char* nombre_archivo;
    char* text = ".txt";
    char* ret;

    lista* juegos;
    FILE* fp;
    dato* dp;

    init(&juegos);

    getcwd(nombre_carpeta, sizeof(nombre_carpeta));
    FD = opendir(nombre_carpeta);

    while ((s_archivo = readdir(FD))){

        if (!strcmp (s_archivo->d_name, "."))
            continue;
        if (!strcmp (s_archivo->d_name, ".."))
            continue;

        archivo = fopen(s_archivo->d_name, "r");

        // Reconocer archivos .txt
        nombre_archivo = s_archivo->d_name;
        ret = strstr(nombre_archivo, text);

        if (ret != NULL){
            fp = fopen(nombre_archivo, "r");
            dp = readFile(fp);
            append(juegos, dp);
            dp = at(((tipoJuego*)dp->contenido)->categorias, 0);
            crear_carpeta((char*)dp->contenido, nombre_archivo);
            fclose(fp);
        }

        fclose(archivo);
    }

    lista* datos;

    ordenarJuegos(juegos, &datos);
    consola(juegos, datos);

    return 0;
}
