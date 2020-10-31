#include <stdio.h>
#include <stdlib.h>
#include <time.h> //srand y rand
#include <unistd.h> //pipes y cosas

int mover_pieza(char* direccion, int cantidad, int jugador, int actual, int*tablero){
    // i = Jugador o tablero, j = casilla
    int cols = 29;
    int nueva_posicion;
    int offset = (jugador-1) * cols + actual;
    if (strcmp(direccion, "b") == 0){
        nueva_posicion = actual - cantidad;
    }
    else if(strcmp(direccion, "f") == 0){
        nueva_posicion = actual + cantidad;
    }

    int nuevo_offset = (jugador-1) * cols + nueva_posicion;
    tablero[offset] = 0;
    tablero[nuevo_offset] = 1;
    return 0;
}

int poder_1(){
    srand(time(0));
    int poder = rand()%5;
    if (poder == 0){
        mover_pieza("b", 2); //El jugador que la activa
    }
    else if (poder == 1){
        mover_pieza("b", 1); //Los demas jugadores
    }
    else if (poder == 2){
        mover_pieza("f", 1); //El jugador que la activa
    }
    else if (poder == 3){
        skip_turn(); //Para el siguiente jugador
    }
    else if (poder == 4){
        //Cambio en el sentido de los turnos
    }
    return 0;
}

int poder_2(){
    srand(time(0));
    int poder = rand()%10;
    // Ver porcentaje
    if ((poder == 0) || (poder == 1) || (poder == 2)){
        mover_pieza("b", 2); //TODOS los jugadores
    }
    else if ((poder == 3) || (poder == 4)){
        //Los demas jugadores avanzan hasta la proxima casilla/cuadricula blanca
    }
    else if ((poder == 5) || (poder == 6)){
        //Cambio de posicion con el jugador que va ultimo
    }
    else if ((poder == 7) || (poder == 8)){
        //Cambio de posicion con el jugador que va primero
    }
    else if (poder == 9){
        //Cambio en el sentido del tablero
    }
    return 0;
}

/*
int prueba_rand(){
    srand(time(0));
    int prueba = rand()%3;
    if (prueba == 0){
        printf("%d\n", prueba);
    }
    else if (prueba == 1){
        printf("%d\n", prueba);
    }
    else if (prueba == 2){
        printf("%d\n", prueba);
    }
    return 0;
}
*/

int main(){
    printf("%s", "Bienvenido al jueguito de mesa :D\n\n");

    int orden[4];
    printf("%s", "Ingrese orden de los jugadores:\n");
    for(int i=0; i<4; i++){
        printf("Turno %d: ", i+1);
        scanf("%d", &orden[i]);
    }

    srand(time(0));
    int dado = rand()%6+1;
    printf("%d\n", dado);

    return 0;
}
