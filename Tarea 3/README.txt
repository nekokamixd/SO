-José Montecinos 201804541-1
-Sofía Palet     201873570-1

NOTAS DE IMPLEMENTACION PREGUNTA 1:

- Se empleó la librería ScriptEngine para resolver las expresiones
matemáticas, la cual no nos compilaba en Windows por una razón que
no logramos determinar. Sin embargo al probar en Linux todo funcio-
naba bien asi que recomendamos ejecutar en Linux.

Consideración: en el archivo "funciones.txt" solo debe haber funciones
que tengan a "x" como parámetro.

NOTAS DE IMPLEMENTACION PREGUNTA 2:

- El algoritmo de ordenamiento del array de números corresponde
a QuickSort que sigue la idea de dividir y conquistar separando
el array en 2 partes con numeros mayores y menores en relación
a un número pivote. En su uso se emplea recursión ya que después
de dividir el arreglo en 2, el algoritmo se vuelve a ejecutar
sobre las mitades hasta que el arreglo se encuentre ordenando.

- Teniendo lo anterior en consideración, las threads fueron imple-
mentadas de manera de que, después de ordenar el arreglo en relación
a un pivote, se crea una thread por cada lado del arreglo que se
tenga que ordenar para que ejecute el algoritmo.

- Al ejecutar el programa primero se puede visualizar el funciona-
miento de ambos algoritmos con un arreglo aleatorio de tamaño 10, 
luego se puede ver nuevas ejecuciones de ambos algoritmos con tamaños
de arreglo cada vez mayores. Por cada ejecución se tiene el:
    - El tamaño del arreglo
    - Tiempo de ejecución de ambos algoritmos
    - Relación del tiempo que tardo un algoritmo respecto del otro
Cada vez que se ejecuta el algoritmo se crea un arreglo del doble de 
tamaño con numeros aleatorios. Se omitió la impresión de los arreglos
de gran tamaño para facilitar la lectura.

- Conclusión: con los datos anteriores se puede ver que el algoritmo
implementado con threads se demora cada vez más en ordenar el arreglo
en comparación al algoritmo sin Threads(la relación aumenta), por lo
tanto no se vió beneficiado en su nueva implementación. Por otro lado
quizás la idea no era la más eficiente ya que se crean demasiados hilos
y una idea más adecuada haya sido ejecutar una cantidad detereminada de
hilos en vez de uno por cada mitad que se produzca al dividir el arreglo.

*****Agregar que la clase creada para ejecutar el algoritmo QSThread extiende
a la clase Thread y tiene un método run().
