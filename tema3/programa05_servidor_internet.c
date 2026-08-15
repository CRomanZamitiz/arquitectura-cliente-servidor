#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <limits.h>

#define QLEN 2

void main(int argc, char *argv[])
{
  struct sockaddr_in servidor;
  struct sockaddr_in cliente;
  struct hostent* info_cliente;
  int fd_s, fd_c;
  int longClient;
  char buf_peticion[256];
  char buf_respuesta[256];

  fd_s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  /*
  int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len)
  La funcion setsockopt indica que este socket fd_s pueda reutilizar una direccion (puerto) que ya esta en uso, o que esta en estado TIME_WAIT
  fd_s Es el descriptor del socket que has creado previamente con socket(). A este socket le vas a cambiar el comportamiento.

  SOL_SOCKET Especifica que la opcion que vas a establecer pertenece al nivel del socket (y no al protocolo TCP, por ejemplo).

  SO_REUSEADDR Esta es la opcion que quieres configurar. Permite reutilizar una direccion local (IP + puerto), incluso si esta en estado de espera (TIME_WAIT).

  &(int){1} Esta es una forma de pasar un puntero a un entero que vale 1.

  sizeof(int) Especifica el tamano de la opcion que estas pasando, que en este caso es un int.
  */
  
  if (setsockopt(fd_s, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
  {
    perror("Server-setsockopt() error!");
    exit(1);
  }
  else
    printf("Server-setsockopt is OK...\n");
  

  memset((char *) &servidor, 0, sizeof(servidor));
  servidor.sin_family = AF_INET;
  servidor.sin_addr.s_addr = INADDR_ANY;
  servidor.sin_port = htons((u_short) atoi(argv[1]) );
  memset(&(servidor.sin_zero), '\0', 8);

  bind(fd_s, (struct sockaddr *) &servidor, sizeof(servidor));

  listen(fd_s, QLEN);

  longClient = sizeof(cliente);

  while(1)
  {
    fd_c = accept(fd_s, (struct sockaddr *) &cliente, &longClient);

    info_cliente = gethostbyaddr((char *) &cliente.sin_addr, sizeof(struct in_addr), AF_INET);
    if( info_cliente == NULL )
    {
      printf("Cliente conectado pero no puedo obtener su origen\n");
    }
    else
    {
      printf("IP obtenida de la estructura hostent: %s\n\n", inet_ntoa(*((struct in_addr *)info_cliente -> h_addr )) );
    }
    // Si la estructura hostent es NULL, una alternativa (la buena practica) es obtener
    // la IP del cliente de la estructura cliente que ya inicializo accept()
    printf("IP obtenida de la estructura socket cliente: %s\n\n", inet_ntoa(cliente.sin_addr) );

    do
    {
      memset(&(buf_peticion), '\0', 256);
      printf("1. buf_peticion=%s\n", buf_peticion);
      int n = recv(fd_c, buf_peticion, sizeof(buf_peticion), 0);
      printf("n=%d\n", n);
      printf("2. longitud de buf_peticion=%d\n", strlen(buf_peticion));
      printf("3. El mensaje del cliente es %s\n", buf_peticion);
      buf_peticion[n-1] = '\n';
      printf("strcmp(buf_peticion,\"adios\")=%d\n", strcmp(buf_peticion,"adios\n"));

      if( strcmp(buf_peticion,"adios\n") == 0 )
        strcpy( buf_respuesta, "respuesta automatica\n" );
      else
      {
	fgets(buf_respuesta, sizeof(buf_respuesta), stdin);
        printf("El mensaje para cliente es %s\n", buf_respuesta);
      }
      send( fd_c, buf_respuesta, strlen(buf_respuesta), 0 );
    }
    while( strcmp(buf_peticion,"adios\n") != 0 );
    printf("Se ha cerrado la conexion con el cliente... Esperando al siguiente\n");
    close(fd_c);
  }
  close(fd_s);
  shutdown( fd_s, SHUT_RDWR );
  exit(0);
}
