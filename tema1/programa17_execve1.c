#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Este ejemplo muestra la funcion execve
// invocando a un programa nuestro getenv_user
// para leer la variable de entorno USER
int main(void)
{
  printf("En el proceso inicial pid=%d, antes de llamar a execve\n", getpid() );
  // como es execve entonces es un vector/arreglo 
  // y ademas puede leer variables de entorno que vienen en un arreglo 
  // y que solo son validas para la ejecucion del ejecutable dentro de la funcion execve
  // la sintaxis es:
  // int execve(char *path, char *argv[ ], char *envp[ ]);
  // path es la ruta incluyendo el nombre del comando/ejecutable
  // En C el nombre del comando/ejecutable es el argumento 0
  // por eso se repite
  // con NULL se indica que se llego al final del vector/arreglo
  char * args[]={"./programa13_getenv_user2", NULL};
  char *variables_entorno[] = { "PATH=/bin:/usr/bin", "USER=jose", "MI_VARIABLE=acs", NULL };
  printf( "el valor de la variable de entorno USER es: %s\n", getenv("USER") );
  printf( "el valor de la variable de entorno MI_VARIABLE es: %s\n", getenv("MI_VARIABLE") );
  printf( "ahora ejecutando ./programa13_getenv_user2, los valores de las variables de entorno USER y MI_VARIABLE van a cambiar solo para esta ejecucion\n" );
  if( execve("./programa13_getenv_user2", args, variables_entorno) == -1)
  {
    printf("execve genero error y devolvio -1\n");
    perror("execve");
    exit(EXIT_FAILURE);
  }
  // Por lo tanto, sea exito o error, las sig 2 sentencias
  // JAMAS se ejecutan
  printf("Si execve es exito o error, esto no se imprime\n");
  exit(EXIT_SUCCESS);
}
