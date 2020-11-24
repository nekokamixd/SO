//***** Librerías
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/mman.h>
#include "funciones.h"
#include <time.h>

/*
Nombre: sendall
Parametros:
    int* pipes[num_players][2]: Matriz de pipes.
    char* msg: Mensaje que se enviará a los hijos.
    int len: Largo del mensaje.
Retorno: funcion void
Descripcion: Recibe un puntero a las pipes junto con un mensaje y su largo, luego envia el mismo mensaje a todos los hijos.
*/
void sendall(int* pipes[num_players][2], char* msg, int len){
    for (int i = 0; i < num_players; i++){
        int *pipeFC = pipes[i][0];
        write(pipeFC[1],msg,len);
    }
}

/*
Nombre: crear_tablero
Parametros:
    int** tablero: Matriz que contiene la fila correspondiente al tablero.
Retorno: funcion void
Descripcion: Modifica la fila correspondiente al tablero con los tipos de casillas.
*/
void crear_tablero(int** tablero){
    int casillasPoderNor[9] = {2, 4, 6, 12, 14, 21, 23, 25, 27};
    int casillasPoderSup[4] = {16, 22, 24, 26};
    int recorrerNor = 0;
    int recorrerSup = 0;

    for(int i = 0; i < numCasillas; i++){
        if(i == casillasPoderNor[recorrerNor]){
            tablero[num_players][i] = podNor;
            recorrerNor++;
        }
        else if(i == casillasPoderSup[recorrerSup]){
            tablero[num_players][i] = podSup;
            recorrerSup++;
        }
        else{
            tablero[num_players][i] = blanca;
        }
    }

    tablero[num_players][0] = inicio;
    tablero[num_players][numCasillas-1] = fin;

    return;
}

/*
Nombre: crear_cursedTablero
Parametros:
    int** tablero: Matriz para el tablero invertido que contiene la fila correspondiente al tablero invertido.
Retorno: funcion void
Descripcion: Modifica la fila correspondiente al tablero invertido con los tipos de casillas que corresponden a ese tablero.
*/
void crear_cursedTablero(int** tablero){
    int casillasPoderNor[4] = {2, 4, 6, 12};
    int casillasPoderSup[9] = {1, 3, 5, 7, 14, 16, 22, 24, 26};
    int recorrerNor = 0;
    int recorrerSup = 0;

    for(int i = 0; i < numCasillas; i++){
        if(i == casillasPoderNor[recorrerNor]){
            tablero[num_players][i] = podNor;
            recorrerNor++;
        }
        else if(i == casillasPoderSup[recorrerSup]){
            tablero[num_players][i] = podSup;
            recorrerSup++;
        }
        else{
            tablero[num_players][i] = blanca;
        }
    }

    tablero[num_players][0] = inicio;
    tablero[num_players][numCasillas-1] = fin;

    return;
}

/*
Nombre: mover_pieza
Parametros:
    char direccion: Caracter que define en que dirección se moverá la pieza.
    int cantidad: Cantidad de casillas que se moverá la pieza.
    int actual: Posición actual del jugador.
    int jugador: Jugador que se mueve.
    int** tablero: Matriz que contiene las posiciones de cada jugador y el tablero.
Retorno: (int) Retorna la nueva posición en la que se encuentra el jugador.
Descripcion: Toma la posición actual del jugador, la dirección y cantidad en la que se moverá para luego
realizar los cambios en la matriz que contiene las posiciones de cada jugador y el tablero.
*/
int mover_pieza(char direccion, int cantidad, int actual, int jugador, int** tablero){
    int nueva_posicion;

    if (direccion == 'b'){
        nueva_posicion = actual - cantidad;
        if (nueva_posicion < 0){
            nueva_posicion = 0;
        }
    }
    else if(direccion == 'f'){
        nueva_posicion = actual + cantidad;
        if(nueva_posicion > numCasillas-1){
            nueva_posicion = numCasillas-1;
        }
    }
    else if(direccion == 'm'){
        nueva_posicion = cantidad;
    }

    tablero[jugador-1][actual] = 0;
    tablero[jugador-1][nueva_posicion] = 1;
    return nueva_posicion;
}

/*
Nombre: poder_Nor
Parametros:
    char* aux: Dirección del buffer donde se guardará la instrucción resultante.
Retorno: funcion void
Descripcion: Calcula qué poder se realizará dentro de los poderes normales y luego copia la instrucción correspondiente al poder en aux.
*/
void poder_Nor(char* aux){
    int mover;

    srand(time(0));
    int poder = rand()%5;

    //El jugador retrocede una cuadricula
    if (poder == 0){
        printf("%s\n", "El jugador retrocede una cuadricula");
        mover = 1;
        memcpy(aux, &mover, sizeof(int));
        aux[4] = 'b';
    }

    //Los demas jugadores retroceden una cuadricula
    else if (poder == 1){
        printf("%s\n", "Los demas jugadores retroceden una cuadricula");
        memcpy(aux, othersBack, msg_len);
    }

    //El jugador avanza una cuadricula
    else if (poder == 2){
        printf("%s\n", "El jugador avanza una cuadricula");
        mover = 1;
        memcpy(aux, &mover, sizeof(int));
        aux[4] = 'f';
    }

    //El siguiente jugador no puede jugar su turno.
    else if (poder == 3){
        printf("%s\n", "El siguiente jugador no puede jugar su turno.");
        memcpy(aux, skip, msg_len);
    }

    //Cambio en el sentido de los turnos
    else if (poder == 4){
        printf("%s\n", "Cambio en el sentido de los turnos");
        memcpy(aux, reverse, msg_len);
    }
    return;
}

