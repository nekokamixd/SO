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
#define nextWhite "0000n"   //
#define statusEND "1111e"   //mensaje para avisarle a los demas jugadores que el juego termino
#define cursedBoard "0000B"   //mensaje para invertir tablero


//***** Tipos de Casilla
#define inicio 0
#define fin 1
#define blanca 2
#define podNor 3 //poder normal
#define podSup 4 //poder superior

void sendall(int *pipes[num_players][2], char * msg){
    for (int i = 0; i < num_players; i++){
        int *pipeFC = pipes[i][0];
        write(pipeFC[1],msg,msg_len);
    }
}

int crear_tablero(int** tablero){
    int casillasPoderNor[9] = {2, 4, 6, 12, 14, 21, 23, 25, 27};
    int casillasPoderSup[4] = {16, 22, 24, 26};
    int recorrerNor = 0;
    int recorrerSup = 0;

    for(int i = 0; i < numCasillas; i++){
        if(i == casillasPoderNor[recorrerNor]){
            tablero[4][i] = podNor;
            recorrerNor++;
        }
        else if(i == casillasPoderSup[recorrerSup]){
            tablero[4][i] = podSup;
            recorrerSup++;
        }
        else{
            tablero[4][i] = blanca;
        }
    }

    tablero[4][0] = inicio;
    tablero[4][28] = fin;

    return 0;
}

int crear_cursedTablero(int** tablero){
    int casillasPoderNor[9] = {2, 4, 6, 12};
    int casillasPoderSup[4] = {1, 3, 5, 7, 14, 16, 22, 24, 26};
    int recorrerNor = 0;
    int recorrerSup = 0;

    for(int i = 0; i < numCasillas; i++){
        if(i == casillasPoderNor[recorrerNor]){
            tablero[4][i] = podNor;
            recorrerNor++;
        }
        else if(i == casillasPoderSup[recorrerSup]){
            tablero[4][i] = podSup;
            recorrerSup++;
        }
        else{
            tablero[4][i] = blanca;
        }
    }

    tablero[4][0] = inicio;
    tablero[4][28] = fin;

    return 0;
}

int mover_pieza(char* direccion, int cantidad, int actual, int jugador, int** tablero){
    int nueva_posicion;

    if (strcmp(direccion, "b") == 0){
        nueva_posicion = actual - cantidad;
        if (nueva_posicion < 0){
            nueva_posicion = 0;
        }
    }
    else if(strcmp(direccion, "f") == 0){
        nueva_posicion = actual + cantidad;
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
        mover = 2;
        memcpy(aux, &mover, sizeof(int));
        aux[4] = 'b';
    }

    //Los demas jugadores retroceden una cuadricula
    else if (poder == 1){
        mover = 1;
        memcpy(aux, &mover, sizeof(int));
        aux[4] = 'B';
    }

    //El jugador avanza una cuadricula
    else if (poder == 2){
        mover = 1;
        memcpy(aux, &mover, sizeof(int));
        aux[4] = 'f';
    }

    //El siguiente jugador no puede jugar su turno.
    else if (poder == 3){
        memcpy(aux, skip, msg_len);
    }

    //Cambio en el sentido de los turnos
    else if (poder == 4){
        memcpy(aux, reverse, msg_len);
    }
    return;
}

void poder_Sup(char* aux){
    int mover;

    srand(time(0));
    int poder = rand()%10;

    //Todos los jugadores retroceden 2 cuadriculas
    if ((poder == 0) || (poder == 1) || (poder == 2)){
        mover = 2;
        memcpy(aux, &mover, sizeof(int));
        aux[4] = 'B';
    }

    //Los demas jugadores avanzan hasta su proxima cuadricula blanca
    else if ((poder == 3) || (poder == 4)){
        memcpy(aux, nextWhite, msg_len);
    }

    //El jugador cambia de posicion con el jugador que va en ultimo lugar
    else if ((poder == 5) || (poder == 6)){
        memcpy(aux, switchLast, msg_len);
    }

    //El jugador cambia de posicion con el jugador que va en primer lugar
    else if ((poder == 7) || (poder == 8)){
        memcpy(aux, switchFirst, msg_len);
    }

    //Cambio en el sentido del tablero
    else if (poder == 9){
        memcpy(aux, cursedBoard, msg_len);
    }
    return;
}

void* create_shared_memory(size_t size) {

  int protection = PROT_READ | PROT_WRITE;

  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  return mmap(NULL, size, protection, visibility, -1, 0);
}

