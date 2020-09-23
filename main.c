#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include "lista.h"
#include "readFiles.c"

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
    /*
    fp = fopen("Ligo Leyen.txt", "r");
    dp = readFile(fp);
    append(juegos, dp);
    dp = at(juegos, 0);

    printf("%s", ((tipoJuego*)dp->contenido)->nombre_juego);
    */

    getcwd(nombre_carpeta, sizeof(nombre_carpeta));
    FD = opendir(nombre_carpeta);

    while ((s_archivo = readdir(FD))){

        if (!strcmp (s_archivo->d_name, "."))
            continue;
        if (!strcmp (s_archivo->d_name, ".."))
            continue;

        archivo = fopen(s_archivo->d_name, "r");

        /*
        if (archivo == NULL){
            fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
            return 1;
        }
        */

        // Reconocer archivos .txt
        nombre_archivo = s_archivo->d_name;
        ret = strstr(nombre_archivo, text);

        if (ret != NULL){
            fp = fopen(nombre_archivo, "r");
            dp = readFile(fp);
            dp = at(((tipoJuego*)dp->contenido)->categorias, 0);
            crear_carpeta((char*)dp->contenido, nombre_archivo);
            fclose(fp);
        }

        fclose(archivo);
    }

    char opcion[32];
    int existe_categoria = 0;
    while(strcmp(opcion, "Salir") != 0){
        printf("%s\n", "Categorias:");
        for(int i=0; i<length(tabla); i++){
            juegos = at(tabla, 0)->contenido;
            dp = at(juegos, 0);
            printf("%s\n", (char*)dp->contenido);
        }
        printf("%s", "Ingrese categoria, ingrese 'Salir' para terminar el programa: ");
        scanf("%s", opcion);

        // Verificar si se escribe bien el nombre de la categoria
        for(int i=0; i<length(tabla); i++){
            juegos = at(tabla, 0)->contenido;
            dp = at(juegos, 0);
            if(strcmp(opcion, (char*)dp->contenido) == 0){
                existe_categoria = 1;
                break
            }
        }

        if(existe_categoria == 1){
            for(int i=1; i<length(juegos); i++){
                dp = at(juegos, i);
                printf("%s\n", ((chartSec*)dp->contenido)->nombre_juego); //ASFJDHAKSHDLAKSFHALSFHLKAKSHADL
            }

            printf("%s", "Ingrese juego, ingrese 'Atras' para volver: ");
            scanf("%s", opcion);
            if(strcmp(opcion, "Atras") != 0){
                for(int i=1; i<length(juegos); i++){
                    dp = at(juegos, i);
                    printf("%s\n", ((chartSec*)dp->contenido)->nombre_juego); //ASFJDHAKSHDLAKSFHALSFHLKAKSHADL
                }

            }
            else {break};
        }


    }

    return 0;
}
