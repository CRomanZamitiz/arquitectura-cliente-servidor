#include <stdio.h>      // Libreria estandar para imprimir en pantalla (printf)
#include <stdlib.h>     // Libreria estandar para utilidades generales
#include <dirent.h>     // Libreria especifica de Linux/POSIX para manejar directorios

int main() {
    // 1. Declaramos un "puntero" al directorio. Esto sera nuestra referencia a la carpeta.
    DIR *carpeta;
    
    // 2. Declaramos una estructura que guardara la informacion de cada archivo que leamos.
    struct dirent *archivo;

    // 3. Abrimos el directorio actual, representado por "."
    carpeta = opendir(".");

    // Verificamos si hubo un error al intentar abrir la carpeta
    if (carpeta == NULL) {
        printf("Error: No se pudo abrir el directorio actual.\n");
        return 1; // Salimos del programa con un codigo de error
    }

    printf("Contenido del directorio actual:\n");
    printf("--------------------------------\n");

    // 4. Leemos el directorio elemento por elemento usando un ciclo while.
    // readdir() nos da el siguiente archivo, y cuando ya no hay mas, devuelve NULL.
    while ((archivo = readdir(carpeta)) != NULL) {
        // archivo->d_name contiene el nombre de texto del archivo o carpeta
        printf("%s\n", archivo->d_name);
    }

    // 5. Cerramos la carpeta para ser ordenados y liberar recursos
    closedir(carpeta);

    return 0; // El programa termino exitosamente
}