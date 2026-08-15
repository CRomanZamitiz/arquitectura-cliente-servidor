#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <netdb.h>

#define PUERTO_SERVIDOR 80

void medir_tiempo(char *dominio, int buffer_size) {
    int socket_fd;
    struct sockaddr_in servidor;
    struct hostent* hostinfo;

    char *buffer = malloc(buffer_size);  // Crear buffer dinamico

    if (!buffer) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }

    // Crear socket
    // printf(" Paso 1. Funcion socket(AF_INET, SOCK_STREAM, 0)\n");
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Error al crear el socket");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // Configurar direccion del servidor
    // printf(" Paso 2. Inicializar la estructura sockaddr_in\n");
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO_SERVIDOR);
    hostinfo = gethostbyname (dominio);
    if (hostinfo == NULL)
    {
        perror("Error al obtener hostent");
        free(buffer);
        exit(EXIT_FAILURE);
    }
    else
        servidor.sin_addr = *((struct in_addr *) hostinfo->h_addr);

    // Conectar al servidor
    // printf(" Paso 3. Funcion connect(socket_fd, (struct sockaddr*)&servidor, sizeof(servidor))\n");
    if (connect(socket_fd, (struct sockaddr*)&servidor, sizeof(servidor)) == -1) {
        perror("Error en connect");
        close(socket_fd);
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // Enviar peticion HTTP
    // printf("Paso 4. Funcion write(socket_fd, request, strlen(request))\n"); 
    char request[] = "GET / HTTP/1.0\n\n";
    write(socket_fd, request, strlen(request));

    // Medir el tiempo de recepcion
    // printf("Paso 5. Medir el tiempo de reloj en este punto, antes de empezar a leer la respuesta\n");
    clock_t inicio = clock();

    // printf("Paso 6. Se lee con read(socket_fd, buffer, buffer_size)) en un while para leer hasta que lo que se lea sea 0 bytes\n");
    // Esta variable lee los bytes del ciclo actual
    ssize_t bytes_leidos;
    // Esta variable va acumulando la suma de los bytes leidos
    ssize_t total_bytes_leidos = 0;
    int num_ciclo = 0;
    while ((bytes_leidos = read(socket_fd, buffer, buffer_size)) > 0)
    {
        // Aqui se suman los bytes leidos a lo acumulado
	printf("ciclo %d , bytes leidos %ld\n", ++num_ciclo, bytes_leidos);
        total_bytes_leidos += bytes_leidos;
    }

    // printf("Paso 7. Medir el tiempo de reloj en este punto, despues de terminar de leer la respuesta\n");
    clock_t fin = clock();
    double tiempo_total = (double)(fin - inicio) / CLOCKS_PER_SEC;

    printf("Buffer: %d bytes , Tiempo: %.4f segundos , Total recibido: %ld bytes\n",
           buffer_size, tiempo_total, total_bytes_leidos);

    close(socket_fd);
    free(buffer);
}

// Empecemos por la funcion main()
// Aqui solo se llama a la funcion medir tiempo pasando los valores
// que tendrán los buffers
int main(int argc, char* const argv[]) {
    printf("=== PRUEBA DE TIEMPOS CON DIFERENTES TAMANOS DE BUFFERS ===\n");

    medir_tiempo(argv[1], 1024);   // 1 KB
    medir_tiempo(argv[1], 4096);   // 4 KB
    medir_tiempo(argv[1], 8192);   // 8 KB
    medir_tiempo(argv[1], 16384);  // 16 KB
    medir_tiempo(argv[1], 100000);  // 100000 B

    return 0;
}
