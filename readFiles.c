
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
    char nombre_juego[256],creador[256],descripcion[1024];
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
    tipoJuego *gp;
    dato *dp;
    ///////////////////////////////////////////
    //dp es el tipo-dato que contendrá al juego en si, en el campo "contenido"
    //flag servirá para saber si la linea de categorias se terminó de leer
    //buff contendra el nombre del juego y después las categorías
    
    gp = (tipoJuego*)malloc(sizeof(tipoJuego));

    fgets(gp->nombre_juego,255,fp); // obtener nombre del juego
    init(   &(gp->categorias)   );   //inicializar categorias como lista
    gp->prioridad = 0;             //iniciar prioridad
    
    dato *cat;
    
    while (flag == 1){
        // bloque para colocar categorias en una lista
        if (fscanf(fp,"%[a-zA-Z0-9]",buff)){
            cat = (dato*)malloc(sizeof(dato));
            cat->contenido = (char*)malloc(sizeof(char)*256);
            cat->tipo = 'c';
            strcpy(cat->contenido,buff);
            append(gp->categorias,cat);
            gp->prioridad += 1;
        }
        else if(fscanf(fp,"%[,]",buff)==1){continue;}
        else if(fscanf(fp,"%[ ]",buff)==1){continue;}
        else{flag = 0; fscanf(fp,"%[\n]",buff);} //esto quiere decir que llegó el salto de linea y no quedan categorías
    }
    //obtener desarrollador/creador
    fgets(gp->creador,255,fp);
    
    //obtener descripcion
    while(fgets(buff,255,fp)){
        strcat(gp->descripcion,buff);
    };

    dp = (dato*)malloc(sizeof(dato));
    dp->contenido = gp;

    return dp;
};

int crear_carpeta(char* categoria, char* juego){
    char path[PATH_MAX];
    char new_path[PATH_MAX];

    int make = mkdir(categoria, S_IRWXU | S_IRWXG | S_IRWXO);

    getcwd(path, sizeof(path));
    strcpy(new_path, path);
    strcat(path, "/");
    strcat(path, juego);

    strcat(new_path, "/");
    strcat(new_path, categoria);
    strcat(new_path, "/");
    strcat(new_path, juego);

    rename(path, new_path);

    return make;
}