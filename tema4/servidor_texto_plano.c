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

#define QLEN 10

void main(int argc, char *argv[])
{
  struct sockaddr_in servidor;
  struct sockaddr_in cliente;
  int fd_s, fd_c;
  int longClient;
  char buf[256];
  char buf_fecha[50];
  int n;
  int num_alumnos = 0;

  fd_s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	      
  memset((char *) &servidor, 0, sizeof(servidor));
  servidor.sin_family = AF_INET;
  servidor.sin_addr.s_addr = INADDR_ANY;
  servidor.sin_port = htons((u_short) atoi(argv[1]));
  memset(&(servidor.sin_zero), '\0', 8);

  bind(fd_s, (struct sockaddr *) &servidor, sizeof(servidor));

  listen(fd_s, QLEN);

  longClient = sizeof(cliente);

  while(1)
  {
    fd_c = accept(fd_s, (struct sockaddr *) &cliente, &longClient);
    char* ip_cliente = inet_ntoa(cliente.sin_addr);

    num_alumnos++;

    n = recv(fd_c, buf, sizeof(buf), 0);
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);
    int length_fecha = sprintf(buf_fecha, "%d : %s-%02d/%02d/%04d %02d:%02d:%02d-", num_alumnos, ip_cliente, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    write(1, buf_fecha, length_fecha);
    write(1, buf, n);
    write(1, "\n", 1);

    char *respuesta = "Registro exitoso";
    send(fd_c, respuesta, strlen(respuesta), 0);

    close(fd_c);
    sleep( 5 );
  }
  close(fd_s);
  shutdown( fd_s, SHUT_RDWR );
  exit(0);
}
