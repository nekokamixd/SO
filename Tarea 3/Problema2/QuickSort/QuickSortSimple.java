package QuickSort;

/*
Nombre: QuickSortSimple
Parametros: ninguno
Retorno: no retorna
Descripcion: Clase encargada de ordenar el arreglo entregado usando QuickSort de manera recursiva.
*/
public class QuickSortSimple{

    /*
    Nombre: particion
    Parametros:
        - int[] arreglo:
        - int inicio:
        - int fin:
    Retorno: int,
    Descripcion:
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
    Nombre: quickSort
    Parametros:
        - int[] arreglo: Arreglo que se quiere ordenar usando QuickSort.
        - int inicio:
        - int fin:
    Retorno: void, no retorna
    Descripcion: Recibe un arreglo, calcula la posicion de la particion en el arreglo que sera el pivote y
        vuelve a realizar quickSort con los sub arreglos creados hasta que se llegue al arreglo mas pequeño.
    */
    public static void quickSort(int[] arreglo, int inicio, int fin) {
        if (fin <= inicio) return;
        int pivot = particion(arreglo, inicio, fin);
        quickSort(arreglo, inicio, pivot-1);
        quickSort(arreglo, pivot+1, fin);
    }
}
