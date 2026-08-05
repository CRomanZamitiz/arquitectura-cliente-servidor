#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
  printf( "Entrando al programa llamado por la familia exec, mi pid es %d\n", getpid() );
  printf( "el valor de la variable de entorno USER es: %s\n", getenv("USER") );
  return 0;
}
