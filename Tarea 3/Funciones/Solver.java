package Funciones;

import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.script.ScriptEngineManager;
import javax.script.ScriptEngine;
import javax.script.ScriptException;

public class Solver extends Thread {
    String[] funciones;
    String input;
    int cantidad;

    public void run() {
        calcular(funciones, input, cantidad);
    }

    public Solver(String[] funciones_aux, String input_aux, int cantidad_aux) {
        funciones = funciones_aux;
        input = input_aux;
        cantidad = cantidad_aux;
    }

    public static int calcular(String[] funciones, String input, int cantidad) {
        String regex = "[a-z]\\(([a-z])+\\)";
        String funcion = "";
        String get_expresion;
        String expresion = "";
        String calculo;

        // Buscar funcion en el arreglo
        for (int i = 0; i < cantidad; i++) {
            if (Boolean.compare(funciones[i].startsWith(input.substring(0, 1)), true) == 0) {
                funcion = funciones[i];
                int igual = funcion.indexOf("=");
                get_expresion = funcion.substring(igual+1, funcion.length());
                expresion = expresion.concat(get_expresion);
            }
        }

        Pattern p = Pattern.compile(regex);
        Matcher m = p.matcher(expresion);

        System.out.println("Expresion: " + expresion);
        boolean coincidencia = m.find();
        if (Boolean.compare(coincidencia, false) == 0) {
            // No hay mas funciones en la expresion
            int cierre_parentesis_funcion = funcion.indexOf(")");
            int cierre_parentesis_input = input.indexOf(")");
            String variable = funcion.substring(2, cierre_parentesis_funcion);
            String valor = input.substring(2, cierre_parentesis_input);

            calculo = expresion.replaceFirst(variable, valor);
            System.out.println("Calculo: " + calculo);

            ScriptEngineManager mgr = new ScriptEngineManager();
            ScriptEngine engine = mgr.getEngineByName("JavaScript");

            try{
                int resultado = (int) engine.eval(calculo);
                System.out.println(resultado);
                return(resultado);
            }
            catch(ScriptException e){
                e.printStackTrace();
                return(0);
            }

        } else {
            // Buscar funciones en la expresion
            for (int i = 0; i < cantidad; i++) {
                if (expresion.indexOf(funciones[i].substring(0, 3)) > -1) {
                    System.out.println("Estoy revisando: " + funciones[i]);
                    int cierre_parentesis_funcion = funciones[i].indexOf(")");
                    int cierre_parentesis_input = input.indexOf(")");

                    String variable = funciones[i].substring(2, cierre_parentesis_funcion);
                    String valor = input.substring(2, cierre_parentesis_input);

                    String nueva_funcion = funciones[i].substring(0, cierre_parentesis_input+1);
                    System.out.println(nueva_funcion);

                    nueva_funcion = nueva_funcion.replace(variable, valor);
                    int nuevo_resultado = calcular(funciones, nueva_funcion, cantidad);
                    System.out.println("Nuevo Resultado: " + nuevo_resultado + " Funcion a reemplazar: " + funciones[i].substring(0, 4) + " Nueva funcion: " + nueva_funcion);

                    System.out.println("Expresion antes del replace: " + expresion);
                    expresion = expresion.replace(funciones[i].substring(0,4), Integer.toString(nuevo_resultado));
                    System.out.println("Nueva luego del replace: " + expresion);
                }
            }
            System.out.println("Expresion final: " + expresion);
            ScriptEngineManager mgr = new ScriptEngineManager();
            ScriptEngine engine = mgr.getEngineByName("JavaScript");

            try{
                int resultado_final = (int) engine.eval(expresion);
                System.out.println(resultado_final);
                return(resultado_final);
            }
            catch(ScriptException e){
                e.printStackTrace();
            }
        }
        return (1);
    }
}
