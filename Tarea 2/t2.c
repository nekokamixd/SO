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
#define numCasillas 19

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
    int *tablero = create_shared_memory((num_players + 1)*numCasillas*sizeof(int));
    int *pipe_jugador[num_players][2];
    
    /*
    TABLERO MODO MATRIZ

    int **tablero = create_shared_memory((num_players + 1)*sizeof(int*));
    for (int i = 0; i < numCasillas; i++){
        tablero[i] = create_shared_memory(numCasillas*sizeof(int));
    }
    */

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
        int id_hijo;

        while(strcmp(statusWIN,instruccion)!=0){

            id_hijo = id_jugador[turno%4];
            pipeFC = pipe_jugador[(turno%4)][0];
            pipeCF = pipe_jugador[(turno%4)][1];

            printf("Turno: %d\n", turno);
            printf("id_hijo: %d\n", id_hijo);

            memcpy(instruccion,&id_hijo,sizeof(int));
            instruccion[4] = 't';

            /*
            Game Master
            La funcion del gamemaster consiste en indicarle a todos los jugadores a quien le toca jugar
            (por el momento). Despues de emitir dicho mensaje espera la respuesta del hijo y dependiendo
            del resultado efetúa diferentes acciones.
            */
            
            write(pipeFC[1],instruccion,msg_len);// indica turno del jugador

            while(read(pipeCF[0],instruccion,msg_len) < 0){};//espera mensaje de hijo

            if(strcmp(instruccion,statusOK) == 0){}// el turno termino correctamente
            else if(strcmp(instruccion,statusWIN) == 0){// el jugador ganó
                printf("hay ganador \n");
                for(int i = 0; i < num_players; i++){ // indica a los otros jugadores el termino
                    write(pipe_jugador[i][0][1],statusEND,msg_len);}
                    wait(NULL);
                    
            }
            else{/*codigo para retransmitir poderes de casillas*/}
            turno++;
        }
        printf("%s\n", "Sos un crack ganastes");

    }



    else if(x == 0){ // PROCESO HIJO
        int casilla_final = calcular_offset(jugador, numCasillas);
        pipeFC = pipe_jugador[jugador-1][0];
        pipeCF = pipe_jugador[jugador-1][1];
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
                }
                else{
                    tablero[pos_antigua] = 0;
                    tablero[pos_actual] = 1;
                    write(pipeCF[1],statusOK,msg_len);
                }
            }
            else if(instruccion[4] == 'f' || instruccion[4] == 'b'){/*codigo para manejar efectos de casillas*/}
            else if(strcmp(instruccion,statusEND) == 0){printf("proceso terminado\n");}
            else{}
        }
        return 0;
    }
    return 0;
}