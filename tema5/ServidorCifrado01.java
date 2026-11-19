import java.net.*;
import java.io.*;
import java.security.*;
import javax.crypto.*;

public class ServidorCifrado01
{
  public static void main(String a[]) throws Exception
  {
    // Se declaran el Socket Servidor y el Socket Cliente
    ServerSocket serverSocket = null;
    Socket socket = null;
    // arreglo es el buffer grande para almacenar el mensaje enviado por el cliente
    byte arreglo[] = new byte[10000];

    // El servidor genera la llave
    System.out.println( "Generando la llave..." );
    KeyGenerator keyGen = KeyGenerator.getInstance("AES");
    keyGen.init(128);
    Key llave = keyGen.generateKey();
    System.out.println( "llave=" + llave );
    System.out.println( "Llave generada!" );

    // Y guarda el objeto llave en el archivo llave.ser
    ObjectOutput out = new ObjectOutputStream(new FileOutputStream("llave.ser"));
    out.writeObject( llave );
    out.close();

    // De aqui en adelante es el codigo propio del Socket Servidor
    try
    {
      System.out.println("Escuchando por el puerto 8000");
      serverSocket = new ServerSocket(8000); 
    }
    catch(IOException e)
    {
      System.out.println("java.io.IOException generada");
      e.printStackTrace();
    }

    System.out.println("Esperando a que los clientes se conecten...");
    while(true)
    {
      try
      {
        socket = serverSocket.accept();
        System.out.println("Se conecto un cliente: " + socket.getInetAddress().getHostName());

        // Como ya hay socket, obtengo los flujos asociados a este
        DataInputStream dis = new DataInputStream( socket.getInputStream() );
        DataOutputStream dos = new DataOutputStream( socket.getOutputStream() );

        // Despues de la conexion, Servidor y Cliente deben ponerse de acuerdo
        // para ver quien escribe primero y entonces el otro debe leer
        // Como el Cliente escribe, yo debo leer
	// El metodo read() lee el mensaje
	// y lo coloca en el buffer grande llamado arreglo
	// pero ademas devuelve el numero real de bytes leidos
        int bytesLeidos = dis.read(arreglo);
        System.out.println("bytes leidos=" + bytesLeidos);

	// Ahora se crea un nuevo arreglo2 que sera del tamano justo del
	// numero real de bytes leidos que devolvio read()
        byte arreglo2[]  = new byte[bytesLeidos];
	
	// Con este for se copia del arreglo grande al arreglo justo
        for(int i=0; i < bytesLeidos; i++ )
        {
          arreglo2[i] = arreglo[i];
        }

        // Se crea el objeto Cipher con getInstance() pasandole la transformacion
        // Cipher cifrar = Cipher.getInstance("AES");
        Cipher cifrar = Cipher.getInstance("AES/ECB/PKCS5Padding");

	// Se llama a init() pasando DECRYPT_MODE y la llave
        cifrar.init(Cipher.DECRYPT_MODE, llave);
        bytesToBits( arreglo2 );

	// Para DECRYPT_MODE el metodo doFinal() recibe como argumento
	// el arreglo de bytes del texto cifrado y
	// devuelve el arreglo de bytes del texto plano
        byte[] newPlainText = cifrar.doFinal(arreglo2);
        System.out.println( "El argumento DESENCRIPTADO es:" );

        // NO SE DEBE PASAR A String
        // System.out.println( new String(newPlainText, "UTF8") );

	// Con este for se imprime en pantalla el arreglo de bytes del texto plano
        for(int i=0; i < newPlainText.length; i++)
          System.out.print( (char)newPlainText[i] );
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

