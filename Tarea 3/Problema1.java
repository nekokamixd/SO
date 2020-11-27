import java.io.File;
import java.util.Scanner;
import java.io.FileNotFoundException;

public class Problema1 {
  public static void main (String[] args) {
    try {
      File archivo = new File("funciones.txt");
      Scanner lector = new Scanner(archivo);
      while (lector.hasNextLine()) {
        String data = lector.nextLine();
        System.out.println(data);
      }
      lector.close();
    } catch (FileNotFoundException e) {
      System.out.println("Hubo un error.");
      e.printStackTrace();
    }
  }
}