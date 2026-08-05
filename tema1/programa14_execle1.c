#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Este ejemplo muestra la funcion execle con el comando ls -l /
int main(void)
{
  printf("En el proceso inicial, antes de llamar a execle\n");
  // como es execle entonces es una lista 
  // y ademas puede leer variables de entorno que vienen en un arreglo 
  // y que solo son validas para la ejecucion del archivo dentro de la funcion execle
  // la sintaxis es:
  // int execle(char *path, char *arg0,... char*argn, (char *)0, char *envp[ ]);
  // path es la ruta incluyendo el nombre del comando/ejecutable
  // En C el nombre del comando/ejecutable es el argumento 0
  // por eso se repite
  // con NULL se indica que se llego al final de la lista de argumentos
  char *variables_entorno[] = { "PATH=/bin:/usr/bin", "USER=jose", NULL };
  printf( "el valor de la variable de entorno USER es: %s\n", getenv("USER") );
  if( execle("/bin/ls", "/bin/ls", "-l", "/", NULL, variables_entorno) == -1)
  {
    printf("execle genero error y devolvio -1\n");
    perror("execle");
    exit(EXIT_FAILURE);
  }
  // Por lo tanto, sea exito o error, las sig 2 sentencias
  // JAMAS se ejecutan
  printf("Si execle es exito o error, esto no se imprime\n");
  exit(EXIT_SUCCESS);
}
