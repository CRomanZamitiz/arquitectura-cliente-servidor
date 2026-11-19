import java.net.*;
import java.io.*;
import java.security.*;
import javax.crypto.*;

public class ClienteCifrado02
{
  public static void main(String a[]) throws Exception
  {
    // Se declara el socket, el unico que tiene el cliente
    Socket socket = null;

    try
    {
      // Esto se comenta porque el Cliente no debe generar su propia llave
      /*
      System.out.println( "Generando la llave..." );
      KeyGenerator keyGen = KeyGenerator.getInstance("AES");
      keyGen.init(56);
      Key llave = keyGen.generateKey();
      System.out.println( "Llave generada!" );
      System.out.println( "llave=" + llave );
      */

      // En este ejemplo, el Cliente tomara la llave del archivo llave.ser
      ObjectInput in = new ObjectInputStream(new FileInputStream("llave.ser"));
      // El metodo readObject() devuelve un Object generico 
      // por eso se hace cast explicito a tipo Key generico
      Key llave = (Key)in.readObject();
      System.out.println( "llave=" + llave );
      in.close();
      
      // Esto es el codigo del Socket Cliente
      System.out.println("Me conecto al puerto 9999 del servidor");
      socket = new Socket(a[0],9999);

      // Como ya hay socket, obtengo los flujos asociados a este
      DataInputStream dis = new DataInputStream( socket.getInputStream() );
      DataOutputStream dos = new DataOutputStream( socket.getOutputStream() );

      // Ya que me conecte con el Servidor, debo leer del teclado 
      // para despues eso mismo enviarlo al Servidor
      BufferedReader br = new BufferedReader( new InputStreamReader( System.in ) );
      // Peticion es lo que envia el Cliente
      String peticion = br.readLine();
      System.out.println( "Mi peticion es: " + peticion );
      System.out.println( "Ahora encriptamos la peticion..." );
      byte[] arrayPeticion = peticion.getBytes();

      // EMPIEZA EL CODIGO DE CIFRADO
      // Cipher cifrar = Cipher.getInstance("AES");
      // Se crea el objeto Cipher con getInstance() pasandole la transformacion
      Cipher cifrar = Cipher.getInstance("AES/ECB/PKCS5Padding");

      // Se llama a init() de Cipher pasando ENCRYPT_MODE y la llave
      cifrar.init(Cipher.ENCRYPT_MODE, llave);

      // Para ENCRYPT_MODE el metodo doFinal() recibe como argumento
      // el arreglo de bytes del texto plano y
      // devuelve el arreglo de bytes del texto cifrado
      byte[] cipherText = cifrar.doFinal( arrayPeticion );
      System.out.println( "El argumento ENCRIPTADO es:" );

      // NO SE DEBE PASAR A String
      // System.out.println( new String( cipherText ) );

      // Con este for se imprime en pantalla el arreglo de bytes del texto cifrado
      for(int i=0; i < cipherText.length; i++)
        System.out.print( (char)cipherText[i] );
      System.out.println( "" );

      // Ahora el cliente escribe por el socket el arreglo de bytes del texto cifrado
      // Como yo cliente escribo la peticion,
      // el Servidor debe leer
      bytesToBits( cipherText );
      // el metodo write() recibe:
      // 1. el arreglo de bytes del texto cifrado
      // 2. el inicio de la porcion del arreglo a escribir
      //    en nuestro caso queremos desde el inicio es decir indice 0
      // 3. la longitud de la porcion del arreglo que queremos escribir
      //    como queremos escribir todo el arreglo, le pasamos 
      //    arreglo.length
      dos.write( cipherText, 0, cipherText.length );

      dos.close();
      dis.close();
      socket.close();
    }
    catch(IOException e)
    {
      System.out.println("java.io.IOException generada");
      e.printStackTrace();
    }
  }

  public static void bytesToBits( byte[] texto )
  {
    StringBuilder stringToBits = new StringBuilder();
    for( int i=0; i < texto.length; i++ )
    {
      StringBuilder binary = new StringBuilder();
      byte b = texto[i];
      int val = b;
      for( int j = 0; j < 8; j++ )
      {
        binary.append( (val & 128) == 0 ? 0 : 1 );
        val <<= 1;
      }
      System.out.println( (char)b + " \t " + b + " \t " + binary );
      stringToBits.append( binary );
    }
    System.out.println( "El mensaje completo en bits es:" + stringToBits );
  }

}
