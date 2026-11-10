#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

void manejarErrores(void) {
    fprintf(stderr, "Ocurrio un error en el proceso de encriptacion.\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <archivo_a_encriptar>\n", argv[0]);
        return 1;
    }

    // 1. Preparar nombres de archivos
    char *archivoEntradaStr = argv[1];
    char archivoSalidaStr[512];
    // Creamos el nombre de salida concatenando ".encrypt"
    snprintf(archivoSalidaStr, sizeof(archivoSalidaStr), "%s.encrypt", archivoEntradaStr);

    // 2. Generar llave y IV
    unsigned char llave[32];
    unsigned char iv[16];
    if (!RAND_bytes(llave, sizeof(llave)) || !RAND_bytes(iv, sizeof(iv))) {
        manejarErrores();
    }

    // 3. Guardar la llave en "llave_archivo.key"
    FILE *fLlave = fopen("llave_archivo.key", "wb");
    if (!fLlave) {
        perror("Error al crear llave_archivo.key");
        return 1;
    }
    fwrite(llave, 1, sizeof(llave), fLlave);
    fclose(fLlave);
    printf("Llave generada y guardada en 'llave_archivo.key'.\n");

    // 4. Abrir archivos de datos en modo Binario (rb = read binary, wb = write binary)
    FILE *fEntrada = fopen(archivoEntradaStr, "rb");
    if (!fEntrada) {
        perror("Error al abrir el archivo de entrada");
        return 1;
    }
    FILE *fSalida = fopen(archivoSalidaStr, "wb");
    if (!fSalida) {
        perror("Error al crear el archivo .encrypt");
        fclose(fEntrada);
        return 1;
    }

    // 5. Escribir el IV de 16 bytes al inicio del archivo cifrado
    // No es un dato privado como si lo es la llave
    // y es necesario para desencriptar
    fwrite(iv, 1, sizeof(iv), fSalida);

    // 6. Preparar el contexto de criptografia
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, llave, iv)) manejarErrores();

    // 7. Ciclo de lectura y encriptacion por pedazos (Chunks)
    unsigned char bufferEntrada[4096];
    // El buffer de salida debe ser un bloque mas grande que el de entrada
    unsigned char bufferSalida[4096 + EVP_MAX_BLOCK_LENGTH]; 
    int bytesLeidos, bytesCifrados;

    printf("Encriptando archivo '%s'...\n", archivoEntradaStr);
    
    // Leemos hasta 4096 bytes en cada pasada
    while ((bytesLeidos = fread(bufferEntrada, 1, sizeof(bufferEntrada), fEntrada)) > 0) {
        if (1 != EVP_EncryptUpdate(ctx, bufferSalida, &bytesCifrados, bufferEntrada, bytesLeidos))
            manejarErrores();
        // Escribimos los bytes que se acaban de encriptar en esta pasada
        fwrite(bufferSalida, 1, bytesCifrados, fSalida);
    }

    // 8. Finalizar la encriptacion (agrega los bytes de padding al final)
    if (1 != EVP_EncryptFinal_ex(ctx, bufferSalida, &bytesCifrados)) manejarErrores();
    fwrite(bufferSalida, 1, bytesCifrados, fSalida);

    // 9. Limpieza de memoria y cerrado de archivos
    EVP_CIPHER_CTX_free(ctx);
    fclose(fEntrada);
    fclose(fSalida);

    printf("¡Exito! Archivo guardado como '%s'\n", archivoSalidaStr);
    return 0;
}