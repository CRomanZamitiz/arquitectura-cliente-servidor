#include <stdio.h>
#include <errno.h>
#include <string.h>

int main()
{
  int result;

  result = rename("antes.txt","despues.txt");
  if( result != 0 )
  {
    printf("Error al renombrar el archivo\n");
    printf("errno:%d\n", errno);
    printf( "strerror(errno):%s\n", strerror( errno ) );
    perror( "el mensaje de error es" );
    switch(errno)
    {
     case EPERM:
                printf("Operation not permitted\n");
                break;
     case ENOENT:
                printf("File not found\n");
                break;
     case EACCES:
                printf("Permission denied\n");
                break;
     case ENAMETOOLONG:
                printf("Filename is too long\n");
                break;
     default:
                printf("Otro error\n");
    }
    return 1;
  }
  printf("Archivo renombrado\n");
  return 0;
}
