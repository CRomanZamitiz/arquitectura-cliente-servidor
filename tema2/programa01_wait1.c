#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
  int retorno_fork;
  int arg_wait;
  int pid = getpid();
  printf( "Iniciando proceso con pid %d\n", pid );
  printf( "Llamando a fork\n" );
  retorno_fork = fork();
  if( retorno_fork != 0 )
  {
    printf( "Proceso padre, la variable retorno_fork=%d\n",retorno_fork );
    printf( "Proceso padre, llamando a wait\n" );
    int retorno_wait =  wait( &arg_wait );
    printf( "Proceso padre, despues de wait\n" );
    printf( "Proceso padre, el valor &arg_wait=%p\n",&arg_wait );
    printf( "Proceso padre, la variable arg_wait=%d\n",arg_wait );
    printf( "Proceso padre, WIFEXITED(arg_wait)=%d\n",WIFEXITED(arg_wait) );
    printf( "Proceso padre, WEXITSTATUS(arg_wait) codigo de salida del hijo=%d\n",WEXITSTATUS(arg_wait) );
    printf( "Proceso padre, la variable retorno_wait=%d\n",retorno_wait );
    printf( "EXIT_SUCCESS en el proceso padre=%d\n", EXIT_SUCCESS );
    exit( EXIT_SUCCESS );
  }
  else
  {
    printf( "Proceso hijo, el pid es %d\n", getpid() );
    printf( "Proceso hijo, a dormir 10 seg\n" );
    sleep( 10 );
    printf( "Proceso hijo, despierta\n" );
    printf( "proceso hijo=%d\n", 2 );
    exit( 2 );
  }
}
