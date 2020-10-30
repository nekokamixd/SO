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

void sendall(int *pipes[num_players][2], char * msg, int len){
    for (int i = 0; i < num_players; i++){
        int *pipeFC = pipes[i][0];
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
        mover = 1;
        memcpy(aux, &mover, sizeof(int));
        aux[4] = 'b';
    }

    //Los demas jugadores retroceden una cuadricula
    else if (poder == 1){
        memcpy(aux, othersBack, msg_len);
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

    srand(time(0));
    int poder = rand()%10;

    //Todos los jugadores retroceden 2 cuadriculas
    if ((poder == 0) || (poder == 1) || (poder == 2)){
        memcpy(aux, allBack, msg_len);
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
    
    int *pos = create_shared_memory(num_players*sizeof(int));
    int *pipe_jugador[num_players][2];

    //TABLERO MODO MATRIZ

    int **tablero = create_shared_memory((num_players + 1)*sizeof(int*));
    for (int i = 0; i < numCasillas; i++){
        tablero[i] = create_shared_memory(numCasillas*sizeof(int));
    }
    crear_tablero(tablero);

    int **cursedTablero = create_shared_memory((num_players + 1)*sizeof(int*));
    for (int i = 0; i < numCasillas; i++){
        cursedTablero[i] = create_shared_memory(numCasillas*sizeof(int));
    }
    crear_cursedTablero(cursedTablero);

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
    int *pipeFC, *pipeCF;

    for (int i = 0; i < num_players; i++){
        x = fork();
        if (x < 0){return 1;}// caso de error
        else if (x == 0){jugador = i+1;break;}// si soy hijo salgo
        else if (x > 0){//apongo a todos los jugadores en posicion 0
            pos[i] = 0;
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

        // turno   --> variable para iterar sobre los turnos
        // modo    --> recorrer turnos al derecho(0) o al inverso(1)
        int modo = 0;
        unsigned turno = 1000;

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

            if(strcmp(instruccion,statusOK) == 0){}// el turno termino correctamente, no hsy poderes
            else if(strcmp(statusWIN,instruccion) == 0){// el jugador ganó
                printf("hay ganador \n");
                sendall(pipe_jugador,statusEND,msg_len);
            }
            else if(strcmp(cursedBoard,instruccion) == 0){
                int **aux = tablero;
                tablero = cursedTablero;
                cursedTablero = aux;
                sendall(pipe_jugador,cursedBoard,msg_len);
            }
            else if(strcmp(reverse,instruccion) == 0){
                if (modo == 0){
                    modo = 1;
                }
                else{modo = 0;}
            }
            else if(strcmp(switchFirst,instruccion) == 0){
                int first = 0;
                for(int i = 0; i < num_players; i++){
                    if(pos[i] > first){
                        first = i;
                    }
                    else{}
                }
                int pos_act = pos[turno%4];
                int pos_fir = pos[first];
                int *aux_pipeFC = pipe_jugador[first][0];
                int *aux_pipeCF = pipe_jugador[first][1];

                memcpy(instruccion,&pos_fir,sizeof(int));
                instruccion[5] = 'm';
                write(pipeFC[1],instruccion,msg_len);
                    
                memcpy(instruccion,&pos_act,sizeof(int));
                instruccion[5] = 'm';
                write(aux_pipeFC[1],instruccion,msg_len);
            }
            else if(strcmp(switchLast,instruccion) == 0){
                int last = numCasillas;
                for(int i = 0; i < num_players; i++){
                    if(pos[i] < last){
                        last = i;
                    }
                    else{}
                }
                int pos_act = pos[turno%4];
                int pos_las = pos[last];
                int *aux_pipeFC = pipe_jugador[last][0];
                int *aux_pipeCF = pipe_jugador[last][1];

                memcpy(instruccion,&pos_las,sizeof(int));
                instruccion[5] = 'm';
                write(pipeFC[1],instruccion,msg_len);
                    
                memcpy(instruccion,&pos_act,sizeof(int));
                instruccion[5] = 'm';
                write(aux_pipeFC[1],instruccion,msg_len);
            }
            else if(strcmp(skip,instruccion) == 0){
                if (modo == 0){ turno++;}
                else{turno--;}
            }
            else if(strcmp(nextWhite,instruccion) == 0){
                for(int i = 0; i <num_players; i++){
                    if(turno%4 != i){
                        int *aux_pipeFC = pipe_jugador[i][0];
                        int *aux_pipeCF = pipe_jugador[i][1];
                        write(aux_pipeFC[1],nextWhite,msg_len);
                    }

                }
            }
            else if(strcmp(allBack,instruccion) == 0){
                int aux = 2;
                memcpy(instruccion,&aux,sizeof(int));
                instruccion[4] = 'b';
                sendall(pipe_jugador,instruccion,msg_len);
            }
            else if(strcmp(othersBack,instruccion) == 0){
                int aux = 1;
                memcpy(instruccion,&aux,sizeof(int));
                instruccion[4] = 'b';
                for(int i = 0; i <num_players; i++){
                    if(turno%4 != i){
                        int *aux_pipeFC = pipe_jugador[i][0];
                        int *aux_pipeCF = pipe_jugador[i][1];
                        write(aux_pipeFC[1],instruccion,msg_len);
                    }

                }
                
            }
            else{
                write(pipeFC[1],instruccion,msg_len);
            }
            //checkear ganador
            for (int i = 0; i < num_players; i++){
                if(tablero[i][numCasillas-1]){memcpy(instruccion,statusWIN,msg_len);}
            }
            
            if(modo == 0){turno++;}
            else{turno--;}
        }
        sendall(pipe_jugador, statusEND,msg_len);
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

                pos_actual = mover_pieza('f', dado, pos_actual, jugador, tablero);
                pos[jugador-1] = pos_actual;
                printf("Ahora estoy en: %d\n\n", pos_actual+1);

                if(tablero[num_players][pos_actual] == podNor){
                    printf("%s", "Has caido en una casilla con poder normal (?) :o\n\n");
                    if(jugador == 1){
                        printf("%s\n", "Quieres usar el poder? (si/no)");
                        scanf("%s", usarPoder);
                        if(strcmp(usarPoder, "si") == 0){
                            poder_Nor(instruccion);
                            write(pipeCF[1],instruccion,msg_len);
                        }
                    }
                }
                else if(tablero[num_players][pos_actual] == podSup){
                    if(jugador == 1){
                        printf("%s", "Has caido en una casilla con poder superior (? ?) :o\n\n");
                        printf("%s\n", "Quieres usar el poder? (si/no)");
                        scanf("%s", usarPoder);
                        if(strcmp(usarPoder, "si") == 0){
                            poder_Sup(instruccion);
                            write(pipeCF[1],instruccion,msg_len);
                        }
                    }
                }
                else{write(pipeCF[1],statusOK,msg_len);}

                if(pos_actual >= numCasillas){
                    write(pipeCF[1],statusWIN,msg_len);
                }
                else{
                    write(pipeCF[1],statusOK,msg_len);
                }
            }
            else if(instruccion[4] == 'f' || instruccion[4] == 'b' || instruccion[4] == 'm'){//mover pieza
                memcpy(&x,instruccion,4);
                //char tipoIns[1] = instruccion[4];
                pos_actual = mover_pieza(instruccion[4],x,pos_actual,jugador,tablero);
                pos[jugador-1] = pos_actual;
                }
            else if(strcmp(instruccion,cursedBoard) == 0){
                tablero[jugador-1][pos_actual] = 0;
                int **aux = tablero;
                tablero = cursedTablero;
                cursedTablero = aux;
                mover_pieza('m',numCasillas-pos_actual,0,jugador,tablero);
            }
            else if(strcmp(instruccion,statusEND) == 0){//terminar proceso
                printf("proceso terminado\n");
            }


        }
    }
    return 0;
}