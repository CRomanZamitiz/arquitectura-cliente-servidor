#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      // Para operaciones de cierre de archivos/sockets
#include <arpa/inet.h>   // Para las funciones y estructuras de sockets

#define BUFFER_SIZE 1024 // Tamano del bloque de memoria para recibir el contenido

int main(int argc, char *argv[]) {
    // Verificamos que el usuario pase el puerto como argumento
    if (argc != 2) {
        printf("Uso correcto: %s <puerto>\n", argv[0]);
        return 1;
    }

    int puerto = atoi(argv[1]);
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    // 1. Crear el socket AF_INET (IPv4) y SOCK_STREAM (TCP)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error al crear el socket del servidor");
        return 1;
    }

    // Configurar los datos del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(puerto);
    server_addr.sin_addr.s_addr = INADDR_ANY; // Escuchar en cualquier interfaz de red

    // 2. Unir (Bind) el socket al puerto especificado
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al hacer bind. Quizas el puerto este ocupado");
        return 1;
    }

    // 3. Escuchar conexiones entrantes
    listen(server_fd, 1);
    printf("Servidor FTP iniciado y escuchando en el puerto %d...\n", puerto);

    // 4. Aceptar la conexion del cliente
    // Observen que accept esta antes del while(1)
    addr_size = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_size);
    printf("Cliente conectado con exito!\n");

    // Solo aceptara a este cliente pero va a recibir de el, varios archivos
    // por eso, sigue el while(1)
    // Bucle para recibir multiples archivos durante esta conexion
    // Por cada archivo hay 3 pasos: A, B, C
    while (1) {
        char nombre_archivo[256]; // Bufer estatico para recibir siempre 256 bytes de nombre
        
        // Paso A: Recibir el nombre del archivo
        int bytes_recibidos = recv(client_fd, nombre_archivo, sizeof(nombre_archivo), 0);
        
        // Si recibimos 0 bytes, significa que el cliente cerro la conexion (termino)
        if (bytes_recibidos <= 0) {
            printf("Transferencia completada. El cliente se desconecto.\n");
            break;
        }

        // Paso B: Recibir el tamano del archivo
        long tamano_archivo;
        recv(client_fd, &tamano_archivo, sizeof(long), 0);
        printf("Recibiendo: %s (%ld bytes)\n", nombre_archivo, tamano_archivo);

        // Abrir un nuevo archivo en el directorio actual para escritura binaria ("wb")
        FILE *archivo = fopen(nombre_archivo, "wb");
        if (archivo == NULL) {
            perror("No se pudo crear el archivo para guardar");
            break;
        }

        // Paso C: Recibir el contenido del archivo usando bloques
        long total_recibido = 0;
        char buffer[BUFFER_SIZE];
        
        while (total_recibido < tamano_archivo) {
            // Calculamos cuanto falta para no leer de mas e invadir datos del proximo archivo
            int a_leer = BUFFER_SIZE;
            if (tamano_archivo - total_recibido < BUFFER_SIZE) {
                a_leer = tamano_archivo - total_recibido;
            }

            int leidos = recv(client_fd, buffer, a_leer, 0);
            if (leidos <= 0) break; // Prevencion de errores de red

            fwrite(buffer, 1, leidos, archivo); // Escribimos lo recibido en el disco
            total_recibido += leidos;
        }
        
        fclose(archivo);
        printf("-> Archivo '%s' guardado exitosamente.\n\n", nombre_archivo);
    }

    // 5. Cerrar los sockets para liberar recursos
    close(client_fd);
    close(server_fd);
    return 0;
}