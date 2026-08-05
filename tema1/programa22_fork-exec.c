#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int spawn (char* program, char** arg_list)
{
 pid_t child_pid;
 printf ("Voy a llamar a fork() para crear un hijo\n");
 child_pid = fork ();
 printf ("Este printf sale duplicado, fork() me devolvio: %d\n", child_pid);
 if (child_pid != 0)
  return child_pid;
 else
 {
  printf( "Soy el proceso hijo, PID=%d y PPID=%d\n", getpid(), getppid() );
  // RECUERDEN: Si execvp() se ejecuta exitosamente
  // el proceso "invocador" termina en ese momento,
  // pero termina con exito
  printf ("Hasta aqui llego! Voy a llamar a exec(...) para ejecutar programa01_print-pid\n");
  execvp (program, arg_list);
  printf ("ocurrio un error al ejecutar execvp(...)\n");
  return 1; // termina con error
 }
}

int main ()
{
  char* arg_list[] = { "./programa01_print-pid", NULL };

  printf( "Soy el proceso padre, PID=%d y PPID=%d\n", getpid(), getppid() );
  printf( "Voy a la funcion spawn(...)\n" );
  spawn ("./programa01_print-pid", arg_list);
  printf ("Termina el proceso padre\n");
  return 0; // Termina con exito
}
