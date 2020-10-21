#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/mman.h>
#define num_players 4


/*
    Escribí este struct pensando en las casillas del tablero, pero en verdad es de prueba 
    y no se si lo vayamos a usar tal cual como está
*/
typedef struct structCasilla{
    int posicion;
    char tipo[3];
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
    int x = 0;

    pipe(pipeFC);
    pipe(pipeCF);
    /******************************************/


    /* 
    msg     --> mensaje de prueba para probar las pipes
    */
    char *msg = "chumi",buff[10];

    /*
    Aqui se empieza a poner entretenido xd
    la funcion para crear una memoria compartida era mucho mas simple de lo que 
    pensaba, funciona como pedir memoria con malloc, simplemente le das la cantidad
    de memoria que quieres alojar y te retorna un puntero de tipo void* a la direccion
    de dicha memoria

    En síntesis: el puntero "tablero" va a funcionar como un array de casillas
    asi que puedes acceder a las casillas por indice

    tablero  --> puntero al tablero(array de casillas)
    */
    casilla *tablero = create_shared_memory(sizeof(struct structCasilla)*15);

    /*
    valor de prueba, la idea era cambiar el valor en el proceso
    padre para que el hijo lo leyera
    */    
    tablero[0].posicion = 5;
    

    /*
    loop para crear procesos, solo se crearan 4 procesos hijos,
    todos producidos por el padre, notar que si el pid = 0 se rompe el loop
    */

    for (int i = 0; i < num_players; i++){
        x = fork();
        if (x < 0){return 1;}// caso de error
        else if (x > 0){}
        else if (x == 0){break;}
    }



    if (x > 0){// PROCESO PADRE
        close(pipeFC[0]);// cierro terminal de lectura ya que en esta pipe escribo al hijo
        close(pipeCF[1]);// cierro terminal de escritura ya que en esta pipe leo lo que envio el hijo

        tablero[0].posicion += 1;  // modifico el valor de la primera casilla para verificar el
                                    // cambio en los hijos
        
        /*
        este loop escribe en la pipe varias veces el mensaje
        cada vez que se lee con un "read()" un mensaje, este ya no es accesible
        asi que es necesario stackear los mensajes para que todos los subprocesos los lean
        en este caso escribimos 4 veces el mensaje original para cada uno de los 4 subprocesos
        */
        for(int i = 0; i < num_players; i++){
            write(pipeFC[1],msg,strlen(msg)+1);     
        }

    }
    else if(x == 0){ // PROCESO HIJO
        close(pipeFC[1]);// cierro terminal de escritura ya que en esta pipe leo
        close(pipeCF[0]);// cierro terminal de lectura ya que en esta pipe escribo

        /*
        Aquí espero a que el padre envíe un mensaje, despues de recibir el mensaje
        lo printeo y también la variable que cabié en el tablero

        ***NOTA: hay que tener muuuchisimo cuidado con el tamaño delinput que estoy
        leyendo desde la pipe ya que si me paso en la cantidad de caracteres podría
        estarle quitando caracteres de input al próximo proceso.
        
        POR EJEMPLO un proceso lee 7 letras en vez de 6, entonces lo que qedaría 
        guardado sería "chumi\0c" en el buffer, es decir que el proximo proceso tendría 
        una "c" menos en su input
        */
        while(read(pipeFC[0],buff,6)<0){};
        
        printf("mensaje recibido: %s\n", buff);
        printf("pos: %d\n",tablero[0].posicion);     
    }
    return 0;
}