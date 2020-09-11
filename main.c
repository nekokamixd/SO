#include <stdio.h>
#include <sys/stat.h> //mkdir
//#include <sys/file.h>
//#include <stdlib.h>
//#include <sys/dir.h>

int main(){
    // system("ls"); Para tirar comandos por consola

    int make = mkdir("prueba", S_IRWXU | S_IRWXG | S_IRWXO);
    printf("%d", make);
    return 0;
}
