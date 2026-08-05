#include <stdio.h>
#include <stdlib.h>

int main ()
{
  int return_value;
  printf("Ejemplo de funcion system(\"ls -l /\") con un comando que se ejecuta correctamente\n");
  return_value = system ("ls -l /");
  printf("Termina proceso hijo, return_value=%d\n", return_value);
  return return_value;
}
