#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int sig)
{
  printf("El codigo de senal recibida es %d\n", sig);
  switch (sig)
  {
    // 1
    case SIGHUP:
      printf("Se recibio la senal 1 SIGHUP\n");
      break;

    // 2
    case SIGINT:
      printf("Se recibio la senal 2 SIGINT (Ctrl+C)\n");
      break;

    // 3
    case SIGQUIT:
      printf("Se recibio la senal 3 SIGQUIT\n");
      break;

    // 4
    case SIGILL:
      printf("Se recibio la senal 4 SIGILL\n");
      break;

    // 5
    case SIGTRAP:
      printf("Se recibio la senal 5 SIGTRAP\n");
      break;

    // 6
    case SIGABRT:
      printf("Se recibio la senal 6 SIGABRT\n");
      break;

    // 7
    case SIGBUS:
      printf("Se recibio la senal 7 SIGBUS\n");
      break;

    // 8
    case SIGFPE:
      printf("Se recibio la senal 8 SIGFPE\n");
      break;

    // 9
    case SIGKILL:
      printf("Se recibio la senal 9 SIGKILL\n");
      break;

    // 10
    case SIGUSR1:
      printf("Se recibio la senal 10 SIGUSR1\n");
      break;

    // 15
    case SIGTERM:
      printf("Se recibio la senal 15 SIGTERM\n");
      break;

    // 17
    case SIGCHLD:
      printf("Se recibio la senal 17 SIGCHLD\n");
      break;

    // 20
    case SIGTSTP:
      printf("Se recibio la senal 20 SIGTSTP\n");
      break;

    default:
      printf("Se recibio una senal que no esta en los case, entro al default\n");
      break;
  }
}

int main()
{
  printf("El PID de este proceso es: %d\n", getpid());

  // Registrar los manejadores de senales
  // Puede ser como esta en las sig lineas comentadas
  // o comparando contra SIG_ERR
  // signal(SIGINT, signal_handler);
  // signal(SIGTERM, signal_handler);
  // signal(SIGUSR1, signal_handler);

  // 1
  if( signal(SIGHUP, signal_handler) == SIG_ERR )
    printf("No puedo manejar SIGHUP\n");
  else
    printf("Si puedo manejar SIGHUP\n");

  // 2
  if( signal(SIGINT, signal_handler) == SIG_ERR )
    printf("No puedo manejar SIGINT\n");
  else
    printf("Si puedo manejar SIGINT\n");

  // 3
  if( signal(SIGQUIT, signal_handler) == SIG_ERR )
    printf("No puedo manejar SIGQUIT\n");
  else
    printf("Si puedo manejar SIGQUIT\n");

  // 4
  if( signal(SIGILL, signal_handler) == SIG_ERR )
    printf("No puedo manejar SIGILL\n");
  else
    printf("Si puedo manejar SIGILL\n");

  // 5
  if( signal(SIGTRAP, signal_handler) == SIG_ERR )
    printf("No puedo manejar SIGTRAP\n");
  else
    printf("Si puedo manejar SIGTRAP\n");

  // 6
  if( signal(SIGABRT, signal_handler) == SIG_ERR )
    printf("No puedo manejar SIGABRT\n");
  else
    printf("Si puedo manejar SIGABRT\n");

  // 7
  if( signal(SIGBUS, signal_handler) == SIG_ERR )
    printf("No puedo manejar SIGBUS\n");
  else
    printf("Si puedo manejar SIGBUS\n");

  // 8
  if( signal(SIGFPE, signal_handler) == SIG_ERR )
    printf("No puedo manejar SIGFPE\n");
  else
    printf("Si puedo manejar SIGFPE\n");

  // 9
  if( signal(SIGKILL, signal_handler) == SIG_ERR )
    printf("No puedo manejar SIGKILL\n");
  else
    printf("Si puedo manejar SIGKILL\n");

  // 10
  if( signal(SIGUSR1, signal_handler) == SIG_ERR )
    printf("No puedo manejar SIGUSR1\n");
  else
    printf("Si puedo manejar SIGUSR1\n");

  // 15
  if( signal(SIGTERM, signal_handler) == SIG_ERR )
    printf("No puedo manejar SIGTERM\n");
  else
    printf("Si puedo manejar SIGTERM\n");

  // 17
  if( signal(SIGCHLD, signal_handler) == SIG_ERR )
    printf("No puedo manejar SIGCHLD\n");
  else
    printf("Si puedo manejar SIGCHLD\n");

  // 20
  if( signal(SIGTSTP, signal_handler) == SIG_ERR )
    printf("No puedo manejar SIGTSTP\n");
  else
    printf("Si puedo manejar SIGTSTP\n");

  // Esperar por las senales
  while (1)
  {
    sleep(1);
  }

  return 0;
}

