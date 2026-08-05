#include <stdio.h>
#include <unistd.h>

int main ()
{
    pid_t id_hijo;
    printf( "1: el id del proceso main es %d\n", getpid() );
    id_hijo = fork ();
    printf( "2: Este printf saldra duplicado\n");
    printf( "3: Este printf tambien sale por duplicado. La variable id_hijo es %d\n", id_hijo );
    if( id_hijo != 0 )
    {
        printf( "\nPADRE:proceso main o padre con id %d\n", getpid());
        printf( "PADRE:variable id_hijo es %d\n", id_hijo );
        printf( "PADRE:id del padre del padre %d\n",getppid());
    }
    else
    {
        printf( "\nHIJO:proceso hijo con id %d\n", getpid() );
        printf( "HIJO:variable id_hijo es %d\n", id_hijo );
        printf( "HIJO:id del padre es %d\n", getppid() );
    }
    return 0;
}
