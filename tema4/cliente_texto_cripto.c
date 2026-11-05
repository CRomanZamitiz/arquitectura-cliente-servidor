#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

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
  if(argc != 3) {
      printf("Uso: %s <IP_Servidor> <Puerto>\n", argv[0]);
      exit(1);
  }

  struct sockaddr_in socket_cliente;
  struct hostent *h;
  int fd;
  int n;
  char nom[128];
  char buf[128];

  // --- NUEVO: Cargar llave privada ---
  unsigned char llave[32];
  FILE *archivoLlave = fopen("llave.key", "rb");
  if (!archivoLlave) {
      perror("Error al leer llave.key en el cliente");
      exit(1);
  }
  fread(llave, 1, sizeof(llave), archivoLlave);
  fclose(archivoLlave);

  fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 

  memset((char *) &socket_cliente, 0, sizeof(struct sockaddr_in)); 
  socket_cliente.sin_family = AF_INET; 
  socket_cliente.sin_port = htons((u_short) atoi(argv[2])); 
  h = gethostbyname(argv[1]); 
  memcpy(&socket_cliente.sin_addr, h->h_addr, h->h_length); 

  connect(fd, (struct sockaddr *) &socket_cliente, sizeof(socket_cliente)); 

  printf("Conectado al servidor. Ingresa tu nombre: ");
  fgets(nom, sizeof(nom), stdin); 
  nom[strcspn(nom, "\n")] = 0; // Limpiar el salto de linea para evitar problemas en bitacora

  // --- NUEVO: Proceso de Encriptacion ---
  unsigned char iv[16];
  RAND_bytes(iv, sizeof(iv)); // Generar IV aleatorio

  int longitudTextoOriginal = strlen(nom);
  unsigned char textoEncriptado[longitudTextoOriginal + EVP_MAX_BLOCK_LENGTH];
  int longitudPaso = 0;
  int longitudTextoEncriptado = 0;

  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, llave, iv);
  EVP_EncryptUpdate(ctx, textoEncriptado, &longitudPaso, (unsigned char*)nom, longitudTextoOriginal);
  longitudTextoEncriptado = longitudPaso;
  EVP_EncryptFinal_ex(ctx, textoEncriptado + longitudPaso, &longitudPaso);
  longitudTextoEncriptado += longitudPaso;
  EVP_CIPHER_CTX_free(ctx);

  // LLAMADA A LA NUEVA FUNCION
  // Enviamos el texto ya cifrado a nuestra funcion para ver los bytes y bits
  bytes_to_bits(textoEncriptado, longitudTextoEncriptado);

  // --- NUEVO: Envio de datos cifrados ---
  // 1. Enviar el IV (16 bytes)
  send(fd, iv, 16, 0);
  // 2. Enviar el tamano del texto encriptado
  send(fd, &longitudTextoEncriptado, sizeof(int), 0);
  // 3. Enviar el texto encriptado
  send(fd, textoEncriptado, longitudTextoEncriptado, 0);

  printf("Nombre cifrado y enviado. Esperando respuesta...\n");
  n = recv(fd, buf, sizeof(buf) - 1, 0); 
  buf[n] = '\0';
  printf("Respuesta del servidor: %s\n", buf); 
  
  close(fd); 
  exit(0); 
}