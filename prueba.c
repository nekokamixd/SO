#include <string.h>
#include <stdio.h>

int main(){
    int existe_juego = 0;
    int existe_categoria = 1;
    char opcion_juego[100];

    while((existe_juego == 0) && (existe_categoria == 1)){
        printf("%s", "Ingrese juego, ingrese 'Atras' para volver: ");
        scanf(" %[A-Za-z ]", opcion_juego);

        if(strcmp(opcion_juego, "Atras") != 0){
            // Recorrer lista juegos
            if(strncmp(opcion_juego, "Fire Emblem", strlen(opcion_juego)) == 0){
                printf("%s", "xd\n");
                existe_juego = 1;
                break;
                    // Imprimo informacion
            }
            else{
                printf("%s", "yikes\n");
            }
        }else{
            existe_categoria = 0;
            existe_juego = 0;
        }
    }
    return 0;
}
