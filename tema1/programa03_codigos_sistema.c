/* Manejo de codigos de sistema */

#include <stdio.h>
#include <string.h>

int main()
{
  int i;

  for(i=0; i <= 150; i++)
  {
    printf( "%d: %s\n", i , strerror( i ) );
  }
  return 0;
}
