package Funciones;

import java.io.File;
import java.util.Scanner;
import java.io.FileNotFoundException;

public class Problema1 {
    public static void main(String[] args) {
        try {
            File archivo = new File("Funciones/funciones.txt");
            Scanner scan = new Scanner(archivo);
            Scanner input = new Scanner(System.in);
            String func_input;
            int salida = 0;

            String cantidad_str = "";
            int cantidad;
            String aux = "";
            String[] funciones;

            // Obtener cantidad de funciones
            cantidad_str = cantidad_str.concat(scan.nextLine());
            cantidad = Integer.parseInt(cantidad_str);
            funciones = new String[cantidad];

            // Leer funciones
            int i = 1;
            while (scan.hasNextLine()) {
                aux = "";
                aux = aux.concat(scan.nextLine());
                funciones[i - 1] = aux;
                i++;
            }
            scan.close();

            while (salida != 1) {
                System.out.println("Ingrese una funcion: (1 para salir)");
                func_input = input.nextLine();
                if (func_input.equals("1")) {
                    salida = 1;
                } else {
                    Thread t = new Thread(new Solver(funciones, func_input, cantidad));
                    t.start();
                }
            }
            input.close();

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }
}