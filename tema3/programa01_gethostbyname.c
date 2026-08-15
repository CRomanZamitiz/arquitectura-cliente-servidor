/*
Esta funcion se utiliza para la resolucion de nombres (DNS forward).
Toma el nombre de un dominio (ej. "google.com") y devuelve 
una estructura hostent que contiene, entre otras cosas, la direccion IP asociada.
*/

#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

int main() {
    const char *hostname = "www.google.com";
    struct hostent *he;
    struct in_addr **addr_list;

    he = gethostbyname(hostname);
    if (he == NULL) {
        herror("gethostbyname"); // Funcion especifica para errores de host
        return 1;
    }

    printf("Nombre oficial: %s\n", he->h_name);
    
    // El campo h_addr_list es un array de punteros a direcciones de red
    addr_list = (struct in_addr **)he->h_addr_list;
    
    for(int i = 0; addr_list[i] != NULL; i++) {
        printf("IP %d: %s\n", i + 1, inet_ntoa(*addr_list[i]));
    }

    return 0;
}
