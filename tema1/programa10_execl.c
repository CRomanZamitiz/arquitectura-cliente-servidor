#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Este ejemplo muestra la funcion execl con el comando ls -l /
int main(void)
{
  printf("En el proceso inicial, antes de llamar a execl\n");
  // como es execl entonces es una lista
  // la sintaxis es:
  // int execl (char *path, char *arg0,... char *argn, (char *)0);
  // path es la ruta incluyendo el nombre del comando/ejecutable
  // En C el nombre del comando/ejecutable es el argumento 0
  // por eso se repite
  // con NULL se indica que se llego al final de la lista de argumentos
  // Si execl falla devuelve -1 y entra al if por lo que termina
  // con exit(EXIT_FAILURE)
  if( execl("/bin/lsa", "/bin/lsa", "-l", "/", NULL) == -1)
  {
    printf("execl genero error y devolvio -1\n");
    perror("execl");
    exit(EXIT_FAILURE);
  }
  // Por lo tanto, sea exito o error, las sig 2 sentencias
  // JAMAS se ejecutan
  printf("Si execl es exito o error, esto no se imprime\n");
  exit(EXIT_SUCCESS);
}
