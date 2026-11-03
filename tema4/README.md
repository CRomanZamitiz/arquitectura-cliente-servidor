# Práctica de Criptografía: AES-256 en C y Presentación Interactiva

Este repositorio contiene los materiales de apoyo para la sesión práctica de **Arquitectura Cliente-Servidor** y **Redes de Computadoras y Ciberseguridad** en la Facultad de Ingeniería de la UNAM.

## 📁 Contenido del Repositorio
- **`cripto.c`**: Código fuente en C que implementa el cifrado y descifrado simétrico utilizando **AES-256-CBC** a través de la API `EVP` de OpenSSL, incluyendo la generación de entropía segura con `RAND_bytes()` y la gestión de la llave binaria (`llave.key`).
- **`presentacion_cripto_unam.html`**: Presentación interactiva con la teoría básica de Criptografía para la clase.

---

## 🚀 Instrucciones de Compilación y Ejecución (Linux / GCC)

Para compilar y ejecutar el programa en un entorno Linux (como Ubuntu o WSL), asegúrate de tener instaladas las cabeceras y bibliotecas de desarrollo de OpenSSL (`libssl-dev`).

### 1. Compilación
Utiliza el compilador `gcc` enlazando explícitamente la biblioteca `libcrypto`:
```bash
gcc -Wall cripto.c -o cripto -lcrypto
