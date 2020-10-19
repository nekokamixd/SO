#enlazado
a.out: lista.o readFiles.o main.o
	gcc -Wall lista.o readFiles.o main.o

#compilacion de main.o
main.o: main.c
	gcc -Wall -c main.c lista.h readFiles.h

#compilacion de readFiles.o
readFiles.o: readFiles.c
	gcc -Wall -c lista.h readFiles.c

#compilacion de lista.o
lista.o: lista.c
	gcc -Wall -c lista.c