/*
Nombre: poder_Sup
Parametros:
    char* aux: Dirección del buffer donde se guardará la instrucción resultante.
Retorno: funcion void
Descripcion: Calcula qué poder se realizará dentro de los poderes superiores y luego copia la instrucción correspondiente al poder en aux.
*/
void poder_Sup(char* aux){

    srand(time(0));
    int poder = rand()%10;

    //Todos los jugadores retroceden 2 cuadriculas
    if ((poder == 0) || (poder == 1) || (poder == 2)){
        printf("%s\n", "Todos los jugadores retroceden 2 cuadriculas");
        memcpy(aux, allBack, msg_len);
    }

    //Los demas jugadores avanzan hasta su proxima cuadricula blanca
    else if ((poder == 3) || (poder == 4)){
        printf("%s\n", "Los demas jugadores avanzan hasta su proxima cuadricula blanca");
        memcpy(aux, nextWhite, msg_len);
    }

    //El jugador cambia de posicion con el jugador que va en ultimo lugar
    else if ((poder == 5) || (poder == 6)){
        printf("%s\n", "El jugador cambia de posicion con el jugador que va en ultimo lugar");
        memcpy(aux, switchLast, msg_len);
    }

    //El jugador cambia de posicion con el jugador que va en primer lugar
    else if ((poder == 7) || (poder == 8)){
        printf("%s\n", "El jugador cambia de posicion con el jugador que va en primer lugar");
        memcpy(aux, switchFirst, msg_len);
    }

    //Cambio en el sentido del tablero
    else if (poder == 9){
        printf("%s\n", "Cambio en el sentido del tablero");
        memcpy(aux, cursedBoard, msg_len);
    }
    return;
}

/*
Nombre: create_shared_memory
Parametros:
    size_t size: Tamaño de la memoria que se solicita.
Retorno: (void*) Corresponde a la dirección base de la memoria que se solicita.
Descripcion: Recibe el tamaño de la memoria que se requiere y retorna la dirección a esa memoria.
*/
void* create_shared_memory(size_t size){

  int protection = PROT_READ | PROT_WRITE;

  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  return mmap(NULL, size, protection, visibility, -1, 0);
}

/*
Nombre: imprimirTablero
Parametros:
    int j1: Posición en la que se encuentra jugador 1.
    int j2: Posición en la que se encuentra jugador 2.
    int j3: Posición en la que se encuentra jugador 3.
    int j4: Posición en la que se encuentra jugador 4.
    int** tablero: Matriz que contiene la posición de cada jugador junto con el tablero.
Retorno: funcion void
Descripcion: Se encarga de imprimir los números de cada casilla, el tablero con sus tipos de casilla y la posición de los jugadores.
*/
void imprimirTablero(int j1, int j2, int j3, int j4, int** tablero){
    // Tablero
    printf("%s\n", "\n  1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29 ");
    printf("%s\n", " ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___ ");

    for(int i = 0; i < numCasillas; i++){
        if(tablero[num_players][i] ==  inicio){
            printf("%s", "|ini|");
        }
        else if(tablero[num_players][i] ==  fin){
            printf("%s", "|fin|");
        }
        else if(tablero[num_players][i] ==  blanca){
            printf("%s", "|   |");
        }
        else if(tablero[num_players][i] ==  podNor){
            printf("%s", "| ? |");
        }
        else if(tablero[num_players][i] ==  podSup){
            printf("%s", "|? ?|");
        }
    }
    printf("%s", "\n");

    for(int i = 0; i < numCasillas; i++){
        if(tablero[num_players][i] ==  inicio){
            printf("%s", "|cio|");
        }
        else if(tablero[num_players][i] ==  fin){
            printf("%s", "|_ _|");
        }
        else if(tablero[num_players][i] ==  blanca){
            printf("%s", "|_ _|");
        }
        else if(tablero[num_players][i] ==  podNor){
            printf("%s", "|_ _|");
        }
        else if(tablero[num_players][i] ==  podSup){
            printf("%s", "|_ _|");
        }
    }
    printf("%s", "\n");

    // Jugadores
    printf("%s\n", " ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___ ");
    for(int i = 0; i < numCasillas; i++){
        if((i == j1) && (i == j2)){
            printf("%s", "|1 2|");
        }
        else if(i == j1){
            printf("%s", "|1  |");
        }
        else if(i == j2){
            printf("%s", "|  2|");
        }
        else{
            printf("%s", "|   |");
        }
    }
    printf("%s", "\n");
    for(int i = 0; i < 29; i++){
        if((i == j3) && (i == j4)){
            printf("%s", "|3 4|");
        }
        else if(i == j3){
            printf("%s", "|3 _|");
        }
        else if(i == j4){
            printf("%s", "|_ 4|");
        }
        else{
            printf("%s", "|_ _|");
        }
    }
    printf("%s", "\n\n");
    return;
}
