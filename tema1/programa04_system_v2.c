#include <stdio.h>
#include <stdlib.h>

int main ()
{
 int return_value;
 printf("Ejemplo 2 de funcion system(\"ls -l /\") con un comando que se ejecuta correctamente\n");
 return_value = system ("ls -l /");
 printf("Termina proceso hijo, return_value=%d\n", return_value);
 printf("Lo siguiente tiene que ver con la funcion wait(), lo veremos mas adelante\n");
 printf("En el proceso inicial, WIFEXITED(return_value)=%d\n",WIFEXITED(return_value) );
 printf("En el proceso inicial, WEXITSTATUS(return_value) codigo de salida del hijo=%d\n",WEXITSTATUS(return_value) );
 printf("---------------------------------------\n");
 printf("Ejemplo 2 de funcion system(\"ls -l no_existe\") con un comando que se ejecuta incorrectamente\n");
 return_value = system ("ls -l no_existe");
 printf("Termina proceso hijo, return_value=%d\n", return_value);
 printf("Lo siguiente tiene que ver con la funcion wait(), lo veremos mas adelante\n");
 printf("En el proceso inicial, WIFEXITED(return_value)=%d\n",WIFEXITED(return_value) );
 printf("En el proceso inicial, WEXITSTATUS(return_value) codigo de salida del hijo=%d\n",WEXITSTATUS(return_value) );
 return return_value;
}
