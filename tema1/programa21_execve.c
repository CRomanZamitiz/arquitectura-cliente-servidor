


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  char *newargv[] = { NULL, "hola", "mundo", "buen", "dia",  NULL };

  if( argc != 2 )
  {
    fprintf( stderr, "La sintaxis es: %s <archivo-a-ejecutar>\n", argv[0] );
    exit( EXIT_FAILURE );
  }

  // ESTA SENTENCIA ES MUY IMPORTANTE!
  // QUE HACE ESTA SENTENCIA? 
  newargv[0] = argv[1];

  execve( argv[1], newargv, NULL );
  
  // AUNQUE LA SIG SENTENCIA DICE QUE NO SE EJECUTA,
  // ASI COMO ESTA EL CODIGO, SI PODRIA EJECUTARSE, SI EL execve FALLA
  // MODIFICA ESTE BLOQUE PARA QUE AUNQUE FALLE LA LLAMADA A execve
  // LA SIGUIENTE SENTENCIA NO SE EJECUTE
  printf("Esta sentencia no se ejecuta\n");
  perror("execve");
  exit( EXIT_FAILURE );
  
}
