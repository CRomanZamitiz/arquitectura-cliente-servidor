#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
 int contador = 0;
 int return_value;
 while( contador < 5 )
 {
   printf("Antes de system(...) recursivo\n");
   sleep( 3 );
   printf("contador=%d\n", contador);
   contador++;
   return_value = system ("./programa03_system_v3");
   printf("Termina proceso hijo, return_value=%d\n", return_value);
   printf("En el proceso inicial, WIFEXITED(return_value)=%d\n",WIFEXITED(return_value) );
   printf("En el proceso inicial, WEXITSTATUS(return_value) codigo de salida del hijo=%d\n",WEXITSTATUS(return_value) );
 }
 return return_value;
}
