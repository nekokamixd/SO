#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>

#define num_players 4
#define msg_len 5
#define statusOK "00000"    //turno finalizado correctamente
#define statusWIN "00001"   //el jugador gano la partida
#define statusEND "1111e"   //mensaje para avisarle a los demas jugadores que el juego termino

int main(){
    char prueba[msg_len];
    memcpy(prueba, statusWIN, msg_len);
    printf("%s\n", prueba);

    return 0;
}
