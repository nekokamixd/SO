//***** Librerias
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include "funciones.h"

int main(){
    /*
    player          --> Permite elegir el turno partirá el jugador principal.
    x               --> Será la variable que contendrá el pid de los procesos.
    mostrar_turno   --> Permite mostrar los turnos que transcurren en el juego.
    pipe_jugador    --> Array que contiene las pipes de todos los jugadores.
    */

    int player;
    int x = 0;
    int mostrar_turno = 1;
    int *pipe_jugador[num_players][2];


    /*
    MEMORIA COMPARTIDA
    pos             --> Array con la posición de cada jugador.
    tablero         --> Puntero al tablero.
    cursedTablero   --> Puntero al tablero invertido.
    */

    int *pos = create_shared_memory(num_players*sizeof(int));

    //TABLERO MODO MATRIZ
    int **tablero = create_shared_memory((num_players + 1)*sizeof(int*));
    for (int i = 0; i < numCasillas; i++){
        tablero[i] = create_shared_memory(numCasillas*sizeof(int));
    }
    crear_tablero(tablero);

    //TABLERO INVERTIDO MODO MATRIZ
    int **cursedTablero = create_shared_memory((num_players + 1)*sizeof(int*));
    for (int i = 0; i < numCasillas; i++){
        cursedTablero[i] = create_shared_memory(numCasillas*sizeof(int));
    }
    crear_cursedTablero(cursedTablero);

    printf("Elige el turno en que quieres empezar (del 1 al 4): ");
    scanf("%d", &player);
    printf("\n");

    // Se crean las pipes.
    for(int i = 0; i < num_players; i++){
        pipe_jugador[i][0] = alloca(sizeof(int)*2);
        pipe_jugador[i][1] = alloca(sizeof(int)*2);
        pipe(pipe_jugador[i][0]);
        pipe(pipe_jugador[i][1]);
    }

    /*
    VARIABLES
    jugador     --> Variable para contener el numero de jugador de un subproceso.
    pipeFC      --> Pipe que envía o recibe mensajes desde el proceso padre hacia los procesos hijos.
    pipeCF      --> Pipe que envía o recibe mensajes desde el proceso hijo hacia el padre.
    */

    int jugador;
    int *pipeFC, *pipeCF;

    // Se crean los procesos hijos.
    for (int i = 0; i < num_players; i++){
        x = fork();
        if (x < 0){return 1;}// caso de error
        else if (x == 0){jugador = i+1;break;}// si soy hijo salgo
        else if (x > 0){//apongo a todos los jugadores en posicion 0
            pos[i] = 0;
            }
    }

    // instruccion    --> Buffer para recibir y escribir instrucciones a los hijos.
    char instruccion[msg_len];

    if (x > 0){// PROCESO PADRE

        /* cierro terminales que no usaré*/
        for(int i = 0; i < num_players; i++){
            close(pipe_jugador[i][0][0]); // terminal de lectura
            close(pipe_jugador[i][1][1]);// terminal de escritura
        }

        /*
        mostrar_ganador     --> Permite mostrar el ganador al final del juego.
        turno               --> Variable para iterar sobre los turnos a traves del %jugador.
        modo                --> Recorrer turnos al derecho(0) o al inverso(1).
        */
        int mostrar_ganador = 0;
        int modo = 0;
        unsigned turno = 1000;

        while(strcmp(statusWIN,instruccion)!=0){

            // Cambio pipes actuales a las que correspondan.
            pipeFC = pipe_jugador[(turno%4)][0];
            pipeCF = pipe_jugador[(turno%4)][1];

            // Se muestra el turno en el que se está jugando.
            printf("Turno: %d\n", mostrar_turno);
            mostrar_turno++;

            /*
            Game Master
            La funcion del gamemaster consiste en indicarle a todos los jugadores a quien le toca jugar
            (por el momento). Después de emitir dicho mensaje espera la respuesta del hijo y dependiendo
            del resultado efectúa diferentes acciones.
            */
            memcpy(instruccion,turn,msg_len);

            /*Escribo mensaje y espero respuesta del hijo*/
            write(pipeFC[1],instruccion,msg_len); // Indica turno del jugador
            while(read(pipeCF[0],instruccion,msg_len) < 0){}; // Espera mensaje de hijo

            /*
            Acciones de respuesta:
            Recomendamos colapsar cada condición para una mejor visualización.
            */

            if(strcmp(instruccion,statusOK) == 0){} // El turno termino correctamente, no hay poderes.
            else if(strcmp(statusWIN,instruccion) == 0){// El jugador ganó.
                mostrar_ganador = turno%num_players+1;
            }
            else if(strcmp(cursedBoard,instruccion) == 0){ // Se invierte el tablero.
                int **aux = tablero;
                tablero = cursedTablero;
                cursedTablero = aux;
                sendall(pipe_jugador,cursedBoard,msg_len);
            }
            else if(strcmp(reverse,instruccion) == 0){ // Se invierten los turnos.
                if (modo == 0){
                    modo = 1;
                }
                else{modo = 0;}
            }
            else if(strcmp(switchFirst,instruccion) == 0){ // Se cambia el jugador que va primero por el actual.
                int pos_fir = 0;
                int first = 0;
                for(int i = 0; i < num_players; i++){
                    if(pos[i] > pos_fir){
                        first = i;
                        pos_fir = pos[i];
                    }
                    else{}
                }
                int pos_act = pos[turno%4];
                int *aux_pipeFC = pipe_jugador[first][0];

                memcpy(instruccion,&pos_fir,sizeof(int));
                instruccion[4] = 'm';
                write(pipeFC[1],instruccion,msg_len);

                memcpy(instruccion,&pos_act,sizeof(int));
                instruccion[4] = 'm';
                write(aux_pipeFC[1],instruccion,msg_len);
            }
            else if(strcmp(switchLast,instruccion) == 0){ // Se cambia el jugador que va último por el actual.
                int pos_las = numCasillas;
                int last = 0;
                for(int i = 0; i < num_players; i++){
                    if(pos[i] < pos_las){
                        last = i;
                        pos_las = pos[i];
                    }
                    else{}
                }
                int pos_act = pos[turno%4];
                int *aux_pipeFC = pipe_jugador[last][0];

                memcpy(instruccion,&pos_las,sizeof(int));
                instruccion[4] = 'm';
                write(pipeFC[1],instruccion,msg_len);

                memcpy(instruccion,&pos_act,sizeof(int));
                instruccion[4] = 'm';
                write(aux_pipeFC[1],instruccion,msg_len);
            }
            else if(strcmp(skip,instruccion) == 0){ // El siguiente jugador pierde su turno.
                if (modo == 0){ turno++;}
                else{turno--;}
            }
            else if(strcmp(nextWhite,instruccion) == 0){ // Los demás jugadores avanzan hasta la siguiente casilla blanca.
                for(int i = 0; i < num_players; i++){
                    int pl = turno%num_players;
                    if(pl != i){
                        for (int j = pos[i]+1; j < numCasillas-1; j++){
                            if(tablero[num_players][j] == blanca){
                                int *aux_pipeFC = pipe_jugador[i][0];
                                memcpy(instruccion,&j,sizeof(int));
                                instruccion[4] = 'm';
                                write(aux_pipeFC[1],instruccion,msg_len);
                                break;
                            }

                        }

                    }
                }
            }
            else if(strcmp(allBack,instruccion) == 0){ // Todos los jugadores retroceden 2 casillas.
                int aux = 2;
                memcpy(instruccion,&aux,sizeof(int));
                instruccion[4] = 'b';
                sendall(pipe_jugador,instruccion,msg_len);
            }
            else if(strcmp(othersBack,instruccion) == 0){ // Los demás jugadores retroceden 1 casilla.
                int aux = 1;
                memcpy(instruccion,&aux,sizeof(int));
                instruccion[4] = 'b';
                for(int i = 0; i <num_players; i++){
                    if(turno%4 != i){
                        int *aux_pipeFC = pipe_jugador[i][0];
                        write(aux_pipeFC[1],instruccion,msg_len);
                    }
                }
            }
            else{ // Jugador actual retrocede o avanza una casilla.
                write(pipeFC[1],instruccion,msg_len);
            }

            // Se checkea el final del turno y se imprime el tablero.
            char ok[2];
            printf("%s", "Escriba ok para continuar: ");
            scanf("%s", ok);
            imprimirTablero(pos[0], pos[1], pos[2], pos[3], tablero);

            // Se checkea el ganador después de un poder.
            for (int i = 0; i < num_players; i++){
                if(tablero[i][numCasillas-1] == 1){
                    mostrar_ganador = i+1;
                    memcpy(instruccion,statusWIN,msg_len);
                }
            }

            // Aumenta o disminuye el turno dependiendo de la dirección de los turnos.
            if(modo == 0){turno++;}
            else{turno--;}
        }

        // Se imprime que jugador ganó y envía un mensaje para terminar los procesos hijos.
        printf("El ganador es: Jugador %d\n", mostrar_ganador);
        sendall(pipe_jugador, statusEND,msg_len);
    }

    else if(x == 0){ // PROCESO HIJO

        // Se asignan las pipes correspondientes.
        pipeFC = pipe_jugador[jugador-1][0];
        pipeCF = pipe_jugador[jugador-1][1];
        close(pipeFC[1]); // Cierro terminal de escritura ya que en esta pipe leo.
        close(pipeCF[0]); // Cierro terminal de lectura ya que en esta pipe escribo.

        int pos_actual = 0;

        while(strcmp(statusEND,instruccion)!=0){
            // Leer mensaje del padre y descifrarlo (separarlo).
            while(read(pipeFC[0],instruccion,msg_len)<0){};

            if(strcmp(instruccion,turn) == 0){ // Es el turno del proceso.

                // Se lanzan los dados
                srand(getpid()*time(0));
                int dado = rand()%6+1;

                printf("Soy el jugador: %d\n", jugador);
                printf("Posicion actual: %d\n", pos_actual+1);
                printf("Dado: %d\n", dado);

                // Se mueve la pieza según el dado que se lanzó y se actualiza el array pos.
                pos_actual = mover_pieza('f', dado, pos_actual, jugador, tablero);
                pos[jugador-1] = pos_actual;
                printf("Ahora estoy en: %d\n\n", pos_actual+1);

                char usarPoder[2];
                if(tablero[num_players][pos_actual] == podNor){ // Casilla actual tiene un poder normal (?).
                    printf("%s", "Has caido en una casilla con poder normal (?) :o\n\n");

                    // Pregunta al jugador principal si quiere utilizar el poder.
                    if(player == jugador){
                        printf("%s\n", "Quieres usar el poder? (si/no)");
                        scanf("%s", usarPoder);
                        printf("%s", "\n");
                        if(strcmp(usarPoder, "si") == 0){
                            poder_Nor(instruccion);
                            write(pipeCF[1],instruccion,msg_len);
                        }else{
                            write(pipeCF[1],statusOK,msg_len);
                        }
                    }
                    else{
                        poder_Nor(instruccion);
                        write(pipeCF[1],instruccion,msg_len);
                    }
                }
                else if(tablero[num_players][pos_actual] == fin){ // Checkea si el jugador gana luego de lanzar los dados.
                    write(pipeCF[1], statusWIN, msg_len);
                }
                else if(tablero[num_players][pos_actual] == podSup){ // Casilla actual tiene un poder superior (??).
                    printf("%s", "Has caido en una casilla con poder superior (? ?) :o\n\n");

                    // Pregunta al jugador principal si quiere utilizar el poder.
                    if(player == jugador){
                        printf("%s\n", "Quieres usar el poder? (si/no)");
                        scanf("%s", usarPoder);
                        printf("%s", "\n");
                        if(strcmp(usarPoder, "si") == 0){
                            poder_Sup(instruccion);
                            write(pipeCF[1],instruccion,msg_len);
                        }else{
                            write(pipeCF[1],statusOK,msg_len);
                        }
                    }
                    else{
                        poder_Sup(instruccion);
                        write(pipeCF[1],instruccion,msg_len);
                    }
                }
                else if(tablero[num_players][pos_actual] == blanca){ // Turno sin poderes.
                    write(pipeCF[1],statusOK,msg_len);
                }
            }
            else if(instruccion[4] == 'f' || instruccion[4] == 'b' || instruccion[4] == 'm'){ // Mover pieza.
                memcpy(&x,instruccion,4);
                pos_actual = mover_pieza(instruccion[4],x,pos_actual,jugador,tablero);
                pos[jugador-1] = pos_actual;
                }
            else if(strcmp(instruccion,cursedBoard) == 0){ // Cambia el puntero del tablero y se coloca la pieza en el lugar correspondiente.
                tablero[jugador-1][pos_actual] = 0;
                int **aux = tablero;
                tablero = cursedTablero;
                cursedTablero = aux;
                pos_actual = mover_pieza('m',numCasillas - pos_actual -1,0,jugador,tablero);
                pos[jugador-1] = pos_actual;
            }
            else if(strcmp(instruccion,statusEND) == 0){} // Termina el proceso.
        }
    }
    return 0;
}
