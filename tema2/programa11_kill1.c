#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

int main()
{
 pid_t pid;
 if (( pid = fork()) == 0) {
   while(1) {
     printf("HIJO getpid()=%d pid = %d\n", getpid(), pid);
     sleep(1);
   }
 }
 sleep(10);
 printf("PADRE Terminacion proceso %d\n", pid);
 kill (pid,SIGKILL);
 exit(0);
}

