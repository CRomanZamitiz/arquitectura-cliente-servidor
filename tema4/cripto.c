#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Librerias de OpenSSL necesarias para la criptografia
#include <openssl/evp.h>
#include <openssl/rand.h>

// Funcion sencilla para manejar errores y salir del programa de forma segura
void manejarErrores(void) {
    fprintf(stderr, "Ocurrio un error en el proceso criptografico.\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    // 1. Verificar que el usuario haya ingresado una frase en la linea de comandos
    if (argc != 2) {
        printf("Uso correcto: %s \"Frase a encriptar\"\n", argv[0]);
        return 1;
    }

    // Guardamos la frase ingresada por el usuario
    unsigned char *textoOriginal = (unsigned char *)argv[1];
    int longitudTextoOriginal = strlen((char *)textoOriginal);

    // 2. Variables para la criptografia AES-256
    // La llave para AES-256 debe ser de 32 bytes (256 bits)
    unsigned char llave[32]; 
    // El Vector de Inicializacion (IV) para AES siempre es de 16 bytes (128 bits)
    unsigned char iv[16];    

    // Generar bytes aleatorios seguros para la llave y el IV
    if (!RAND_bytes(llave, sizeof(llave)) || !RAND_bytes(iv, sizeof(iv))) {
        manejarErrores();
    }

    // 3. Guardar la llave generada en un archivo llamado "llave.key"
    FILE *archivoLlave = fopen("llave.key", "wb"); // Abrimos en modo escritura binaria
    if (!archivoLlave) {
        perror("Error al crear el archivo de la llave");
        return 1;
    }
    fwrite(llave, 1, sizeof(llave), archivoLlave);
    fclose(archivoLlave);
    printf("Exito! La llave privada se ha guardado en 'llave.key'.\n\n");

    // 4. Preparar buffers para los textos encriptados y desencriptados
    // El texto cifrado puede ser ligeramente mas largo que el original por el "padding" (relleno)
    unsigned char textoEncriptado[longitudTextoOriginal + EVP_MAX_BLOCK_LENGTH];
    int longitudTextoEncriptado = 0;
    int longitudPaso = 0; // Variable auxiliar para guardar longitudes temporales

    // --- PROCESO DE ENCRIPTACION ---
    
    // Crear y establecer el contexto de criptografia
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) manejarErrores();

    // Inicializar la operacion de encriptacion con AES-256-CBC
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, llave, iv))
        manejarErrores();

    // Proporcionar el mensaje que queremos encriptar y obtener la salida cifrada
    if (1 != EVP_EncryptUpdate(ctx, textoEncriptado, &longitudPaso, textoOriginal, longitudTextoOriginal))
        manejarErrores();
    longitudTextoEncriptado = longitudPaso;

    // Finalizar la encriptacion (agrega el padding final si es necesario)
    if (1 != EVP_EncryptFinal_ex(ctx, textoEncriptado + longitudPaso, &longitudPaso))
        manejarErrores();
    longitudTextoEncriptado += longitudPaso;

    // Mostrar el texto encriptado en pantalla en formato Hexadecimal (para que sea legible)
    printf("Texto encriptado (en formato hexadecimal):\n");
    for(int i = 0; i < longitudTextoEncriptado; i++) {
        printf("%02x", textoEncriptado[i]);
    }
    printf("\n\n");

    // --- PROCESO DE DESENCRIPTACION ---
    
    // Buffer para guardar el texto que vamos a recuperar
    unsigned char textoDesencriptado[longitudTextoEncriptado];
    int longitudTextoDesencriptado = 0;

    // Inicializar la operacion de desencriptacion usando la misma llave y el mismo IV
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, llave, iv))
        manejarErrores();

    // Proporcionar el texto cifrado para obtener el texto plano original
    if (1 != EVP_DecryptUpdate(ctx, textoDesencriptado, &longitudPaso, textoEncriptado, longitudTextoEncriptado))
        manejarErrores();
    longitudTextoDesencriptado = longitudPaso;

    // Finalizar la desencriptacion (quita el padding final)
    if (1 != EVP_DecryptFinal_ex(ctx, textoDesencriptado + longitudPaso, &longitudPaso))
        manejarErrores();
    longitudTextoDesencriptado += longitudPaso;

    // Asegurarnos de colocar un caracter nulo al final para imprimirlo como cadena de texto (string) en C
    textoDesencriptado[longitudTextoDesencriptado] = '\0';

    // Mostrar el resultado final al usuario
    printf("Texto desencriptado:\n%s\n", textoDesencriptado);

    // 5. Limpieza de memoria (una excelente practica en C!)
    EVP_CIPHER_CTX_free(ctx);

    return 0;
}