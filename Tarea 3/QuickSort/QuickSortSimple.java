package QuickSort;

public class QuickSortSimple{

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

    public static void quickSort(int[] arreglo, int inicio, int fin) {
        if (fin <= inicio) return;
        int pivot = particion(arreglo, inicio, fin);
        quickSort(arreglo, inicio, pivot-1);
        quickSort(arreglo, pivot+1, fin);
    }
}