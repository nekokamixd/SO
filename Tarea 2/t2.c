#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/mman.h>

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


/*
    el struct va a quedar
*/
typedef struct structCasilla{
    int jugador[num_players];
    int tipo;
}casilla;

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

    int x = 0, id[4];

    pipe(pipeFC);
    pipe(pipeCF);
    /******************************************/

    /*
    tablero  --> puntero al tablero(array de casillas)
    */
    casilla *tablero = create_shared_memory((num_players + 1)*30);


    /*
    loop para crear procesos, solo se crearan 4 procesos hijos,
    todos producidos por el padre, notar que si el pid = 0 se rompe el loop

    **Nota: el padre guarda el numero de proceso de los hijos
    */
    int id[num_players];

    for (int i = 0; i < num_players; i++){
        x = fork();
        if (x < 0){return 1;}// caso de error
        else if (x == 0){x = i;break;}// si soy hijo salgo
        else if (x > 0){//agrego id de proceso hijo al array
            id[i] = x;
            }
    }

    /*
    instruccion    --> buffer para recibir y escribir instrucciones a los hijos
    */
    char instruccion[5];



    if (x > 0){// PROCESO PADRE
        close(pipeFC[0]);// cierro terminal de lectura ya que en esta pipe escribo al hijo
        close(pipeCF[1]);// cierro terminal de escritura ya que en esta pipe leo lo que envio el hijo

        
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

            /*
            empaquetado de instrucción:
                La instrucción se anota en un array de tamaño 5, los primeros 4 bytes
                corresponden a un entero y el quinto byte corresponde al tipo de instruccion.
                't' -> turn
                'b' -> back
                'f' -> forward
            */
            memcpy(instruccion,id[turno%4],sizeof(int));
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
        /*escribir mensaje de salida<-----------------*/

    }



    else if(x == 0){ // PROCESO HIJO
        close(pipeFC[1]);// cierro terminal de escritura ya que en esta pipe leo
        close(pipeCF[0]);// cierro terminal de lectura ya que en esta pipe escribo

        /*
        
        */
        int pos_actual = 0;
        while(strcmp(statusEND,instruccion)!=0){
            while(read(pipeFC[0],instruccion,msg_len)<0){};
            memcpy(&x,instruccion,4);
            if(x == getpid() && instruccion[4]=='t'){   // es el turno del proceso

                /* codigo para que el proceso mueva su propia ficha*/
                
                if(/*codigo para determinar si el jugador gano despues de lanzar dado*/){
                    write(pipeCF[1],statusWIN,msg_len);
                    break;
                }
                else{
                    write(pipeCF[1],statusOK,msg_len);
                }
            }
            else if(instruccion[4] == 'f' || instruccion[4] == 'b'){/*codigo para manejar efectos de casillas*/}
            else if(strcmp(instruccion,statusEND)){break;}
        }
        exit(0);
    }
    return 0;
}