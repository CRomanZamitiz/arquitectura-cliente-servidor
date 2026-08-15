/*
 * ============================================================================
 * Practica de Laboratorio: Sniffer de Red en Capa 2 (Raw Sockets)
 * Descripcion: Intercepta y analiza tramas Ethernet, paquetes IPv4 y 
 * segmentos TCP directamente desde la tarjeta de red.
 * Advertencia: Este programa requiere privilegios de superusuario (root) 
 * y la interfaz de red en "Modo Promiscuo" para ver trafico ajeno.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>       // Estructura de la cabecera IPv4 (struct iphdr)
#include <netinet/tcp.h>      // Estructura de la cabecera TCP (struct tcphdr)
#include <netinet/if_ether.h> // Estructura de la cabecera Ethernet (struct ethhdr)
#include <arpa/inet.h>        // Funciones de conversion como inet_ntoa y ntohs
#include <unistd.h>
#include <linux/if_packet.h>  // Definiciones para AF_PACKET (Capa de Enlace)
#include <net/if.h>

int main() {
    int raw_socket;
    // Reservamos 64KB de memoria. Este es el tamano maximo teorico de un paquete IP.
    unsigned char *buffer = (unsigned char *)malloc(65536);
    
    /* * PASO 1: CREACION DEL SOCKET EN CRUDO (RAW SOCKET)
     * AF_PACKET: Nos permite interactuar directamente con el driver del dispositivo 
     * de red (Capa 2 del modelo OSI - Enlace de Datos).
     * SOCK_RAW:  Le decimos al kernel que no procese el paquete, lo queremos "crudo".
     * ETH_P_ALL: Queremos capturar TODOS los protocolos de Ethernet (IPv4, ARP, IPv6, etc.).
     * Nota: Usamos htons() porque ETH_P_ALL debe enviarse en Network Byte Order.
     */
    raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_socket < 0) {
        perror("Error al crear el socket (Ejecutaste el programa con sudo?)");
        return 1;
    }

    printf(">>> SNIFFER ACTIVO: Escuchando tramas Ethernet en la red...\n");
    printf(">>> Esperando trafico TCP en el puerto 8080...\n");

    // Bucle infinito para escuchar paquetes continuamente
    while (1) {
        /*
         * PASO 2: RECIBIR EL PAQUETE
         * recvfrom captura los bits que llegan a la antena/cable y los guarda en 'buffer'.
         * data_size nos dira cuantos bytes exactos capturamos.
         */
        int data_size = recvfrom(raw_socket, buffer, 65536, 0, NULL, NULL);
        
        // Si hay un error de lectura (ruido, interferencia), ignoramos y continuamos
        if (data_size < 0) {
            continue;
        }

        /*
         * PASO 3: ANALISIS DE LA CAPA 2 (ENLACE DE DATOS - ETHERNET)
         * El buffer empieza con la cabecera Ethernet, que siempre mide 14 bytes.
         * Hacemos un "casting" para que C lea los primeros 14 bytes como una estructura 'ethhdr'.
         */
        struct ethhdr *eth = (struct ethhdr *)buffer;

        // Verificamos el campo 'h_proto' de la cabecera Ethernet.
        // ETH_P_IP (0x0800) significa que el paquete encapsulado dentro de Ethernet es IPv4.
        // Usamos ntohs() porque el paquete viene de la red (Big-Endian) y nuestra CPU 
        // necesita procesarlo en su formato nativo (Host Order / Little-Endian).
        if (ntohs(eth->h_proto) == ETH_P_IP) {
            
            /*
             * PASO 4: ANALISIS DE LA CAPA 3 (RED - IPv4)
             * Para leer la cabecera IP, debemos "saltarnos" la cabecera Ethernet.
             * Sumamos sizeof(struct ethhdr) (14 bytes) a la direccion base del buffer.
             */
            struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));

            // El campo 'protocol' dentro de IPv4 nos indica que hay en la capa superior.
            // El valor 6 es el estandar de la IANA para el protocolo TCP.
            if (iph->protocol == 6) {
                
                /*
                 * PASO 5: ANALISIS DE LA CAPA 4 (TRANSPORTE - TCP)
                 * Las cabeceras IP tienen un tamano variable (usualmente 20 bytes).
                 * El campo 'ihl' (Internet Header Length) indica la longitud en "palabras de 32 bits".
                 * Multiplicamos por 4 para obtener los bytes reales (ej. 5 * 4 = 20 bytes).
                 */
                unsigned short iphdrlen = iph->ihl * 4;
                
                // Saltamos la cabecera Ethernet (14) + la cabecera IP (ej. 20) para llegar a TCP
                struct tcphdr *tcph = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + iphdrlen);

                // Filtramos para capturar SOLO la comunicacion de nuestro laboratorio (puerto 8080).
                // Revisamos tanto el puerto de destino (peticion) como el origen (respuesta).
                if (ntohs(tcph->dest) == 8080 || ntohs(tcph->source) == 8080) {
                    
                    /*
                     * PASO 6: CALCULO DEL PAYLOAD (LOS DATOS REALES)
                     * Igual que IP, la cabecera TCP tiene tamano variable.
                     * 'doff' (Data Offset) nos da el tamano de la cabecera TCP en palabras de 32 bits.
                     */
                    int tcp_header_len = tcph->doff * 4;
                    
                    // Tamano total de TODAS las cabeceras sumadas (Ethernet + IP + TCP)
                    int total_headers_size = sizeof(struct ethhdr) + iphdrlen + tcp_header_len;
                    
                    // El payload (el mensaje "Hola mundo" o el "GET / HTTP") empieza 
                    // justo despues de todas las cabeceras.
                    unsigned char *payload = buffer + total_headers_size;
                    
                    // El tamano de los datos es el paquete completo menos las cabeceras
                    int payload_size = data_size - total_headers_size;

                    // Si efectivamente hay datos (no es solo un paquete de control como SYN o ACK)
                    if (payload_size > 0) {
                        printf("\n=======================================================");
                        printf("\n---[ PAQUETE INTERCEPTADO EN TEXTO PLANO ]---");
                        
                        // Imprimimos las direcciones MAC (Capa 2) extrayendolas byte por byte en Hexadecimal
                        printf("\nMAC Origen: %02x:%02x:%02x:%02x:%02x:%02x", 
                               eth->h_source[0], eth->h_source[1], eth->h_source[2], 
                               eth->h_source[3], eth->h_source[4], eth->h_source[5]);
                        
                        // Imprimimos la IP de origen (Capa 3) convirtiendola de binario a texto legible
                        printf("\nIP Origen : %s", inet_ntoa(*(struct in_addr *)&iph->saddr));
                        
                        // Imprimimos los puertos involucrados (Capa 4)
                        printf("\nPuerto Src: %d -> Puerto Dst: %d", ntohs(tcph->source), ntohs(tcph->dest));
                        
                        // Imprimimos el payload (Capa de Aplicacion). Usamos %.*s para imprimir
                        // exactamente la cantidad de bytes que calculamos, evitando leer memoria extra.
                        printf("\n\nContenido del mensaje (Payload):\n%.*s\n", payload_size, payload);
                        printf("=======================================================\n");
                    }
                }
            }
        }
    }
    
    // Liberar la memoria reservada antes de cerrar (buena practica en C)
    free(buffer);
    close(raw_socket);
    return 0;
}
