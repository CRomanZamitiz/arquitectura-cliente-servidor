#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define PORT 4898

void main()
{
  struct sockaddr_in servidor;
  struct sockaddr_in cliente;
  struct hostent* info_cliente;
  int fd_s, fd_c;
  int longClient;
  char buf[128];

  printf( "1. Se llama a la funcion socket(...) y devuelve el file descriptor del servidor fd_s\n" );
  printf( "fd_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);\n\n" );
  fd_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	      
  printf( "2. Se inicializan los atributos de la variable struct sockaddr_in llamada servidor\n\n" );
  memset((char *) &servidor, 0, sizeof(servidor));
  servidor.sin_family = AF_INET;
  servidor.sin_addr.s_addr = INADDR_ANY;
  servidor.sin_port = htons((u_short) PORT);
  memset(&(servidor.sin_zero), '\0', 8);

  printf( "3. bind(fd_s, (struct sockaddr *) &servidor, sizeof(servidor));\n\n" );
  bind(fd_s, (struct sockaddr *) &servidor, sizeof(servidor));

  printf( "4. listen(fd_s, 1);\n\n" );
  listen(fd_s, 1);

  longClient = sizeof(cliente);

  printf( "5. La funcion accept() recibe como argumento el file descriptor del servidor y recibe tambien\n" );
  printf( "la struct sockaddr_in del cliente y devuelve el file descriptor del cliente\n" );
  printf( "fd_c = accept(fd_s, (struct sockaddr *) &cliente, &longClient);\n" );
  printf( "La funcion accept(...) se quedara ahi indefinidamente hasta que se conecte un cliente\n\n" );
  fd_c = accept(fd_s, (struct sockaddr *) &cliente, &longClient);

  printf( "6. Se llama a la funcion gethostbyaddr(...) para obtener la informacion\n" );
  printf( "propia del cliente y lo que devuelve la funcion, se asigna a la variable\n" );
  printf( "de tipo struct hostent* llamada info_cliente:\n" );
  printf( "info_cliente = gethostbyaddr((char *) &cliente.sin_addr, sizeof(struct in_addr), AF_INET);\n\n" );
  info_cliente = gethostbyaddr((char *) &cliente.sin_addr, sizeof(struct in_addr), AF_INET);

  printf( "7. Gracias a la sentencia anterior, por ejemplo, podemos saber el nombre del host cliente\n");
  printf( "info_cliente -> h_name\n" );
  printf("Conectado desde: %s\n\n", info_cliente -> h_name);

  printf( "8. Se le envia al cliente una frase \"El mensaje del servidor es: Conexion exitosa!\", con la funcion send(...)\n" );
  printf( "send(fd_c, buf, strlen(buf), 0);\n\n" );
  sprintf(buf, "El mensaje del servidor es: Conexion exitosa!\n");
  send(fd_c, buf, strlen(buf), 0);

  printf( "9. Se cierra el file descriptor del cliente\n" );
  printf( "close(fd_c);\n" );
  close(fd_c);

  printf( "10. Por ultimo, ahora si se cierra el file descriptor del servidor\n" );
  printf( "close(fd_s);\n" );
  close(fd_s);
  shutdown( fd_s, SHUT_RDWR );
  exit(0);
}
