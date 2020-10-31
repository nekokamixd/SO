#ifndef FUNCIONES
#define FUNCIONES

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/mman.h>
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

void sendall(int *pipes[num_players][2], char * msg, int len);

int crear_tablero(int** tablero);

int crear_cursedTablero(int** tablero);

int mover_pieza(char direccion, int cantidad, int actual, int jugador, int** tablero);

void poder_Nor(char* aux);

void poder_Sup(char* aux);

void* create_shared_memory(size_t size);

int imprimirTablero();

#endif //FUNCIONES
