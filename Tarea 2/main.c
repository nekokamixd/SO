#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    int tablero[29];
    for(int i=0; i<29; i++){
        tablero[i] = i;
        printf("%d", tablero[i]);
    }

    srand(time(0));
    int dado = rand()%7;
    printf("%d\n", dado);

    return 0;
}
