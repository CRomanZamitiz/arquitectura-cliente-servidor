#include<stdio.h>
#include<unistd.h>

int main()
{
  printf("El process id es %d\n", getpid());
  printf("El parent process id es %d\n", getppid());
  return 0;
}
