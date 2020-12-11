-José Montecinos 201804541-1
-Sofía Palet     201873570-1

NOTAS DE IMPLEMENTACION PREGUNTA 1:

-Se empleó la librería ScriptEngine para resolver las expresiones
matemáticas, la cual no nos compilaba en Windows por una razón que
no logramos determinar, sin embargo al probar en Linux todo funcio-
naba bien asi que recomendamos ejecutar en Linux.

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

- Agregar que la clase creada para ejecutar el algoritmo extiende
a la clase Thread y tiene un método run().
