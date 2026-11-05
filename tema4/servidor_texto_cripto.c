#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h> 
#include <fcntl.h> 
#include <openssl/evp.h>

#define QLEN 10 

// NUEVA FUNCION: Muestra un arreglo de bytes en formato caracter, decimal (con signo) y binario
void bytes_to_bits(unsigned char *texto, int longitud) {
    printf("El argumento ENCRIPTADO pasado a bytes y a bits es:\n");
    
    // Recorremos cada byte del texto
    for (int i = 0; i < longitud; i++) {
        // Imprimimos el caracter y su valor numerico como entero con signo (char)
        // Usamos %-4c para alinear a la izquierda el caracter y %4d para el número
        printf("%-4c %4d    ", texto[i], (char)texto[i]);
        
        // Bucle para imprimir los 8 bits del byte actual
        // Empezamos desde el bit mas significativo (posicion 7) hasta el 0
        for (int j = 7; j >= 0; j--) {
            // Desplazamos los bits hacia la derecha 'j' posiciones y usamos AND (&) con 1
            // para saber si el bit en esa posicion es 0 o 1
            int bit = (texto[i] >> j) & 1;
            printf("%d", bit);
        }
        printf("\n"); // Salto de linea para el siguiente byte
    }
    printf("\n");
}

void main(int argc, char *argv[])
{
  if(argc != 2) {
      printf("Uso: %s <Puerto>\n", argv[0]);
      exit(1);
  }

  // --- NUEVO: Cargar llave privada ---
  unsigned char llave[32];
  FILE *archivoLlave = fopen("llave.key", "rb");
  if (!archivoLlave) {
      perror("Error al leer llave.key en el servidor");
      exit(1);
  }
  fread(llave, 1, sizeof(llave), archivoLlave);
  fclose(archivoLlave);
  printf("Servidor iniciado y llave cargada correctamente.\n");

  struct sockaddr_in servidor; 
  struct sockaddr_in cliente; 
  int fd_s, fd_c; 
  int longClient; 
  char buf_fecha[50]; 
  int num_alumnos = 0; 

  fd_s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); 
	      
  memset((char *) &servidor, 0, sizeof(servidor)); 
  servidor.sin_family = AF_INET; 
  servidor.sin_addr.s_addr = INADDR_ANY; 
  servidor.sin_port = htons((u_short) atoi(argv[1])); 
  memset(&(servidor.sin_zero), '\0', 8); 

  bind(fd_s, (struct sockaddr *) &servidor, sizeof(servidor)); 
  listen(fd_s, QLEN); 

  longClient = sizeof(cliente); 

  while(1) 
  {
    fd_c = accept(fd_s, (struct sockaddr *) &cliente, &longClient); 
    char* ip_cliente = inet_ntoa(cliente.sin_addr); 
    num_alumnos++; 

    // --- NUEVO: Recepcion y Desencriptacion ---
    unsigned char iv[16];
    int longitudTextoEncriptado;
    
    // 1. Recibir el IV
    recv(fd_c, iv, 16, 0);
    // 2. Recibir la longitud del mensaje
    recv(fd_c, &longitudTextoEncriptado, sizeof(int), 0);
    
    // 3. Recibir el mensaje cifrado
    unsigned char textoEncriptado[longitudTextoEncriptado];
    recv(fd_c, textoEncriptado, longitudTextoEncriptado, 0);

    // LLAMADA A LA NUEVA FUNCION
    // Enviamos el texto ya cifrado a nuestra funcion para ver los bytes y bits
    bytes_to_bits(textoEncriptado, longitudTextoEncriptado);

    unsigned char textoDesencriptado[longitudTextoEncriptado];
    int longitudPaso = 0;
    int longitudTextoDesencriptado = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, llave, iv);
    EVP_DecryptUpdate(ctx, textoDesencriptado, &longitudPaso, textoEncriptado, longitudTextoEncriptado);
    longitudTextoDesencriptado = longitudPaso;
    EVP_DecryptFinal_ex(ctx, textoDesencriptado + longitudPaso, &longitudPaso);
    longitudTextoDesencriptado += longitudPaso;
    textoDesencriptado[longitudTextoDesencriptado] = '\0'; // Asegurar el fin de cadena
    EVP_CIPHER_CTX_free(ctx);

    // Formatear fecha y hora originales
    time_t T = time(NULL); 
    struct tm tm = *localtime(&T); 
    int length_fecha = sprintf(buf_fecha, "%d : %s-%02d/%02d/%04d %02d:%02d:%02d-", 
                               num_alumnos, ip_cliente, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec); 
    
    write(1, buf_fecha, length_fecha); 
    // Imprimir el nombre ya desencriptado en lugar del buffer original
    write(1, textoDesencriptado, longitudTextoDesencriptado); 
    write(1, "\n", 1); 

    char *respuesta = "Registro exitoso"; 
    send(fd_c, respuesta, strlen(respuesta), 0); 

    close(fd_c); 
  }
  close(fd_s); 
  shutdown( fd_s, SHUT_RDWR ); 
  exit(0); 
}