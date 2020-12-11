import QuickSort.*;
import java.time.LocalTime;
import static java.time.temporal.ChronoUnit.MICROS;

/*
Nombre: Problema2
Parametros: ninguno
Retorno: no retorna
Descripcion: Crea un arreglo con numeros al azar para ordenar usando el algoritmo de QuickSort, luego se ordena el arreglo usando QuickSort
    con threads e imprime el tiempo que tarda en ordenarlo, despues se ordena usando QuickSort recursivo e imprime el tiempo que tarda
    en ordenarlo.
*/
public class Problema2 {
    public static void main (String[] args) {

        /*Definiciones: arreglos y variables*/
        int n = 10;
        int[] arreglo, arreglo_aux;
        int min = 0;
        int max = 100;
        double random;
        arreglo = new int[n];
        arreglo_aux = new int[n];
        for(int i = 0; i < n; i++) {
            random = Math.random()*(max-min+1)+min;
            arreglo[i] = arreglo_aux[i] = (int)random;
        }

        LocalTime tiempo_fin, tiempo_inicio;

        // Imprimir arreglo desordenado
        System.out.println("-------------------");
        System.out.println("Arreglo:\n");
        for(int i = 0; i < n; i++) {
            System.out.println(arreglo[i] );
        }
        System.out.println("\n-------------------");


        /******QuickSortSimple******/
        System.out.println("Arreglo ordenado: \n");

        QuickSortSimple qs = new QuickSortSimple();
        tiempo_inicio = LocalTime.now();
        qs.quickSort(arreglo_aux,0,n-1);
        tiempo_fin = LocalTime.now();
        for(int i = 0; i < n; i++) {
            System.out.println(arreglo_aux[i] );
        }
        System.out.println();
        System.out.println("Tiempo de inicio:  " + tiempo_inicio);
        System.out.println("Tiempo de término: " + tiempo_fin);
        System.out.println("Tiempo de ejecución (microsegundos): " + MICROS.between(tiempo_inicio, tiempo_fin));
        System.out.println("\n-------------------");


        //******QuickSortThread******/
        Thread t = new Thread(new QuickSortThread(arreglo, 0, n-1));
        System.out.println("Arreglo ordenado con Threads: \n");
        // Ejecución de Sort (con threads) y calculo de su tiempo de ejecucion
        tiempo_inicio = LocalTime.now();
        t.start();
        try{t.join();}catch(Exception e){};
        tiempo_fin = LocalTime.now();
        for(int i = 0; i < n; i++) {
            System.out.println(arreglo[i] );
        }

        System.out.println();
        System.out.println("Tiempo de inicio: " + tiempo_inicio);
        System.out.println("Tiempo de término:    "+ tiempo_fin);
        System.out.println("Tiempo de ejecución (microsegundos): " + MICROS.between(tiempo_inicio, tiempo_fin));
        System.out.println("\n-------------------");

        /*Comparación de algoritmos*/
        System.out.println("Comparación de algoritmos:\n");    
        for (n = 10; n <= 10240; ){
            // variables
            long duracionThread = 0;
            long duracionSimple = 0;
            // crear nuevos arreglos tamaño n
            arreglo = new int[n];
            arreglo_aux = new int[n];
            for(int i = 0; i < n; i++) {
                random = Math.random()*(max-min+1)+min;
                arreglo[i] = arreglo_aux[i] = (int)random;
            }

            /*QuickSort con Threads*/
            t = new Thread(new QuickSortThread(arreglo, 0, n-1));
            tiempo_inicio = LocalTime.now();
            t.start();
            try{t.join();}catch(Exception e){};
            tiempo_fin = LocalTime.now();

            duracionThread = MICROS.between(tiempo_inicio, tiempo_fin);


            /*QuickSortSimple sin threads */
            qs = new QuickSortSimple();
            tiempo_inicio = LocalTime.now();
            qs.quickSort(arreglo_aux,0,n-1);
            tiempo_fin = LocalTime.now();

            duracionSimple = MICROS.between(tiempo_inicio, tiempo_fin);

            /*Prints para comparación */
            System.out.println("Tamaño del arreglo: " + n);
            System.out.println("Tiempo QuickSortSimple: " + duracionSimple);
            System.out.println("Tiempo QuickSOrtThread: " + duracionThread);
            System.out.println("Relación Thread/Simple : "+(duracionThread/duracionSimple));
            System.out.println();

            // cambio de valor de n
            n = n*2;
            max = n*100;
        }
    }
}
