#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>

// La sentencia wait(NULL) en sistemas Unix/Linux se utiliza para que un proceso padre espere a que cualquiera de sus hijos termine.
// Es importante tener en cuenta que la funcion wait(NULL) no distingue entre procesos hijos que han terminado con exito y aquellos que han terminado con un error.
void func(int signum)
{
  wait(NULL);
}

int main ()
{
  pid_t child_pid;

  /* Crea un proceso hijo */
  child_pid = fork ();

  if (child_pid > 0)
  {
    printf( "Antes de llamar a la funcion signal(SIGCHLD, func)\n" );
    signal(SIGCHLD, func);
    printf( "Despues de llamar a la funcion signal(SIGCHLD, func)\n" );
    printf( "Este es el proceso padre. Se duerme por 60 seg (un minuto)\n" );
    /* Este es el proceso padre. Se duerme por 60 seg (un minuto) */
    sleep (60);
    printf( "El proceso padre es interrumpido de su sueno. Termina inmediatamente\n" );
  }
  else
  {
    printf( "Este es el proceso hijo. Voy a contar 5 seg y termino inmediatamente\n" );
    int cont = 0;
    while( cont < 5 )
    {
      printf("%d seg\n", ++cont);
      sleep(1);
    }
    /* Este es el proceso hijo. Termina inmediatamente */
    exit (0);
  }
  return 0;
}
