#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>

void main(int argc, char *argv[])
{
  struct sockaddr_in server;
  struct hostent *sp;
  int sd;
  int n;
  char *host;
  char buf_peticion[256];
  char buf_respuesta[256];

  sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  memset((char *) &server, 0, sizeof(struct sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_port = htons( (u_short) atoi(argv[2]) );
  sp = gethostbyname( argv[1] );
  memcpy(&server.sin_addr, sp->h_addr, sp->h_length);

  connect(sd, (struct sockaddr *) &server, sizeof(struct sockaddr_in));

  while( strcmp(buf_peticion,"adios\n") != 0 )
  {
    fgets(buf_peticion, 256, stdin);
    printf("El mensaje al servidor es %s\n", buf_peticion);
    send( sd, buf_peticion, strlen(buf_peticion), 0 );

    n = recv ( sd, buf_respuesta, sizeof(buf_respuesta), 0 );
    printf(" numero real de bytes leidos=%d\n",n);
    write(1,buf_respuesta,n);
  }

  close(sd);
  exit(0);
}


