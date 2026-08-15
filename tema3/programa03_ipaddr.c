#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[ ])
{
  struct hostent *h;

  /* error check the command line */
  if(argc != 2)
  {
   fprintf(stderr, "Usage: %s <domain_name>\n", argv[0]);
   exit(1);
  }

  /* get the host info */
  if((h=gethostbyname(argv[1])) == NULL)
  {
   herror("gethostbyname(): ");
   exit(1);
  }
  else
   printf("gethostbyname() is OK.\n");

  printf("The host name is: %s\n", h->h_name);
  printf("El valor h_addrtype es: %d\n", h->h_addrtype);
  printf("El valor AF_INET es: %d\n", AF_INET);
  printf("El valor AF_INET6 es: %d\n", AF_INET6);
  printf("4 maneras de obtener IPv4:\n");
  printf("a) con inet_ntoa(*((struct in_addr *)h->h_addr))\n");
  printf("La IP principal es: %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

  printf("b) con inet_ntop(...)\n");
  char ip_formateada[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, h->h_addr, ip_formateada, INET_ADDRSTRLEN);
  printf("La IP principal es: %s\n", ip_formateada);

  printf("c) con (unsigned char *)h->h_addr\n");  
  // Obtenemos el apuntador a los 4 bytes de la IP
  unsigned char *b = (unsigned char *)h->h_addr;
  // Imprimimos cada byte en decimal y hexadecimal
  printf("b) Formato maquina (4 bytes separados):\n");
  for(int i = 0; i < 4; i++) {
    printf("Byte %d: %3d (Hex: 0x%02x)\n", i, b[i], b[i]);
  }

  printf("d) con el valor de 32 bits completo (unsigned int *)h->h_addr\n");  
  // Imprimimos el valor de 32 bits completo (como un solo entero)
  unsigned int *ip_entero = (unsigned int *)h->h_addr;
  printf("Valor de 32 bits completo (Raw): %u\n", *ip_entero);

  printf("-------------------------------------\n");
  printf("The address length is: %d\n", h->h_length);

  printf("Sniffing other names...sniff...sniff...sniff...\n");
  int j = 0;
  while(h->h_aliases[j] != NULL)
  {
   printf("An alias #%d is: %s\n", j, h->h_aliases[j]);
   j++;
  }

  printf("Sniffing other IPs...sniff....sniff...sniff...\n");
  int i = 0;
  while(h->h_addr_list[i] != NULL)
  {
   printf("Address #%i is: %s\n", i, inet_ntoa(*((struct in_addr *)(h->h_addr_list[i]))));
   i++;
  }

  return 0;
}
