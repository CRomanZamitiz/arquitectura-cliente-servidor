#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>  // Definiciones para operaciones de internet (IPs)
#include <sys/socket.h> // API principal de sockets
#include <sys/select.h> // La libreria de la multiplexacion (select)

#define MAX_CLIENTS 30
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    // servidor_socket: El socket que "escucha" nuevas conexiones
    // new_socket: Socket temporal para aceptar a un cliente
    // client_socket: Arreglo para llevar el control de todos los descriptores de archivos activos
    int servidor_socket, addrlen, new_socket, activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;
    int client_socket[MAX_CLIENTS];
    
    // fd_set: Es una estructura que actua como una "bolsa" de descriptores de archivos
    // que el kernel vigilara para ver si tienen datos listos para leer.
    fd_set readfds;
    
    char buffer[BUFFER_SIZE];
    char formatted_msg[BUFFER_SIZE + 100]; // Espacio extra para el prefijo [IP:Port]

    // Paso 1: Inicializar el arreglo de clientes con 0.
    // Un 0 significa que ese espacio en nuestra "sala de chat" esta vacio.
    for (i = 0; i < MAX_CLIENTS; i++) {
        client_socket[i] = 0;
    }

    // Paso 2: Crear el socket de escucha (Servidor Socket)
    // AF_INET: IPv4, SOCK_STREAM: TCP
    if ((servidor_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error al crear socket servidor");
        exit(EXIT_FAILURE);
    }

    // Paso 3: Configurar SO_REUSEADDR.
    // Esto permite reiniciar el servidor instantaneamente sin esperar al estado TIME_WAIT del kernel.
    int opt = 1;
    if (setsockopt(servidor_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == -1) {
        perror("Error en setsockopt");
        exit(EXIT_FAILURE);
    }

    // Paso 4: Definir la direccion y el puerto del servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Escuchar en todas las interfaces de red disponibles
    address.sin_port = htons( atoi(argv[1]) );     // Convertir el puerto a orden de bytes de red (Big Endian)

    // Paso 5: Vincular (Bind) el socket al puerto fisico
    if (bind(servidor_socket, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Error en bind");
        exit(EXIT_FAILURE);
    }

    // Paso 6: Poner el socket en modo escucha (Listen)
    if (listen(servidor_socket, 10) == -1) {
        perror("Error en listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(address);
    printf("Servidor de CHAT PROFESIONAL iniciado en el puerto %s.\n", argv[1]);
    printf("Vigilando conexiones y mensajes simultaneos...\n");

    while (1) {
        // Limpiar el conjunto de descriptores (el radar)
        FD_ZERO(&readfds);

        // Agrega el socket servidor al radar (para detectar nuevos clientes)
        FD_SET(servidor_socket, &readfds);
        max_sd = servidor_socket;

        // Agrega los sockets de clientes que ya estan conectados al radar
        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];
            
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            
            // select() necesita saber cual es el numero de descriptor mas alto para ser eficiente
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // Paso 7: La funcion select() BLOQUEA el programa hasta que algo ocurra.
        // Es mucho mas eficiente que un bucle infinito que consume CPU.
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            printf("Error en select (interrupcion de señal quiza)\n");
        }

        // --- CASO A: NUEVA CONEXION ENTRANTE ---
        if (FD_ISSET(servidor_socket, &readfds)) {
            if ((new_socket = accept(servidor_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Error en accept");
                exit(EXIT_FAILURE);
            }

            // Convertir la IP binaria a texto legible y el puerto a entero
            char *ip_str = inet_ntoa(address.sin_addr);
            int port_int = ntohs(address.sin_port);
            printf("[NUEVO] Conexion aceptada desde %s:%d\n", ip_str, port_int);

            // Mensaje de bienvenida
            char *welcome = ">>> Bienvenido al chat. Tu IP y puerto te identifican.\n";
            send(new_socket, welcome, strlen(welcome), 0);

            // Guardar el nuevo socket en nuestro arreglo de control
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    break;
                }
            }
        }

        // --- CASO B: ACTIVIDAD DE DATOS EN UN CLIENTE EXISTENTE ---
        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                // Intentar leer datos del socket
                valread = recv(sd, buffer, BUFFER_SIZE, 0);
                
                if (valread == 0) {
                    // Si recv retorna 0, significa que el cliente cerro la conexion (FIN)
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    printf("[LOG] Cliente %s:%d desconectado\n", 
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    
                    close(sd);
                    client_socket[i] = 0; // Liberar espacio en nuestro arreglo
                } 
                else {
                    // Tenemos un mensaje. Terminamos la cadena para que printf/snprintf funcionen.
                    buffer[valread] = '\0';

                    // 1. Obtener la identidad real del remitente desde el Kernel
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    char sender_ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(address.sin_addr), sender_ip, INET_ADDRSTRLEN);
                    int sender_port = ntohs(address.sin_port);

                    // 2. Construir el mensaje de broadcast con la identidad [IP:PUERTO]
                    // snprintf es mas seguro que sprintf porque evita desbordamientos de bufer.
                    snprintf(formatted_msg, sizeof(formatted_msg), "[%s:%d]: %s", 
                             sender_ip, sender_port, buffer);

                    // 3. BROADCAST: Enviar el mensaje formateado a todos los DEMAS clientes
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        int out_sd = client_socket[j];
                        
                        // Solo enviar si: el slot no es 0 Y no es el mismo que envio el mensaje
                        if (out_sd != 0 && out_sd != sd) {
                            send(out_sd, formatted_msg, strlen(formatted_msg), 0);
                        }
                    }
                    
                    // Opcional: imprimir en la consola del servidor lo que se replico
                    printf("Broadcast: %s", formatted_msg);
                }
            }
        }
    }

    return 0;
}
