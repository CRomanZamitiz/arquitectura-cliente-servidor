import java.io.*;
import java.security.*;
import javax.crypto.*;

public class GeneradorLlave
{
 public static void main(String a[]) throws Exception
 {
  System.out.println( "Generando la llave..." );
  KeyGenerator keyGen = KeyGenerator.getInstance("AES");
  keyGen.init(128);
  Key llave = keyGen.generateKey();
  System.out.println( "llave=" + llave );
  System.out.println( "Llave generada!" );

  ObjectOutput out = new ObjectOutputStream(new FileOutputStream("llave.ser"));
  out.writeObject( llave );
  out.close(); 
 }
}
