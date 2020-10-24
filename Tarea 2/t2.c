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
#define statusOK "00000"    //turno finalizado correctamente
#define statusWIN "00001"   //el jugador gano la partida
#define statusEND "1111e"   //mensaje para avisarle a los demas jugadores que el juego termino


//***** Tipos de Casilla
#define inicio 0
#define fin 1
#define blanca 2
#define podNor 3 //poder normal
#define podSup 4 //poder superior
#define numCasillas 2

int calcular_offset(int jugador, int actual){
    int offset = (jugador-1) * (numCasillas-1) + actual;
    return offset;
}

int mover_pieza(char* direccion, int cantidad, int jugador, int actual, int*tablero){
    // i = Jugador o tablero, j = casilla
    int nueva_posicion;
    //int offset = (jugador-1) * numCasillas + actual;

    if (strcmp(direccion, "b") == 0){
        nueva_posicion = actual - cantidad;
        //Verificar inicio
    }
    else if(strcmp(direccion, "f") == 0){
        nueva_posicion = actual + cantidad;
    }

    //int nuevo_offset = (jugador-1) * numCasillas + nueva_posicion;
    /*
    tablero[offset] = 0;
    tablero[nuevo_offset] = 1;*/
    return nueva_posicion;
}

/*
int poder_Nor(){
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

int poder_Sup(){
    srand(time(0));
    int poder = rand()%10;
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
*/

void* create_shared_memory(size_t size) {

  int protection = PROT_READ | PROT_WRITE;

  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  return mmap(NULL, size, protection, visibility, -1, 0);
}

int main(){
    /*****************************************
    Inicializar pipes y el valor x

    pipeFC --> envia mensajes desde el padre a los hijos (Father to Children)
    pipeCF --> envia mensajes desde el hijo al padre (Children to Father)
    x      --> será la variable que contendrá el pid de los procesos
    */
    int pipeFC[2];
    int pipeCF[2];

    int x = 0;

    pipe(pipeFC);
    pipe(pipeCF);
    /******************************************/

    /*
    tablero  --> puntero al tablero(array de casillas)
    */
    int *tablero = create_shared_memory((num_players + 1)*numCasillas);


    /*
    loop para crear procesos, solo se crearan 4 procesos hijos,
    todos producidos por el padre, notar que si el pid = 0 se rompe el loop

    **Nota: el padre guarda el numero de proceso de los hijos
    */
    int id_jugador[num_players];
    int jugador;

    for (int i = 0; i < num_players; i++){
        x = fork();
        if (x < 0){return 1;}// caso de error
        else if (x == 0){jugador = i+1;break;}// si soy hijo salgo
        else if (x > 0){//agrego id de proceso hijo al array
            id_jugador[i] = x;
            printf("x: %d\n", x);
            }
    }

    /*
    instruccion    --> buffer para recibir y escribir instrucciones a los hijos
    */
    char instruccion[msg_len];



    if (x > 0){// PROCESO PADRE
        close(pipeFC[0]);// cierro terminal de lectura ya que en esta pipe escribo al hijo
        close(pipeCF[1]);// cierro terminal de escritura ya que en esta pipe leo lo que envio el hijo
        int id_hijo;


        //turno   --> variable para iterar sobre los turnos
        int turno = 0;

        /*
        Loop del Padre

        El padre funcionará como GameMaster, en el sentido de que no se desarrollaran interacciones
        entre los jugadores sino que cada cambio global debe pasar por el gamemaster y es él quien se
        lo comunica al resto de lso jugadores para que estos efectúen el cambio.

        El padre responde a 3 tipos de mensajes
        statusOK    --> corresponde al hecho de que un turno terminó sin novedad, es decir lanzar
                        dados y avanzar, sin activar casillas. En este caso se sigue iterando en la
                        lista de jugadores.
        statusWIN   --> corresponde al caso de que un jugador al suceder un movimiento llegue al
                        final del tablero. En este caso se le manda un mensaje a los jugadores para
                        que terminen su proceso.
        otro        --> corresponde a los mensajes de activacion de las casillas
        */
        while(strcmp(statusWIN,instruccion)!=0){
            printf("Turno: %d\n", turno);
            id_hijo = id_jugador[turno%4];
            printf("id_hijo: %d\n", id_hijo);

            /*
            empaquetado de instrucción:
                La instrucción se anota en un array de tamaño 5, los primeros 4 bytes
                corresponden a un entero y el quinto byte corresponde al tipo de instruccion.
                't' -> turn
                'b' -> back
                'f' -> forward
            */
            memcpy(instruccion,&id_hijo,sizeof(int));
            instruccion[4] = 't';

            /*
            Game Master
            La funcion del gamemaster consiste en indicarle a todos los jugadores a quien le toca jugar
            (por el momento). Despues de emitir dicho mensaje espera la respuesta del hijo y dependiendo
            del resultado efetúa diferentes acciones.
            */
            for(int i = 0; i < num_players; i++){// indica turno del jugador
                write(pipeFC[1],instruccion,msg_len);}

            while(read(pipeCF[0],instruccion,msg_len) < 0){};//espera mensaje de hijo

            if(strcmp(instruccion,statusOK) == 0){}// el turno termino correctamente
            else if(strcmp(instruccion,statusWIN) == 0){// el jugador ganó
                for(int i = 0; i < num_players-1; i++){ // indica a los otros jugadores el termino
                    write(pipeFC[1],statusEND,msg_len);}
                    break;
            }
            else{/*codigo para retransmitir poderes de casillas*/}
            turno++;
        }
        printf("%s\n", "Sos un crack ganastes");

    }



    else if(x == 0){ // PROCESO HIJO
        int casilla_final = calcular_offset(jugador, numCasillas);
        //int casilla_inicio = calcular_offset(jugador, 0);
        close(pipeFC[1]);// cierro terminal de escritura ya que en esta pipe leo
        close(pipeCF[0]);// cierro terminal de lectura ya que en esta pipe escribo

        int pos_actual = 0;
        while(strcmp(statusEND,instruccion)!=0){
            while(read(pipeFC[0],instruccion,msg_len)<0){};
            memcpy(&x,instruccion,4);
            if(x == getpid() && instruccion[4]=='t'){   // es el turno del proceso

                srand(time(0));
                int dado = rand()%6+1;
                printf("Soy el jugador: %d\n", jugador);
                printf("Posicion actual: %d\n", pos_actual);
                printf("Dado: %d\n", dado);

                int pos_antigua = pos_actual;
                pos_actual = mover_pieza("f", dado, jugador, pos_actual, tablero);
                printf("Ahora estoy en: %d\n\n", pos_actual);

                if(pos_actual >= casilla_final){
                    write(pipeCF[1],statusWIN,msg_len);
                    break;
                }
                else{
                    tablero[pos_antigua] = 0;
                    tablero[pos_actual] = 1;
                    write(pipeCF[1],statusOK,msg_len);
                }
            }
            else if(instruccion[4] == 'f' || instruccion[4] == 'b'){/*codigo para manejar efectos de casillas*/}
            else if(strcmp(instruccion,statusEND) == 0){break;}
            else{}
        }
        exit(0);
    }
    return 0;
}
