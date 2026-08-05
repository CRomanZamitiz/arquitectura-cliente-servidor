#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int spawn (char* program, char** arg_list)
{
 pid_t child_pid;
 child_pid = fork ();
 if (child_pid != 0)
  return child_pid;
 else
 {
  // RECUERDEN: Si execvp() se ejecuta exitosamente
  // el proceso "parent" termina en ese momento,
  // pero termina con exito
  execvp (program, arg_list);
  printf ("ocurrio un error al ejecutar execvp(...)\n");
  // alternativa al printf:
  // fprintf (stderr, "ocurrio un error al ejecutar execvp(...)\n");
  return 1; // termina con error
 }
}

int main ()
{
  char* arg_list[] = {
   "ls",
   "-l",
   "/",
   NULL
  };

  spawn ("ls", arg_list);
  printf ("Termina el proceso padre\n");
  return 0; // Termina con exito
}
