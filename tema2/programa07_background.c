#include<stdio.h>
#include<unistd.h>

int main()
{
  int contador = 0;
  printf( "1: el id del proceso es %d\n", getpid() );
  while( contador < 20 )
  {
    printf( "2: me voy a dormir 10 seg\n" );
    sleep( 10 );
    printf( "3: ya pasaron 10 seg, me despierto\n" );
    contador++;
  }
  printf( "4: ya genere muchos printf, ya voy a terminar, adios\n" );
  return 0;
}

