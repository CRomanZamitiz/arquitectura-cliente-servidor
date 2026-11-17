#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#define BUFFER_SIZE 1024

// Funcion para manejar errores de criptografia
void manejarErrores(void) {
  fprintf(stderr, "Ocurrio un error en el proceso de encriptacion.\n");
  exit(EXIT_FAILURE);
}

// Funcion auxiliar que encripta un archivo individual
// Toma la ruta original y crea un nuevo archivo con extension .encrypt
int encriptar_archivo(const char *ruta_entrada, const char *ruta_salida, unsigned char *llave)
{
  unsigned char iv[16];
    
  // Generar un IV unico para este archivo especifico
  if (!RAND_bytes(iv, sizeof(iv))) {
    return 0;
  }

  FILE *fEntrada = fopen(ruta_entrada, "rb");
  if (!fEntrada) return 0;
    
  FILE *fSalida = fopen(ruta_salida, "wb");
  if (!fSalida) {
    fclose(fEntrada);
      return 0;
  }

  // Escribir el IV de 16 bytes al inicio del archivo cifrado
  fwrite(iv, 1, sizeof(iv), fSalida);

  // Preparar el contexto de criptografia
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, llave, iv)) {
    EVP_CIPHER_CTX_free(ctx);
    fclose(fEntrada);
    fclose(fSalida);
    return 0;
  }

  unsigned char bufferEntrada[4096];
  unsigned char bufferSalida[4096 + EVP_MAX_BLOCK_LENGTH]; 
  int bytesLeidos, bytesCifrados;

  // Ciclo de lectura y encriptacion por pedazos o chunks
  while ((bytesLeidos = fread(bufferEntrada, 1, sizeof(bufferEntrada), fEntrada)) > 0)
  {
    if (1 != EVP_EncryptUpdate(ctx, bufferSalida, &bytesCifrados, bufferEntrada, bytesLeidos)) {
      EVP_CIPHER_CTX_free(ctx);
      fclose(fEntrada);
      fclose(fSalida);
      return 0;
    }
    fwrite(bufferSalida, 1, bytesCifrados, fSalida);
  }

  // Finalizar la encriptacion agregando los bytes de padding al final
  if (1 != EVP_EncryptFinal_ex(ctx, bufferSalida, &bytesCifrados))
  {
    EVP_CIPHER_CTX_free(ctx);
    fclose(fEntrada);
    fclose(fSalida);
    return 0;
  }
  fwrite(bufferSalida, 1, bytesCifrados, fSalida);

  // Limpieza de memoria y cierre de archivos
  EVP_CIPHER_CTX_free(ctx);
  fclose(fEntrada);
  fclose(fSalida);

  return 1;
}

int main(int argc, char *argv[])
{
  // Verificamos los parametros de red y directorio
  if (argc != 4) {
    printf("Uso correcto: %s <IP_Servidor> <Puerto> <Ruta_Directorio>\n", argv[0]);
    return 1;
  }

  char *ip_servidor = argv[1];
  int puerto = atoi(argv[2]);
  char *ruta_directorio = argv[3];

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

  // 2. Crear el socket del cliente y conectar
  int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(puerto);
  server_addr.sin_addr.s_addr = inet_addr(ip_servidor);

  if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("Error al conectar con el servidor. Esta encendido?");
    return 1;
  }
  printf("Conectado al servidor exitosamente!\n\n");

  // 3. Abrir el directorio
  DIR *carpeta = opendir(ruta_directorio);
  if (carpeta == NULL) {
    perror("Error al abrir la ruta del directorio. Verifica que exista.");
    close(client_fd);
    return 1;
  }

  struct dirent *elemento;
  struct stat info_archivo;
  char ruta_completa[1024];
  char ruta_encriptada[1024];
  char nombre_encriptado[256];

  // 4. Recorrer los archivos del directorio
  while ((elemento = readdir(carpeta)) != NULL) {
    // Omitir directorios especiales y la llave generada para evitar auto-enviarla
    if (strcmp(elemento->d_name, ".") == 0 || 
        strcmp(elemento->d_name, "..") == 0 ||
        strcmp(elemento->d_name, "llave_archivo.key") == 0)
    {
      continue;
    }

    // Evitar procesar archivos que ya terminen en .encrypt para prevenir doble cifrado
    if (strstr(elemento->d_name, ".encrypt") != NULL) {
      continue;
    }

    snprintf(ruta_completa, sizeof(ruta_completa), "%s/%s", ruta_directorio, elemento->d_name);

    if (stat(ruta_completa, &info_archivo) == 0) {
      if (S_ISREG(info_archivo.st_mode)) {
        printf("Preparando y encriptando: %s\n", elemento->d_name);

        // Definir los nombres y rutas del nuevo archivo encriptado
        snprintf(ruta_encriptada, sizeof(ruta_encriptada), "%s.encrypt", ruta_completa);
        snprintf(nombre_encriptado, sizeof(nombre_encriptado), "%s.encrypt", elemento->d_name);

        // Realizar la encriptacion
        if (encriptar_archivo(ruta_completa, ruta_encriptada, llave))
        {
          // Extraemos la informacion del NUEVO archivo encriptado para saber su tamano
          struct stat info_encriptado;
          if (stat(ruta_encriptada, &info_encriptado) == 0) {
                        
            // PASO A: Enviar el nombre
            char nombre_envio[256] = {0};
            strncpy(nombre_envio, nombre_encriptado, sizeof(nombre_envio) - 1);
            send(client_fd, nombre_envio, sizeof(nombre_envio), 0);

            // PASO B: Enviar el tamano del archivo
            long tamano = info_encriptado.st_size;
            send(client_fd, &tamano, sizeof(long), 0);

            // PASO C: Enviar el contenido
            FILE *archivo_lectura = fopen(ruta_encriptada, "rb");
            if (archivo_lectura != NULL) {
              char buffer[BUFFER_SIZE];
              size_t bytes_leidos;
                           
              while ((bytes_leidos = fread(buffer, 1, sizeof(buffer), archivo_lectura)) > 0)
              {
                send(client_fd, buffer, bytes_leidos, 0);
              }
              fclose(archivo_lectura);
              printf("-> Archivo '%s' enviado de forma segura.\n", nombre_encriptado);
            }
          }
        }
        else
        {
          printf("Hubo un fallo al intentar encriptar %s\n", ruta_completa);
        }
      }
    }
  }

  // 5. Limpieza final
  closedir(carpeta);
  close(client_fd);
  printf("\nTransferencia completada con exito. Cerrando cliente.\n");

  return 0;
}