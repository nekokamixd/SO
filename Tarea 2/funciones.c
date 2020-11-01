
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/mman.h>
#include "funciones.h"
#include <time.h>

//***** Complementos
#define num_players 4
#define msg_len 5
#define numCasillas 29

//***** Mensajes
#define turn "0000t"
#define statusOK "00000"    //turno finalizado correctamente
#define statusWIN "00001"   //el jugador gano la partida
#define switchFirst "0000p" // cambia jugador actual por el primero
#define switchLast "0000u"  // cambia jugador actual por el ultimo
#define reverse "0000r" // invierte orden de jugadores
#define skip "0000s"    // salta turno
#define nextWhite "0000n"   // los demas se mueven a la siguiente casilla blanca
#define statusEND "1111e"   //mensaje para avisarle a los demas jugadores que el juego termino
#define cursedBoard "0000C"   //mensaje para invertir tablero
#define othersBack "0000b"  // los demas retroceden una casilla
#define allBack "0000B" // todos retroceden 2 casillas

//***** Tipos de Casilla
#define inicio 0
#define fin 1
#define blanca 2
#define podNor 3 //poder normal
#define podSup 4 //poder superior

void sendall(int *pipes[num_players][2], char * msg, int len){
    for (int i = 0; i < num_players; i++){
        int *pipeFC = pipes[i][0];
        int *pipeCF = pipes[i][1];
        write(pipeFC[1],msg,len);
    }
}

int crear_tablero(int** tablero){
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

    return 0;
}

int crear_cursedTablero(int** tablero){
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

    return 0;
}

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

void* create_shared_memory(size_t size){

  int protection = PROT_READ | PROT_WRITE;

  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  return mmap(NULL, size, protection, visibility, -1, 0);
}

int imprimirTablero(){
    printf("%s\n", " ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___  ___ ");
    for(int i = 1; i < 30; i++){
        printf("%s", "|_ _|");
    }
    printf("%s", "\n");
    for(int i = 1; i < 30; i++){
        printf("%s", "|_ _|");
    }
    printf("%s", "\n\n");
    return 0;
}