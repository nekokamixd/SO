#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "lista.h"


typedef struct Juego{
    char nombre_juego[256];
    int prioridad;
    lista *categorias;
}tipoJuego;

/* NOTA *************
* funcion: dato *readFile(File *fp)
*********************************
* basicamente hay que pasarle el puntero a archivo a la funcion
* y esta devolverá las categorias de un juego organizadas en un struct
*********************************
* return: puse el struct en un tipo "dato" para que se pudiese agregar
* directamente a la lista de juegos
*/
dato* readFile(FILE *fp){
    //////////////// variables ////////////////
    char buff[256];
    int flag = 1;
    dato *dp;
    ///////////////////////////////////////////
    //dp es el tipo dato que contendrá al juego en si en el campo "contenido"
    //flag servirá para saber si la linea de categorias se termino de leer
    //buff contendra el nombre del juego y despues las categorias


    dp = (dato*)malloc(sizeof(dato));
    dp->contenido = (tipoJuego*)malloc(sizeof(tipoJuego));
    dp->tipo = 'j';//esto es cualquier cosa, ignorar


    fgets(buff,255,fp); // obtener nombre del juego
    strcpy(   ((tipoJuego*)dp->contenido)->nombre_juego   ,   buff); //colocar nombre en struct
    init(   &((tipoJuego*)dp->contenido)->categorias   );   //inicializar categorias como lista
    ((tipoJuego*)dp->contenido)->prioridad = 0;             //iniciar prioridad

    dato *cat;

    while (flag == 1){
        // bloque para colocar categorias en una lista
        if (fscanf(fp,"%[a-zA-Z0-9]",buff)){
            cat = (dato*)malloc(sizeof(dato));
            cat->contenido = (char*)malloc(sizeof(char)*256);
            cat->tipo = 'c';
            strcpy(cat->contenido,buff);
            append(((tipoJuego*)dp->contenido)->categorias,cat);
            ((tipoJuego*)dp->contenido)->prioridad += 1;
        }
        else if(fscanf(fp,"%[,]",buff)==1){continue;}
        else if(fscanf(fp,"%[ ]",buff)==1){continue;}
        else{flag = 0;} //esto quiere decir que llegó el salto de linea y no quedan categorías
    }
    printf("Nombre Juego: %s",((tipoJuego*)dp->contenido)->nombre_juego);
    printf("Categorias: ");
    for(int i = 0; i < length(  ((tipoJuego*)dp->contenido)->categorias  );  i++){
        cat = at(((tipoJuego*)dp->contenido)->categorias,i);
        printf("%s ",(char*)cat->contenido);
    };
    printf("\n");
    return dp;
};

int crear_carpeta(char* nombre){
    int make = mkdir(nombre, S_IRWXU | S_IRWXG | S_IRWXO);
    return make;
}

int main(){
    FILE *fp;
    dato* juego;
    char nombre;

    fp = fopen("Ligo Leyen.txt","r");
    juego = readFile(fp);

    nombre = juego->nombre_juego;
    printf("%c", nombre);

    char* categoria = "Categoria";
    crear_carpeta(categoria);
    return 0;
}
