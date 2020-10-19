#ifndef READFILES
#define READFILES
#include "lista.h"
#include <stdio.h>

typedef struct Juego{
    char nombre_juego[256],creador[256],descripcion[500];
    int prioridad;
    lista *categorias;
}tipoJuego;

typedef struct cuadroTabla{
    char nombre_juego[256];
    int prioridad;
}chartSec;

dato* readFile(FILE *fp);

int crear_carpeta(char* categoria, char* juego);

void ordenarJuegos(lista *games, lista **chart);

int consola(lista* juegos, lista* tabla);

#endif //READFILES
