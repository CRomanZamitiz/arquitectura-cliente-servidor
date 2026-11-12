#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>     // libreria para el listado de archivos
#include <sys/stat.h>   // libreria para obtener el tamano de los archivos

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    // Verificamos los parametros.
    // Son 4: el nombre del ejecutable, la IP del servidor, el puerto y el directorio a transferir
    if (argc != 4) {
        printf("Uso correcto: %s <IP_Servidor> <Puerto> <Ruta_Directorio>\n", argv[0]);
        return 1;
    }

    char *ip_servidor = argv[1];
    int puerto = atoi(argv[2]);
    char *ruta_directorio = argv[3];

    // 1. Crear el socket del cliente
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(puerto);
    server_addr.sin_addr.s_addr = inet_addr(ip_servidor);

    // 2. Conectar al servidor
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectar con el servidor. ¿Esta encendido?");
        return 1;
    }
    printf("Conectado al servidor exitosamente!\n\n");

    // 3. Abrir el directorio (Integracion del codigo que hace el listado ls)
    DIR *carpeta = opendir(ruta_directorio);
    if (carpeta == NULL) {
        perror("Error al abrir la ruta del directorio. Verifica que exista.");
        close(client_fd);
        return 1;
    }

    struct dirent *elemento;
    struct stat info_archivo;
    char ruta_completa[1024];

    // 4. Recorrer los archivos del directorio
    while ((elemento = readdir(carpeta)) != NULL) {
        // Omitir los directorios especiales ocultos "." y ".."
        if (strcmp(elemento->d_name, ".") == 0 || strcmp(elemento->d_name, "..") == 0) {
            continue;
        }

        // Construimos la ruta absoluta del archivo para poder leerlo
        snprintf(ruta_completa, sizeof(ruta_completa), "%s/%s", ruta_directorio, elemento->d_name);

        // Extraemos la informacion del archivo con stat()
        if (stat(ruta_completa, &info_archivo) == 0) {
            
            // Verificamos que sea un archivo regular, ignorando otros subdirectorios si los hubiera
            if (S_ISREG(info_archivo.st_mode)) {
                printf("Preparando envio de: %s\n", elemento->d_name);

                // PASO 1: Enviar el nombre (usamos un bufer estatico de 256 bytes para evitar desincronizaciones)
                char nombre_envio[256] = {0}; // Llenamos de ceros inicialmente
                strncpy(nombre_envio, elemento->d_name, sizeof(nombre_envio) - 1);
                send(client_fd, nombre_envio, sizeof(nombre_envio), 0);

                // PASO 2: Enviar el tamano del archivo
                long tamano = info_archivo.st_size;
                send(client_fd, &tamano, sizeof(long), 0);

                // PASO 3: Enviar el contenido
                FILE *archivo_lectura = fopen(ruta_completa, "rb"); // "rb" modo de lectura binaria
                if (archivo_lectura != NULL) {
                    char buffer[BUFFER_SIZE];
                    size_t bytes_leidos;
                    
                    // Leemos del disco en bloques y los mandamos por la red
                    while ((bytes_leidos = fread(buffer, 1, sizeof(buffer), archivo_lectura)) > 0) {
                        send(client_fd, buffer, bytes_leidos, 0);
                    }
                    fclose(archivo_lectura);
                    printf("-> Archivo '%s' enviado.\n", elemento->d_name);
                } else {
                    printf("Error al intentar abrir %s\n", ruta_completa);
                }
            }
        }
    }

    // 5. Limpieza final
    closedir(carpeta);
    close(client_fd); // Al cerrar el socket, el servidor sabe que terminamos
    printf("\nTransferencia completada con exito. Cerrando cliente.\n");

    return 0;
}