#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Este ejemplo muestra la funcion execvp con el comando ls -l /
int main(void)
{
  // como es execvp entonces es un vector/arreglo
  // el cual debe declararse previo a la llamada a execvp
  // la sintaxis es:
  // int execvp (char *file, char *argv[ ]);
  // file es el nombre del comando/ejecutable, sin ruta
  // ya que la ruta la toma de la variable PATH, de ahi la "p" en el nombre
  // En C el nombre del comando/ejecutable es el argumento 0
  // por eso se repite
  // con NULL se indica que se llego al final del vector de argumentos
  // OBSERVEN QUE execvp() NO NECESITA LA RUTA ABSOLUTA DEL COMANDO O EJECUTABLE
  // SOLO PONEMOS "ls" NO ES NECESARIO PONER "/bin/ls"
  char *args[]={"ls","-l","/",NULL};
  if( execvp("ls", args) == -1)
  {
    printf("execvp genero error y devolvio -1\n");
    perror("execvp");
    exit(EXIT_FAILURE);
  }
  printf("Si execvp es exitoso, esto no se imprime\n");
  exit(EXIT_SUCCESS);
}
