#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

void main(int argc, char *argv[])
{
  struct sockaddr_in socket_cliente;
  struct hostent *h;
  int fd;
  int n;
  char *host;
  char buf[128];

  printf( "1. Se llama a la funcion socket(...) y devuelve el file descriptor propio del cliente fd\n" );
  printf( "fd = socket(AF_INET, SOCK_STREAM, 0);\n" );
  fd = socket(AF_INET, SOCK_STREAM, 0);

  printf( "2. Se inicializan los atributos de la variable struct sockaddr_in llamada socket_cliente\n\n" );
  printf( "Pero se inicializan con datos del servidor, como son la IP y el puerto\n\n" );

  memset((char *) &socket_cliente, 0, sizeof(socket_cliente));
  socket_cliente.sin_family = AF_INET;
  socket_cliente.sin_port = htons((u_short) atoi(argv[2]) );
  h = gethostbyname( argv[1] );
  memcpy(&socket_cliente.sin_addr, h->h_addr, h->h_length);

  printf( "3. La funcion connect() es la que intenta conectarse al servidor\n" );
  printf( "Esta funcion se relaciona con la funcion accept(...) del servidor\n\n" );
  connect(fd, (struct sockaddr *) &socket_cliente, sizeof(socket_cliente));
    
  printf( "4. Como el servidor hace send(...) entonces el cliente hace recv(...)\n" );
  printf( "n = recv(fd, buf, sizeof(buf), 0);\n" );
  printf( "Lo recibido con recv(...) se va a imprimir a \"algun lado\" con la funcion write(...)\n" );
  printf( "write(1, buf, n);\n" );
  printf( "Revisa la documentacion de write(...), a donde manda el mensaje?\n\n" );
  n = recv(fd, buf, sizeof(buf), 0);
  printf("numero real de bytes leidos=%d\n",n);
  write(1, buf, n);
  printf( "\n" );

  printf( "5. Por ultimo, se cierra el file descriptor\n" );
  printf( "close(fd);\n" );
  close(fd);
  exit(0);
}
