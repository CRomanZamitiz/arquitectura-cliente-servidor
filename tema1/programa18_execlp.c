#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Este ejemplo muestra la funcion execlp con el comando ls -l /
int main(void)
{
  // como es execlp entonces es una lista
  // la sintaxis es:
  // int execlp (char *file, char *arg0,... char *argn, (char *)0);
  // file es el nombre del comando/ejecutable sin ruta absoluta
  // porque toma el directorio de la variable PATH, de ahi la "p" de execlp
  // En C el nombre del comando/ejecutable es el argumento 0
  // por eso se repite
  // con NULL se indica que se llego al final de la lista de argumentos
  // OBSERVEN QUE execlp() NO NECESITA LA RUTA ABSOLUTA DEL COMANDO O EJECUTABLE
  // SOLO PONEMOS "ls" NO ES NECESARIO PONER "/bin/ls"
  if( execlp("ls", "ls", "-l", "/", NULL) == -1)
  {
    printf("execlp genero error y devolvio -1\n");
    perror("execlp");
    exit(EXIT_FAILURE);
  }
  printf("Si execlp es exitoso, esto no se imprime\n");
  exit(EXIT_SUCCESS);
}
