#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/evp.h>

#define BUFFER_SIZE 1024

// Funcion para manejar errores de criptografia
void manejarErrores(void) {
  fprintf(stderr, "Ocurrio un error en el proceso de desencriptacion.\n");
  exit(EXIT_FAILURE);
}

// Funcion que desencripta un archivo que acaba de ser recibido
void desencriptar_archivo(const char *archivoEntradaStr, unsigned char *llave) {
  char archivoSalidaStr[512];
  strncpy(archivoSalidaStr, archivoEntradaStr, sizeof(archivoSalidaStr));
    
  // Buscamos ".encrypt" para reemplazarlo por ".decrypt"
  char *extension = strstr(archivoSalidaStr, ".encrypt");
  if (extension != NULL) {
    strcpy(extension, ".decrypt");
  }
  else
  {
    strcat(archivoSalidaStr, ".decrypt");
  }

  FILE *fEntrada = fopen(archivoEntradaStr, "rb");
  if (!fEntrada) {
    perror("Error al abrir el archivo encriptado");
    return;
  }
  FILE *fSalida = fopen(archivoSalidaStr, "wb");
  if (!fSalida) {
    perror("Error al crear el archivo .decrypt");
    fclose(fEntrada);
    return;
  }

  // Leer el IV (los primeros 16 bytes)
  unsigned char iv[16];
  if (fread(iv, 1, sizeof(iv), fEntrada) != sizeof(iv)) {
    fprintf(stderr, "El archivo es demasiado pequeno o esta corrupto.\n");
    fclose(fEntrada);
    fclose(fSalida);
    return;
  }

  // Preparar el contexto de criptografia para DESENCRIPTAR
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, llave, iv))
    manejarErrores();

  // Ciclo de lectura y desencriptacion por pedazos
  unsigned char bufferEntrada[4096 + EVP_MAX_BLOCK_LENGTH]; 
  unsigned char bufferSalida[4096 + EVP_MAX_BLOCK_LENGTH];
  int bytesLeidos, bytesDescifrados;

  printf("Desencriptando archivo '%s'...\n", archivoEntradaStr);
  while ((bytesLeidos = fread(bufferEntrada, 1, 4096, fEntrada)) > 0) {
    if (1 != EVP_DecryptUpdate(ctx, bufferSalida, &bytesDescifrados, bufferEntrada, bytesLeidos))
      manejarErrores();
    fwrite(bufferSalida, 1, bytesDescifrados, fSalida);
  }

  // Finalizar (quita el padding y escribe los ultimos bytes)
  if (1 != EVP_DecryptFinal_ex(ctx, bufferSalida, &bytesDescifrados)) {
    fprintf(stderr, "Error final. Llave incorrecta o archivo danado.\n");
    EVP_CIPHER_CTX_free(ctx);
    fclose(fEntrada);
    fclose(fSalida);
    return;
  }
  fwrite(bufferSalida, 1, bytesDescifrados, fSalida);

  // Limpieza
  EVP_CIPHER_CTX_free(ctx);
  fclose(fEntrada);
  fclose(fSalida);

  // Opcional: puedes eliminar el archivo encriptado original usando remove(archivoEntradaStr);
  // printf("Exito! Archivo recuperado como '%s'\n\n", archivoSalidaStr);
}

int main(int argc, char *argv[]) {
  // Verificamos que el usuario pase el puerto como argumento
  if (argc != 2) {
    printf("Uso correcto: %s <puerto>\n", argv[0]);
    return 1;
  }

  int puerto = atoi(argv[1]);

  // 1. Leer la llave desde "llave_archivo.key" antes de iniciar el servidor
  unsigned char llave[32];
  FILE *fLlave = fopen("llave_archivo.key", "rb");
  if (!fLlave) {
    perror("Error al leer 'llave_archivo.key'. el archivo llave esta en esta carpeta?");
    return 1;
  }
  fread(llave, 1, sizeof(llave), fLlave);
  fclose(fLlave);
  printf("Llave de desencriptacion cargada correctamente.\n");

  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;

  // 2. Crear el socket AF_INET (IPv4) y SOCK_STREAM (TCP)
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("Error al crear el socket del servidor");
    return 1;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(puerto);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // 3. Unir (Bind) el socket al puerto especificado
  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("Error al hacer bind. Quizas el puerto este ocupado");
    return 1;
  }

  // 4. Escuchar conexiones entrantes
  listen(server_fd, 1);
  printf("Servidor seguro iniciado y escuchando en el puerto %d...\n", puerto);

  // 5. Aceptar la conexion del cliente
  addr_size = sizeof(client_addr);
  client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_size);
  printf("Cliente conectado con exito!\n\n");

  // Bucle para recibir multiples archivos durante esta conexion
  while (1) {
    char nombre_archivo[256];
        
    // Paso A: Recibir el nombre del archivo
    int bytes_recibidos = recv(client_fd, nombre_archivo, sizeof(nombre_archivo), 0);
        
    if (bytes_recibidos <= 0) {
      printf("Transferencia completada. El cliente se desconecto.\n");
      break;
    }

    // Paso B: Recibir el tamano del archivo
    long tamano_archivo;
    recv(client_fd, &tamano_archivo, sizeof(long), 0);
    printf("Recibiendo: %s (%ld bytes)\n", nombre_archivo, tamano_archivo);

    // Abrir un nuevo archivo en el directorio actual para escritura binaria
    FILE *archivo = fopen(nombre_archivo, "wb");
    if (archivo == NULL) {
      perror("No se pudo crear el archivo para guardar");
      break;
    }

    // Paso C: Recibir el contenido del archivo usando bloques
    long total_recibido = 0;
    char buffer[BUFFER_SIZE];

    while (total_recibido < tamano_archivo) {
      int a_leer = BUFFER_SIZE;
      if (tamano_archivo - total_recibido < BUFFER_SIZE) {
        a_leer = tamano_archivo - total_recibido;
      }

      int leidos = recv(client_fd, buffer, a_leer, 0);
      if (leidos <= 0)
        break;

      fwrite(buffer, 1, leidos, archivo);
      total_recibido += leidos;
    }
        
    fclose(archivo);
    printf("-> Archivo '%s' guardado exitosamente en disco.\n", nombre_archivo);

    // PASO D: Proceder a desencriptar automaticamente el archivo que acabamos de recibir
    desencriptar_archivo(nombre_archivo, llave);
  }

  // 6. Cerrar los sockets para liberar recursos
  close(client_fd);
  close(server_fd);
  return 0;
}