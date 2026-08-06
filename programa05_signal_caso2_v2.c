#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

/*
Para manejar senales, hay que usar la funcion signal():
man signal
Este programa muestra el caso 2: trata la senal y termina el proceso.
En este caso, aunque el proceso tambien esta en un ciclo infinito
el proceso muere, porque en nuestro tratamiento de la senal
se agrega un contador y una condicion con exit(0)
*/

/*
 * Rutina de tratamiento para la senal 2 SIGINT
 */
void manejador_senales(int sig)
{
  static int cnt = 1;

  printf("Senal numero %d recibida\n", sig);
  if( sig == SIGINT )
  {
    printf("senal SIGINT %d recibida\n", sig);
    printf("contador = %d\n", cnt);
    exit( 0 );
  }
}

/*
 * main: Inicializa el manejador de la senal SIGINT y
 * se pone en espera para recibir la senal
 */
int main()
{
  if( signal(SIGINT, manejador_senales) == SIG_ERR )
  {
    printf("no puedo cachar SIGINT\n");
    perror("signal");
  }

  while( 1 )
  {
    printf("En espera de Control-C para tratar SIGINT\n");
    sleep( 5 );
  }

  return 0;
}
