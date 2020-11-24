-José Montecinos 201804541-1
-Sofía Palet     201873570-1

Modo de ejecución:

Se debe ejecutar 'make' en consola para compilar el programa.
Se debe ejecutar './a.out' para correr el programa.

Modo de uso:

Una vez iniciado el programa, sólo es necesario elegir el turno
en el que se desea empezar, escribir 'ok' y decidir si usar un
poder o no según se indique. En el caso de que se ingrese un input
erróneo al decidir usar un poder, se considerará como que el poder
no es usado.
El tablero se imprimirá después de la ejecución de cada turno.
Si hay más de un jugador que cumple alguna condición para los poderes, se elegirá arbitrariamente.

Nota: Al elegir en que turno empezar, también se elige el número de jugador,
asumimos que los otros jugadores juegan en su correspondiente turno.
Existen algunas definiciones de variables en funciones.h

Explicación pipes y memoria compartida:

En memoria compartida se encuentran 2 tableros, el tablero normal
y el tablero en sentidos opuestos que es donde los jugadores
moverán sus piezas. Cuando se produce el efecto de cambiar el
sentido del tablero simplemente se intercambian los punteros
de ambos tableros. También existe un array que contiene las
posiciones de cada jugador y será actualizado por cada jugador
después de que haya hecho un movimiento.

Para el uso de pipes se estableció el siguiente método de comu-
nicación entre los procesos padre e hijo. Existen 2 pipes por cada
hijo. También existe un mensaje por cada tipo de efecto posible(? + ??),
más 3 mensaje de turno y  dos de término de juego. Al principio del
turno el proceso padre le indica al hijo correspondiente que debe
ejecutar su turno. Después de ejecutar el turno, el hijo le envía
un mensaje al padre indicándole si:
1) el turno terminó correctamente, es decir sin ejecutar un poder
2) se produce un poder, en cuyo caso el mensaje enviado es el tipo de poder a ejecutar
3) el jugador ganó después de lanzar dados

En el primer caso, simplemente se sigue con la ejecución del siguiente turno.
En  el segundo caso, es el padre el encargado de determinar quiénes
son los jugadores que se deben mover producto del poder y es a ellos
a quienes les envía un mensaje indicando que deben moverse, incluso
si es el caso de que el poder solo involucre al jugador actual.
En el tercer caso, y en cualquier otro en el que el juego termine,
el proceso padre envía un mensaje a los hijos indicando que el juego
terminó, procediendo los hijos a cerrar su propio proceso.
