#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

void manejarErrores(void) {
    fprintf(stderr, "Ocurrio un error en el proceso de desencriptacion.\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <archivo_a_desencriptar.encrypt>\n", argv[0]);
        return 1;
    }

    // 1. Configurar nombres de archivo
    char *archivoEntradaStr = argv[1];
    char archivoSalidaStr[512];
    strncpy(archivoSalidaStr, archivoEntradaStr, sizeof(archivoSalidaStr));
    
    // Buscamos ".encrypt" para reemplazarlo por ".decrypt"
    char *extension = strstr(archivoSalidaStr, ".encrypt");
    if (extension != NULL) {
        strcpy(extension, ".decrypt"); // Reemplaza
    } else {
        // Si por alguna razon el archivo no tiene la extension, se la agregamos al final
        strcat(archivoSalidaStr, ".decrypt");
    }

    // 2. Leer la llave desde "llave_archivo.key"
    unsigned char llave[32];
    FILE *fLlave = fopen("llave_archivo.key", "rb");
    if (!fLlave) {
        perror("Error al leer 'llave_archivo.key'. ¿Asegurate de haberla generado?");
        return 1;
    }
    fread(llave, 1, sizeof(llave), fLlave);
    fclose(fLlave);

    // 3. Abrir archivos binarios
    FILE *fEntrada = fopen(archivoEntradaStr, "rb");
    if (!fEntrada) {
        perror("Error al abrir el archivo encriptado");
        return 1;
    }
    FILE *fSalida = fopen(archivoSalidaStr, "wb");
    if (!fSalida) {
        perror("Error al crear el archivo .decrypt");
        fclose(fEntrada);
        return 1;
    }

    // 4. Leer el IV (los primeros 16 bytes que guardamos al encriptar)
    unsigned char iv[16];
    if (fread(iv, 1, sizeof(iv), fEntrada) != sizeof(iv)) {
        fprintf(stderr, "El archivo es demasiado pequeno o esta corrupto.\n");
        exit(1);
    }

    // 5. Preparar el contexto de criptografia para DESENCRIPTAR
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, llave, iv)) manejarErrores();

    // 6. Ciclo de lectura y desencriptacion por pedazos
    unsigned char bufferEntrada[4096 + EVP_MAX_BLOCK_LENGTH]; 
    unsigned char bufferSalida[4096 + EVP_MAX_BLOCK_LENGTH];
    int bytesLeidos, bytesDescifrados;

    printf("Desencriptando archivo '%s'...\n", archivoEntradaStr);

    while ((bytesLeidos = fread(bufferEntrada, 1, 4096, fEntrada)) > 0) {
        if (1 != EVP_DecryptUpdate(ctx, bufferSalida, &bytesDescifrados, bufferEntrada, bytesLeidos))
            manejarErrores();
        fwrite(bufferSalida, 1, bytesDescifrados, fSalida);
    }

    // 7. Finalizar (quita el padding y escribe los ultimos bytes)
    // Si la llave o el archivo son incorrectos, EVP_DecryptFinal_ex fallara aqui.
    if (1 != EVP_DecryptFinal_ex(ctx, bufferSalida, &bytesDescifrados)) {
        fprintf(stderr, "Error final. Llave incorrecta o archivo danado.\n");
        exit(EXIT_FAILURE);
    }
    fwrite(bufferSalida, 1, bytesDescifrados, fSalida);

    // 8. Limpieza
    EVP_CIPHER_CTX_free(ctx);
    fclose(fEntrada);
    fclose(fSalida);

    printf("¡Exito! Archivo recuperado como '%s'\n", archivoSalidaStr);
    return 0;
}