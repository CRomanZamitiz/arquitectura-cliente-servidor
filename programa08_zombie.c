#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main ()
{
  pid_t child_pid;

  /* Crea un proceso hijo */
  child_pid = fork ();

  if (child_pid > 0)
  {
    /* Este es el proceso padre. Se duerme por 60 seg (un minuto) */
    sleep (60);
  }
  else
  {
    /* Este es el proceso hijo. Termina inmediatamente
    El hijo envia la senal al padre pero esta dormido
    por lo que durante 1 min el hijo sera zombie */
    exit (0);
  }
  return 0;
}
