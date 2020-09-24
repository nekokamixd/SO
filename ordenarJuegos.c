#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "readFiles.c"

typedef struct cuadroTabla{
    char nombre_juego[256];
    int prioridad;
}chartSec;

void ordenarJuegos(lista *games, lista **chart){
    lista *lp,*categoria; // --> tabla ordenada
    init(&lp);
    tipoJuego *juegoActual;
    dato *auxDato, *juegoIndexado;
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
            auxDato = malloc(sizeof(dato));
            auxDato->contenido = malloc(sizeof(char)*20);
            strcpy((char*)auxDato->contenido,catJuegoActual);
            auxDato->tipo = 'c';
            append(categoria,auxDato);

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
