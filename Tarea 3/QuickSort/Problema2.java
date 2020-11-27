package QuickSort;
import java.time.LocalTime;

public class Problema2 {
    /*
    public static void quickSortThreads(int[] arreglo, int inicio, int fin) {
        if(fin <= inicio) {
            return;
        }
        int pivote = particion(arreglo, inicio, fin);

        Problema2 izq = new Problema2();
        Thread t_izq = new Thread(izq);
        t_izq.start();
        
        Problema2 der = new Problema2();
        Thread t_der = new Thread(der);
        t_der.start();
        
        quickSortThreads(arreglo, inicio, pivote-1);
        quickSortThreads(arreglo, pivote+1, fin);
    }*/

    public static void main (String[] args) {

        // Crear arreglo de prueba
        int n = 10;
        int[] arreglo;
        int min = 0;
        int max = 100;
        double random;
        arreglo = new int[n];
        for(int i = 0; i < n; i++) {
            random = Math.random()*(max-min+1)+min;
            arreglo[i] = (int)random;
        }
        for(int i = 0; i < n; i++) {
            System.out.println(arreglo[i]);
        }

        LocalTime tiempo_inicio = LocalTime.now();
        System.out.println(tiempo_inicio);

        /*
        quickSortRecursivo(arreglo, 0, n-1);
        for(int i = 0; i < n; i++) {
            System.out.println(arreglo[i]);
        }*/
        LocalTime tiempo_fin = LocalTime.now();
        System.out.println(tiempo_fin);

        Thread t = new Thread(new QuickSortThread(arreglo, 0, n-1));
        t.start();

        for(int i = 0; i < n; i++) {
            System.out.println(arreglo[i]);
        }

        /*
        Problema2 thread = new Problema2();
        thread.start();
        Problema2 thread2 = new Problema2();
        thread2.start();*/
    }
}
