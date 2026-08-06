#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int spawn (char* program, char** arg_list)
{
  pid_t child_pid;
  /* Duplicate this process. */
  child_pid = fork ();
  if (child_pid != 0) /* This is the parent process. */
    return child_pid;
  else {
    /* Now execute PROGRAM, searching for it in the path. */
    printf("soy el hijo, lanzo el ls y termino\n");
    execvp (program, arg_list);
    /* The execvp function returns only if an error occurs. */
    printf ("un error ocurrio en execvp\n");
    return 1;;
  }
}

int main ()
{
  int child_status=1;
  /* The argument list to pass to the "ls" command. */
  char* arg_list[] = {
    "ls", /* argv[0], the name of the program. */
    "-l",
    "/",
    NULL /* The argument list must end with a NULL. */
  };

  /* Spawn a child process running the "ls" command. Ignore the
   * 	returned child process ID. */
  spawn ("ls", arg_list);
  printf("soy el padre, voy a esperar a que el hijo termine\n");
  /* Wait for the child process to complete. */
  wait (&child_status);
  if (WIFEXITED (child_status)){
    printf ("El proceso hijo termio normalmente, con codigo de salida %d\n", WEXITSTATUS (child_status));
  }
  else
    printf ("El proceso hijo termino anormalmente\n");
  return 0;
}

