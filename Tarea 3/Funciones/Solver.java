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
        String regex = "[a-z]\\([a-z]\\)";
        String funcion = "";
        String get_cuerpo;
        String cuerpo = "";
        String calculo;

        // Buscar funcion en el arreglo
        for (int i = 0; i < cantidad; i++) {

            if (Boolean.compare(funciones[i].startsWith(input.substring(0, 1)), true) == 0) {
                funcion = funciones[i];
                get_cuerpo = funcion.substring(5, funcion.length());
                cuerpo = cuerpo.concat(get_cuerpo);
            } else {
                System.out.println("Funcion: " + funciones[i] + " no empieza con " + input);
            }
        }

        Pattern p = Pattern.compile(regex);
        Matcher m = p.matcher(cuerpo);

        boolean coincidencia = m.find();
        if (Boolean.compare(coincidencia, false) == 0) {
            char variable = funcion.charAt(2);
            char valor = input.charAt(2);

            calculo = cuerpo.replace(variable, valor);
            System.out.println(calculo);

            ScriptEngineManager mgr = new ScriptEngineManager();
            ScriptEngine engine = mgr.getEngineByName("JavaScript");

            try{
                System.out.println(engine.eval(calculo));
            }
            catch(ScriptException e){
                System.out.println("xd");
            }
            return (1);
        } else {
            // Crear ramitas

        }
        return (1);
    }
}
