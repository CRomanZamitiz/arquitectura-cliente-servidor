#include <stdio.h>
#include <errno.h>

// Esta version es parecida a programa06_error
// la diferencia es que esta version 
// llama a la funcion perror( string )
// observen la linea de perror( "El error es" )
// y que imprime en pantalla
int main()
{
  int result;

  result = rename("antes.txt","despues.txt");
  if( result != 0 )
  {
    printf("errno:%d\n", errno);
    perror( "El error es" );
    return 1;
  }
  printf("Archivo renombrado");
  return 0;
}
