package Funciones;

import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.script.ScriptEngineManager;
import javax.script.ScriptEngine;
import javax.script.ScriptException;

/*
Nombre: Solver
Parametros: ninguno
Retorno: no retorna
Descripcion: Clase que se encarga de calcular la funcion que es entregada en el input.
*/
public class Solver extends Thread {
    String[] funciones;
    String input;
    int cantidad;
    static float[] resultado;

    public void run() {
        resultado[0] = calcular(funciones, input, cantidad);
    }

    /*
    Nombre: Solver (constructor)
    Parametros:
        - String[] funciones_aux: Arreglo de strings con las funciones del archivo.
        - String input_aux: String con la funcion y valor entregada en el input.
        - int cantidad_aux: Cantidad de funciones presentes en el archivo.
        - float[] resultado_aux: Arreglo de flotantes para guardar los resultados del calculo.
    Retorno: no retorna
    Descripcion: Constructor de la clase Solver.
    */
    public Solver(String[] funciones_aux, String input_aux, int cantidad_aux, float[] resultado_aux) {
        funciones = funciones_aux;
        input = input_aux;
        cantidad = cantidad_aux;
        resultado = resultado_aux;
    }

    /*
    Nombre: calcular_expresion
    Parametros:
        - String funcion: Funcion que sera evaluada.
        - String input: String con la funcion y valor entregada en el input.
        - String expresion: Cuerpo de la funcion para ser calculado.
    Retorno: float, retorna el resultado de la funcion evaluada.
    Descripcion: Recibe la variable de la funcion, el valor entregado en el input y la expresion a calcular,luego crea un string
        reemplazando el valor en la variable y calcula la expresion, retornando el resultado.
    */
    public static float calcular_expresion(String funcion, String input, String expresion) {
        String calculo;
        String variable = funcion.substring(funcion.indexOf("("), funcion.indexOf(")")+1);
        String valor = input.substring(input.indexOf("("), input.indexOf(")")+1);

        calculo = expresion.replaceAll(variable, valor);

        ScriptEngineManager mgr = new ScriptEngineManager();
        ScriptEngine engine = mgr.getEngineByName("JavaScript");

        try{
            float resultado = 0;
            String tipo = engine.eval(calculo).getClass().getSimpleName();
            if (tipo.equals("Integer")) {
                int auxiliar = Integer.parseInt(engine.eval(calculo).toString());
                resultado = (float) auxiliar;
            }
            else if (tipo.equals("Double")) {
                double auxiliar = Double.parseDouble(engine.eval(calculo).toString());
                resultado = (float) auxiliar;
            }
            return(resultado);
        }
        catch(ScriptException e){
            e.printStackTrace();
            return(0);
        }
    }

    /*
    Nombre: calcular
    Parametros:
        - String[] funciones: Arreglo de strings con las funciones del archivo.
        - String input: String con la funcion y valor entregada en el input.
        - int cantidad: Cantidad de funciones presentes en el archivo.
    Retorno: float, retorna el resultado del input.
    Descripcion: Primero busca la funcion entregada en el input entre las funciones del archivo, luego comprueba si la funcion que se
        quiere calcular tiene otras funciones en su expresion. Si la funcion que se quiere calcular no tiene funciones en su cuerpo, entonces
        lo calcula, si no, revisa las funciones que estan en el cuerpo y crea hebras por cada funcion para obtener su resultado.
        Finalmente retorna el resultado pedido en el input.
    */
    public static float calcular(String[] funciones, String input, int cantidad) {
        String regex = "[a-z]\\(([a-z])+\\)";
        String funcion = "";
        String expresion = "";
        String calculo;

        // Buscar funcion en el arreglo
        for (int i = 0; i < cantidad; i++) {
            if (Boolean.compare(funciones[i].startsWith(input.substring(0, input.indexOf("(")+1)), true) == 0) {
                funcion = funciones[i];
                String get_expresion = funcion.substring(funcion.indexOf("=")+1, funcion.length());
                expresion = expresion.concat(get_expresion);
            }
        }

        Pattern p = Pattern.compile(regex);
        Matcher m = p.matcher(expresion);
        boolean coincidencia = m.find();

        if (Boolean.compare(coincidencia, false) == 0) {
            // No hay mas funciones en la expresion
            float resultado_expresion = calcular_expresion(funcion, input, expresion);
            return(resultado_expresion);

        } else {
            // Buscar funciones en la expresion
            for (int i = 0; i < cantidad; i++) {
                if (expresion.indexOf(funciones[i].substring(0, 3)) > -1) {

                    String variable = funciones[i].substring(funciones[i].indexOf("("), funciones[i].indexOf(")"));
                    String valor = input.substring(input.indexOf("("), input.indexOf(")"));

                    String nueva_funcion = funciones[i].substring(0, funciones[i].indexOf(")")+1);

                    nueva_funcion = nueva_funcion.replace(variable, valor);
                    Thread th = new Thread(new Solver(funciones, nueva_funcion, cantidad, resultado));
                    th.start();
                    try{th.join();}catch(Exception e){}

                    expresion = expresion.replace(funciones[i].substring(0,4), Float.toString(resultado[0]));
                }
            }

            ScriptEngineManager mgr = new ScriptEngineManager();
            ScriptEngine engine = mgr.getEngineByName("JavaScript");

            try{
                float resultado_final = 0;
                String tipo = engine.eval(expresion).getClass().getSimpleName();
                if (tipo.equals("Integer")) {
                    int auxiliar = Integer.parseInt(engine.eval(expresion).toString());
                    resultado_final = (float) auxiliar;
                }
                else if (tipo.equals("Double")) {
                    double auxiliar = Double.parseDouble(engine.eval(expresion).toString());
                    resultado_final = (float) auxiliar;
                }
                return(resultado_final);
            }
            catch(ScriptException e){
                e.printStackTrace();
            }
        }
        return (1);
    }
}
