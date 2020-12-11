package QuickSort;

/*
Nombre: QuickSortThread
Parametros: ninguno
Retorno: no retorna
Descripcion: Clase encargada de ordenar el arreglo entregado usando QuickSort con threads.
*/
public class QuickSortThread extends Thread {
    volatile static int[] arreglo;
    volatile static int flag = -1;
    int inicio;
    int fin;

    /*
    Nombre: run
    Parametros: ninguno
    Retorno: no retorna
    Descripcion: ejecuta el método run inherente a la clase Thread. Dentro de él
        se ejecuta la función para ordenar el arreglo.
    */
    public void run(){
        quickSortRecursivo(arreglo, inicio, fin);
    }

    /*
    Nombre: QuickSortThread
    Parametros:
        - int[] arreglo_aux:
        - int inicio_aux:
        - int fin_aux:
    Retorno: no retorna
    Descripcion: Constructor de la clase QuickSortThread.
    */
    public QuickSortThread(int[] arreglo_aux, int inicio_aux, int fin_aux) {
        arreglo = arreglo_aux;
        this.inicio = inicio_aux;
        this.fin = fin_aux;
    }

    /*
    Nombre: particion
    Parametros:
        - int[] arreglo:
        - int inicio:
        - int fin:
    Retorno: int,
    Descripcion: toma el último elemento del arrelgo como pivote y separa el arreglo
        en relación a este en números menores y mayores. Finalmente el pivote se deja
        en la posición que quedará en la posición adecuada y no se moverá más.
    */
    public static int particion(int[] arreglo, int inicio, int fin) {
        int pivote = fin;
        int contador = inicio;
        int auxiliar;

        for(int i = inicio; i < fin; i++) {
            if(arreglo[i] < arreglo[pivote]) {
                auxiliar = arreglo[contador];
                arreglo[contador] = arreglo[i];
                arreglo[i] = auxiliar;
                contador++;
            }
        }
        auxiliar = arreglo[pivote];
        arreglo[pivote] = arreglo[contador];
        arreglo[contador] = auxiliar;
        return contador;
    }

    /*
    Nombre: quickSortRecursivo
    Parametros:
        - int[] arreglo: Arreglo que se quiere ordenar usando QuickSort.
        - int inicio:
        - int fin:
    Retorno: no retorna
    Descripcion: Recibe un arreglo, calcula la posicion de la particion en el arreglo que sera el pivote y
        crea threads llamando a QuickSortThread con los sub arreglos creados para ordenarlos hasta que se llegue al arreglo mas pequeño.
    */
    public static void quickSortRecursivo(int[] arreglo, int inicio, int fin) {
        if(fin <= inicio) {
            return;
        }
        int pivote = particion(arreglo, inicio, fin);

        Thread t1 = new Thread(new QuickSortThread(arreglo, inicio, pivote-1));
        Thread t2 = new Thread(new QuickSortThread(arreglo, pivote+1, fin));
        t1.start();
        t2.start();
        try{t1.join();}catch(Exception e){}
        try{t2.join();}catch(Exception e){}
    }
}
