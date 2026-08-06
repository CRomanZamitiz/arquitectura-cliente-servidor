#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

/*
Para manejar senales, hay que usar la funcion signal():
man signal
Este programa muestra el caso 2: trata la senal y continua el proceso.
En este caso, aunque se trata la senal, el proceso continua su ejecucion
y como esta en un ciclo infinito, debes matar el proceso con:
kill -9 pid
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
    printf("contador = %d\n", cnt++);
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
    printf("En espera de Control-C para cachar SIGINT pid=%d\n", getpid());
    sleep( 5 );
  }

  return 0;
}
