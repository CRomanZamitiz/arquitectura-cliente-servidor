#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd;
    struct sockaddr_in servidor;
    struct hostent *he;
    const char *hostname = "www.google.com";
    int puerto = 80;

    // 1. Obtener la IP del host (Resolucion DNS)
    he = gethostbyname(hostname);
    if (he == NULL) {
        herror("gethostbyname");
        return 1;
    }

    // 2. Crea el socket
    // AF_INET: IPv4 | SOCK_STREAM: TCP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("No se pudo crear el socket");
        return 1;
    }

    // 3. Configurar la estructura del servidor
    servidor.sin_family = AF_INET;
    // IMPORTANTE Usamos htons para que el puerto viaje en Network Byte Order
    // Si intentas enviar un numero de puerto (por ejemplo, el 80) 
    // a traves de un socket sin usar htons() (Host to Network Short),
    // tu maquina enviara los bytes invertidos.
    // El puerto 80 en hexadecimal es 0x0050.
    // Tu CPU lo guarda como 50 00.
    // El servidor remoto recibira 0x5000, que es el puerto 20480.
    // Asi jamas te conectaras!
    servidor.sin_port = htons(puerto); 
    // Copiamos los bytes crudos de la IP que obtuvimos de gethostbyname
    memcpy(&servidor.sin_addr, he->h_addr_list[0], he->h_length);

    // 4. Conectarse
    printf("Intentando conectar a %s (%s) en el puerto %d...\n", hostname, inet_ntoa(servidor.sin_addr), puerto);

    if (connect(sockfd, (struct sockaddr *)&servidor, sizeof(servidor)) < 0) {
        perror("Error de conexión");
        return 1;
    }

    printf("¡Conexión establecida con éxito!\n");

    // 5. Cerrar el socket
    close(sockfd);
    return 0;
}
