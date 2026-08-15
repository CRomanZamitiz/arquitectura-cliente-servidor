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

void main( int argc, char *argv[] )
{
  struct sockaddr_in servidor;
  struct sockaddr_in cliente;
  struct hostent* info_cliente;
  int fd_s, fd_c;
  int longClient;
  int num_cliente = 0;

  fd_s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	      
  memset((char *) &servidor, 0, sizeof(servidor));
  servidor.sin_family = AF_INET;
  servidor.sin_addr.s_addr = INADDR_ANY;
  servidor.sin_port = htons( (u_short) atoi(argv[1]) );
  memset(&(servidor.sin_zero), '\0', 8);

  bind(fd_s, (struct sockaddr *) &servidor, sizeof(servidor));

  listen(fd_s, QLEN);

  longClient = sizeof(cliente);

  while(1)
  {
    fd_c = accept(fd_s, (struct sockaddr *) &cliente, &longClient);
    num_cliente++;

    /* Lo siguiente es para obtener la fecha y hora y se impriman en pantalla */
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);
    printf("%02d/%02d/%04d %02d:%02d:%02d-Cliente conectado ", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    printf(" desde: %s\n\n", inet_ntoa(cliente.sin_addr) );

    if( fork() == 0 )
    {
      char buf_peticion[256];
      char buf_respuesta[256];
      /* el hijo no necesita el file descriptor del socket servidor,
      porque el no aceptara conexion de cliente, solo se comunica con el por lo que
      solo necesita el file descriptor del socket cliente */
      close( fd_s );
      // printf("0. buf_peticion=%s\n", buf_peticion);
      do
      {
	memset(&(buf_peticion), '\0', 256);
        int n = recv(fd_c, buf_peticion, sizeof(buf_peticion), 0);
        // printf("2. longitud de buf_peticion=%d\n", strlen(buf_peticion));
        printf("3. El mensaje del cliente %d es %s\n", num_cliente, buf_peticion);
        buf_peticion[n] = '\0';

        fgets(buf_respuesta, sizeof(buf_respuesta), stdin);
        printf("El mensaje para cliente %d es %s\n", num_cliente, buf_respuesta);
        send( fd_c, buf_respuesta, strlen(buf_respuesta), 0 );
      }
      while( strcmp(buf_peticion,"adios\n") != 0 );
      printf("Se ha cerrado la conexion con el cliente... Este hijo termina\n");
      close(fd_c);
      exit( 0 );
    }
    else
    {
      /* El padre no necesita el file descriptor del socket cliente 
      porque el no se hablara con los clientes, los que lo haran son los hijos.
      Lo que si necesita el padre es mantener abierto el file descriptor del
      socket servidor porque en el sig ciclo del while llamara a la funcion
      accept() para aceptar a otro cliente */
      close( fd_c );
    }
  }
  close(fd_s);
  shutdown( fd_s, SHUT_RDWR );
  exit(0);
}
