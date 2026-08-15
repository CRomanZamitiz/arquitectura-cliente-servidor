/*
Esta funcion realiza el proceso inverso: 
resolucion inversa de DNS (reverse DNS).
Toma una direccion IP y busca el nombre de host 
(dominio) asociado a ella.
*/

#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

int main() {
    struct in_addr addr;
    struct hostent *he;
    const char *ip_str = "8.8.8.8"; // DNS de Google

    // Convertimos la cadena IP a formato binario de red
    if (inet_aton(ip_str, &addr) == 0) {
        fprintf(stderr, "IP no valida\n");
        return 1;
    }

    // Buscamos el nombre del host
    // El segundo argumento es el tamano de la direccion, el tercero el tipo (AF_INET)
    he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    
    if (he == NULL) {
        herror("gethostbyaddr");
        return 1;
    }

    printf("El nombre del host para la IP %s es: %s\n", ip_str, he->h_name);

    return 0;
}
