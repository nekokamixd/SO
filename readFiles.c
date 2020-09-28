
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "lista.h"
#include "readFiles.h"

/*
readFile()
Lee un unico archivo de juego y extrae todos los
datos del mismo para almacenarlos en un struct.
——————————————–
Inputs:
FILE* fp : puntero al archivo a leer.
——————————————–
Output:
dato* dp : puntero al struct del juego.
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
            cat->tipo = 's';
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
    dp->tipo = 'J';

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

/*
ordenarJuegos()
toma todos los Juegos de una lista, y los ordena
por categoria y por cantidad de categorias para
las cuales el juego califique en una matriz.
Notar que la lista input queda intacta, es decir,
la matriz generada es totalmente nueva.
——————————————–
Inputs:
lista* games : puntero a la lista de juegos.
lista** chart : la direccion de memoria del puntero
en main para almacenar en el la direccion de la matriz.
——————————————–
Output:
void : no hay retorno propiamente tal.
*/
void ordenarJuegos(lista *games, lista **chart){
    lista *lp,*categoria; // --> tabla ordenada
    init(&lp);
    tipoJuego *juegoActual;
    dato *auxDato;
    chartSec *seccion, *auxSeccion;
    int lenGames = length(games), lenListaCat, lenCat, ExisteCat = 0;
    char *catJuegoActual,*catIteradaActual;

    //loop sobre todos los juegos
    for(int i = 0; i < lenGames; i++){
        //inicializar variable con tamaño de tabla(numero de categorias)
        lenListaCat = length(lp);

        ExisteCat = 0;

        //obtener datos juego actual
        juegoActual = at(games,i)->contenido;
        catJuegoActual = at(juegoActual->categorias,0)->contenido;

        //iterar sobre lista de categorias
        for(int j = 0; j < lenListaCat; j++){
            //obtener juegos en una categoria
            categoria = at(lp,j)->contenido;
            //inicializar cantidad de juegos en una categoria y nombre de la misma
            lenCat = length(categoria);
            catIteradaActual = at(categoria,0)->contenido;


            if(strcmp(catIteradaActual,catJuegoActual) == 0){// <--la categoria es la del juego

                auxSeccion = malloc(sizeof(chartSec));
                strcpy(auxSeccion->nombre_juego , juegoActual->nombre_juego);
                auxSeccion->prioridad = juegoActual->prioridad;
                auxDato = malloc(sizeof(dato));
                auxDato->tipo = 'S';
                auxDato->contenido = auxSeccion;

                //iterar sobre una categoria
                for(int k = 1; k < lenCat; k++){
                    //obtener los datos de un juego ya ordenado dentro de la categoria
                    seccion = at(categoria,k)->contenido;

                    //si la prioridad es mayor o igual que la del juego actual en la categoria se agrega
                    if (juegoActual->prioridad >= seccion->prioridad){
                        insert(categoria,k,auxDato);
                        ExisteCat = 1;
                    }
                };
                // si despues de ejecutar el loop, el juego no se ha agregado
                // a la categoria, éste como el último de la misma
                if(ExisteCat == 0){
                    append(categoria,auxDato);
                    ExisteCat = 1;
                }
            }
        }
        // este es el caso en que la categoria del juego
        // todavia no existe
        if(ExisteCat == 0){
            //Agregar categoria a tabla
            categoria = NULL;
            init(&categoria);
            auxDato = malloc(sizeof(dato));
            auxDato->tipo = 'l';
            auxDato->contenido = categoria;
            append(lp,auxDato);
            //agregar nombre y juego a la categoria

            //nombre categoria
            auxDato = malloc(sizeof(dato));
            auxDato->contenido = malloc(sizeof(char)*20);
            strcpy((char*)auxDato->contenido,catJuegoActual);
            auxDato->tipo = 's';
            append(categoria,auxDato);

            //juego
            auxDato = malloc(sizeof(dato));
            auxSeccion = malloc(sizeof(chartSec));
            auxSeccion->prioridad = juegoActual->prioridad;
            strcpy(auxSeccion->nombre_juego,juegoActual->nombre_juego);
            auxDato->tipo = 'S';
            auxDato->contenido = auxSeccion;
            append(categoria,auxDato);

        }
    }

    (*chart) = lp;
    return;
}

/*
consola()
Funcion para hacer los efectos de navegacion
por la biblioteca de juegos.
——————————————–
Inputs:
lista* games : puntero a la lista de juegos.
lista* chart : puntero a la matriz con juegos
ordenados.
——————————————–
Output:
int : el programa se ha ejecutado correctamente.
*/

int consola(lista* juegos, lista* tabla){
    dato* dp;
    dato* dp_juego;
    char opcion_categoria[32];
    char opcion_juego[32];
    int existe_categoria = 0;
    int existe_juego = 0;
    lista* categoria;

    tipoJuego* info_juego;
    dato* dato_categoria;


    while(strcmp(opcion_categoria, "Salir") != 0){

        // Imprimir categorias
        printf("%s", "Categorias:\n\n");
        for(int i=0; i<length(tabla); i++){
            categoria = at(tabla, i)->contenido;
            dp = at(categoria, 0);
            printf("%s\n", (char*)dp->contenido);
        }

        printf("%s", "\nIngrese categoria, ingrese 'Salir' para terminar el programa: ");
        scanf("%s", opcion_categoria);

        // Verificar si se escribe bien el nombre de la categoria
        for(int i=0; i<length(tabla); i++){
            categoria = at(tabla, i)->contenido;
            dp = at(categoria, 0);
            if(strcmp(opcion_categoria, (char*)dp->contenido) == 0){
                existe_categoria = 1;
                break;
            }
        }

        if(existe_categoria == 1){

            // Imprimir juegos de la categoria
            printf("%s", "\n");
            for(int i=1; i<length(categoria); i++){
                dp = at(categoria, i);
                printf("%s", ((chartSec*)dp->contenido)->nombre_juego);
            }

            while((existe_juego == 0) && (existe_categoria == 1)){
                printf("%s", "\nIngrese juego, ingrese 'Atras' para volver: ");
                scanf(" %[A-Za-z ]", opcion_juego);

                if(strcmp(opcion_juego, "Atras") != 0){
                    // Recorrer lista juegos
                    for(int j=0; j<length(juegos); j++){
                        dp_juego = at(juegos, j);
                        info_juego = dp_juego->contenido;

                        if(strncmp(opcion_juego, info_juego->nombre_juego, strlen(opcion_juego)) == 0){
                            existe_juego = 1;
                            char string_categorias[1024] = "";
                            printf("%s", "\nNombre del juego: ");
                            printf("%s", info_juego->nombre_juego);
                            printf("%s", "Categorias: ");
                            for(int k = 0; length((lista*)info_juego->categorias) > k; k++){
                                dato_categoria = at((lista*)info_juego->categorias, k);
                                strcat(string_categorias, (char*)dato_categoria->contenido);
                                strcat(string_categorias, " ");
                            }
                            printf("%s", string_categorias);
                            printf("%s", "\nDesarrollador: ");
                            printf("%s", info_juego->creador);
                            printf("%s", "Descripcion: ");
                            printf("%s\n", info_juego->descripcion);
                            existe_categoria = 0;
                            existe_juego = 0;
                            break;
                        }
                    }
                }else{
                    existe_categoria = 0;
                    existe_juego = 0;
                }
            }
        }



    }
    return 0;
}
