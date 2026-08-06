#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>

int main ()
{
  pid_t child_pid;

  /* Crea un proceso hijo */
  child_pid = fork ();

  if (child_pid > 0)
  {
    printf( "Antes de llamar a la funcion signal(SIGCHLD, SIG_IGN)\n" );
    signal(SIGCHLD, SIG_IGN);
    printf( "Despues de llamar a la funcion signal(SIGCHLD, SIG_IGN)\n" );
    printf( "Este es el proceso padre. Se duerme por 60 seg (un minuto)\n" );
    /* Este es el proceso padre. Se duerme por 60 seg (un minuto) */
    sleep (60);
    printf( "El proceso padre despierta. Termina inmediatamente\n" );
  }
  else
  {
    printf( "Este es el proceso hijo. Termina inmediatamente\n" );
    /* Este es el proceso hijo. Termina inmediatamente */
    exit (0);
  }
  return 0;
}
