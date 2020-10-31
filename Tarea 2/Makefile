#enlazado
a.out: funciones.o main.o
	gcc -Wall funciones.o main.o

#compilacion de main.o
main.o: main.c
	gcc -Wall -c main.c funciones.h

#compilacion de funciones.o
funciones.o: funciones.c
	gcc -Wall -c funciones.c