int main(){
    /*****************************************
    Inicializar pipes y el valor x (pid)
    pipeNC --> envia mensajes desde el padre a al hijo N   (N e {1,2,3,4})
    pipeCN --> envia mensajes desde el hijo al padre
    x      --> será la variable que contendrá el pid de los procesos
    */

    int x = 0;


    /*
    MEMORIA COMPARTIDA

    tablero         --> puntero al tablero(array de casillas)
    pipe_jugador    --> array que contiene las pipes de todos los jugadores
    */
    //int *tablero = create_shared_memory((num_players + 1)*numCasillas*sizeof(int));
    int *pipe_jugador[num_players][2];

    //TABLERO MODO MATRIZ

    int **tablero = create_shared_memory((num_players + 1)*sizeof(int*));
    for (int i = 0; i < numCasillas; i++){
        tablero[i] = create_shared_memory(numCasillas*sizeof(int));
    }
    crear_tablero(tablero);

    int **cursedTablero = create_shared_memory((num_players + 1)*sizeof(int*));
    for (int i = 0; i < numCasillas; i++){
        tablero[i] = create_shared_memory(numCasillas*sizeof(int));
    }

    for(int i = 0; i < num_players; i++){
        pipe_jugador[i][0] = alloca(sizeof(int)*2);
        pipe_jugador[i][1] = alloca(sizeof(int)*2);
        pipe(pipe_jugador[i][0]);
        pipe(pipe_jugador[i][1]);
    }


    /*
    VARIABLES

    jugador         --> variable para contener el numero de jugador de un subproceso
    id_jugador      --> array que contiene el pid de cada hijo
    */
    int jugador;
    int id_jugador[num_players];
    int *pipeFC, *pipeCF;

    for (int i = 0; i < num_players; i++){
        x = fork();
        if (x < 0){return 1;}// caso de error
        else if (x == 0){jugador = i+1;break;}// si soy hijo salgo
        else if (x > 0){//agrego id de proceso hijo al array
            id_jugador[i] = x;
            }
    }

    /*
    instruccion    --> buffer para recibir y escribir instrucciones a los hijos
    */
    char instruccion[msg_len];



    if (x > 0){// PROCESO PADRE
        /* cierro terminales que no usaré*/

        for(int i = 0; i < num_players; i++){
            close(pipe_jugador[i][0][0]); // terminal de lectura
            close(pipe_jugador[i][1][1]);// terminal de escritura
        }

        //turno   --> variable para iterar sobre los turnos
        //id_hijo --> variable que contendra el id del jugador en turno
        //pipes   --> variables de comunicacion con hijos

        int turno = 0;

        while(strcmp(statusWIN,instruccion)!=0){
            //cambio pipes actuales a las que correspondan
            pipeFC = pipe_jugador[(turno%4)][0];
            pipeCF = pipe_jugador[(turno%4)][1];

            printf("Turno: %d\n", turno);
            printf("jugador : %d\n", (turno%4));

            /*
            Game Master
            La funcion del gamemaster consiste en indicarle a todos los jugadores a quien le toca jugar
            (por el momento). Despues de emitir dicho mensaje espera la respuesta del hijo y dependiendo
            del resultado efetúa diferentes acciones.
            */
            memcpy(instruccion,turn,msg_len);

            /*Escribo mensaje y espero respuestta del hijo*/
            write(pipeFC[1],instruccion,msg_len);// indica turno del jugador
            while(read(pipeCF[0],instruccion,msg_len) < 0){};//espera mensaje de hijo

            /*acciones de respuesta*/

            if(strcmp(instruccion,statusOK) == 0){}// el turno termino correctamente
            else if(strcmp(statusWIN,instruccion) == 0){// el jugador ganó
                printf("hay ganador \n");
                sendall(pipe_jugador,statusEND);
            }
            else if(strcmp(cursedBoard,instruccion) == 0){
                /*intrucciones para invertir tablero*/
            }
            else if(strcmp(reverse,instruccion) == 0){}
            else if(strcmp(switchFirst,instruccion) == 0){}
            else if(strcmp(switchLast,instruccion) == 0){}
            else if(strcmp(skip,instruccion) == 0){
                turno++;
            }
            else if(strcmp(nextWhite,instruccion) == 0){
                sendall(pipe_jugador,nextWhite);
            }
            turno++;
        }
        printf("%s\n", "Sos un crack ganastes");

    }

    else if(x == 0){ // PROCESO HIJO
        pipeFC = pipe_jugador[jugador-1][0];
        pipeCF = pipe_jugador[jugador-1][1];
        close(pipeFC[1]);// cierro terminal de escritura ya que en esta pipe leo
        close(pipeCF[0]);// cierro terminal de lectura ya que en esta pipe escribo

        int pos_actual = 0;
        while(strcmp(statusEND,instruccion)!=0){
            // leer mensaje del padre y descifrarlo(separarlo)
            while(read(pipeFC[0],instruccion,msg_len)<0){};

            if(strcmp(instruccion,turn) == 0){   // es el turno del proceso
                char usarPoder[2];
                srand(getpid()*time(0));
                int dado = rand()%6+1;
                printf("Soy el jugador: %d\n", jugador);
                printf("Posicion actual: %d\n", pos_actual+1);
                printf("Dado: %d\n", dado);

                pos_actual = mover_pieza("f", dado, pos_actual, jugador, tablero);
                printf("Ahora estoy en: %d\n\n", pos_actual+1);

                if(tablero[4][pos_actual] == podNor){
                    printf("%s", "Has caido en una casilla con poder normal (?) :o\n\n");
                    if(jugador == 1){
                        printf("%s\n", "Quieres usar el poder? (si/no)");
                        scanf("%s", usarPoder);
                        if(strcmp(usarPoder, "si") == 0){
                            poder_Nor(instruccion);
                        }
                    }
                }
                else if(tablero[4][pos_actual] == podSup){
                    if(jugador == 1){
                        printf("%s", "Has caido en una casilla con poder superior (? ?) :o\n\n");
                        printf("%s\n", "Quieres usar el poder? (si/no)");
                        scanf("%s", usarPoder);
                        if(strcmp(usarPoder, "si") == 0){
                            poder_Sup(instruccion);
                        }
                    }
                }

                if(pos_actual >= numCasillas){
                    write(pipeCF[1],statusWIN,msg_len);
                }
                else{
                    write(pipeCF[1],statusOK,msg_len);
                }
            }
            else if(instruccion[4] == 'f' || instruccion[4] == 'b' || instruccion[4] == 'm'){//mover pieza
                memcpy(&x,instruccion,4);
                }
            else if(strcmp(instruccion,cursedBoard) == 0){
                /*instrucciones para mover piezas dando vuelta el tablero*/
            }
            else if(strcmp(instruccion,nextWhite) == 0){
                /*codgo para mover pieza a la siguiente casilla vacia*/
            }
            else if(strcmp(instruccion,statusEND) == 0){//terminar proceso
                printf("proceso terminado\n");
            }


        }
    }
    return 0;
}
