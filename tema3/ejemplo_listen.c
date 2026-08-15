#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main( int argc, char *argv[] ) {
    if (argc != 2) {
        printf("Uso: %s <puerto>\n", argv[0]);
        return 1;
    }

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;

    // Paso 1 funcion socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Error en socket()");
        return 1;
    }

    // --- EVITA EL ERROR "Address already in use" ---
    /*
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("Error en setsockopt");
        return 1;
    }
    */

    // Paso 2 inicializamos la estructura de tipo sockaddr_in del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons( atoi( argv[1] ) );

    // Paso 3 funcion bind
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error en bind()");
        return 1;
    }

    // Paso 4 funcion listen, aqui se especifica el backlog
    // Escuchar conexiones con backlog = 1 (Para forzar el colapso rapido!)
    if (listen(server_fd, 1) == -1) {
        perror("Error en listen()");
        return 1;
    }

    printf("Servidor iniciado en el puerto %s.\n", argv[1]);

    // Loop principal: mantiene al servidor vivo indefinidamente
    while (1) {
        printf("\n[!] Servidor en estado LISTEN. Esperando en accept()...\n");
        
        socklen_t addr_size = sizeof(client_addr);
        // Paso 5 funcion accept
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_size);
        if (client_fd == -1) {
            perror("Error en accept()");
            continue; // Si hay un error, lo ignoramos y volvemos a intentar
        }

        printf(">>> Cliente aceptado! El servidor esta ahora OCUPADO atendiendo a este cliente.\n");

        // Loop secundario: mantiene al cliente "cautivo"
        char buffer[1024];
        int bytes_read;
        
        // recv() bloquea el proceso hasta que el cliente envie algo o se desconecte
        while ((bytes_read = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
            write(STDOUT_FILENO, "Recibido: ", 10);
            write(STDOUT_FILENO, buffer, bytes_read);
        }

        // Cuando el cliente hace Ctrl+C, recv() retorna 0 y salimos del bucle
        printf("<<< El cliente se ha desconectado. Liberando servidor...\n");
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
