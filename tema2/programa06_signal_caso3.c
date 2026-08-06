#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

/*
Para manejar senales, hay que usar la funcion signal():
man signal
Este programa muestra el caso 3: la senal no puede ser ignorada ni tratada
y se termina el proceso. Un ejemplo de las senales de este caso es SIGKILL.
En este caso, aunque el proceso tambien esta en un ciclo infinito,
el proceso muere solo porque en nuestro tratamiento de la senal SIGINT,
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
    if( cnt < 2 )
    {
      printf("contador = %d\n", cnt++);
    }
    else
    {
      exit( 0 );
    }
  }
  else if( sig == SIGKILL )
    printf("senal SIGKILL %d recibida\n", sig);
  else if( sig == SIGSTOP )
    printf("senal SIGSTOP %d recibida\n", sig);
  else if( sig == SIGTSTP )
    printf("senal SIGTSTP %d recibida\n", sig);
  else
    printf("senal %d recibida\n", sig);
}

/*
 * main: Inicializa el manejador de la senal SIGINT y
 * se pone en espera para recibir la senal
 */
int main()
{
  if( signal(SIGUSR1, manejador_senales) == SIG_ERR )
  {
    printf("no puedo cachar SIGUSR1\n");
    perror("signal");
  }
  if( signal(SIGINT, manejador_senales) == SIG_ERR )
  {
    printf("no puedo cachar SIGINT\n");
    perror("signal");
  }
  if( signal(SIGKILL, manejador_senales) == SIG_ERR )
  {
    printf("no puedo cachar SIGKILL\n");
    perror("signal");
  }
  if( signal(SIGSTOP, manejador_senales) == SIG_ERR )
  {
    printf("no puedo cachar SIGSTOP\n");
    perror("signal");
  }
  if( signal(SIGTSTP, manejador_senales) == SIG_ERR )
  {
    printf("no puedo cachar SIGTSTP\n");
    perror("signal");
  }
  while( 1 )
  {
    printf("En espera de Control-C para cachar SIGINT\n");
    sleep( 5 );
  }

  return 0;
}
