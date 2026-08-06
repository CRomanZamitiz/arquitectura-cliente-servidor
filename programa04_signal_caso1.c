#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

/*
Para manejar senales, hay que usar la funcion signal():
man signal
Para observar que inicialmente la senal SIGINT si interrumpe el proceso
debes modificar la sentencia de
if( signal(SIGINT, SIG_IGN) == SIG_ERR )
a
if( signal(SIGINT, SIG_DFL) == SIG_ERR )
para que haga el comportamiento DEFAULT
y puedes interrumpir el proceso con la combinacion Control C

Asi como esta este codigo, muestra el caso 1: ignorar la senal.
Como Control-C sera ignorado y el proceso esta en un ciclo infinito,
deberas matar el proceso con:
kill -9 pid
*/

/*
 * main: Inicializa el manejador de la senal SIGINT y
 * se pone en espera para recibir la senal
 */
int main()
{
  if( signal(SIGINT, SIG_IGN) == SIG_ERR )
  {
    printf("no puedo tratar SIGINT\n");
    perror("signal");
  }

  while( 1 )
  {
    printf("En espera de Control-C para ignorar SIGINT pid=%d\n", getpid());
    sleep( 5 );
  }

  return 0;
}
