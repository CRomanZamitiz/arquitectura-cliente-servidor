# Práctica de Laboratorio: Anatomía de un Proceso en Linux

## Objetivo de la Práctica
Comprender la estructura de la imagen de un proceso en memoria (Código, Datos Inicializados, BSS, Montículo y Pila) y diferenciarla del contexto del sistema operativo (Bloque de Control de Proceso). Mediante el uso de comandos de terminal, el estudiante inspeccionará la distribución de memoria de un programa en C en ejecución.

## Requisitos Previos
* Entorno Linux (puede ser mediante GitHub Codespaces, un servidor CentOS por SSH, o WSL en Windows 10).
* Compilador GCC instalado.
* Dos terminales (o sesiones) abiertas de forma concurrente.

## 1. Código Fuente (`servidor.c`)
Este programa en C representa una estructura clásica. Contiene variables que el sistema operativo mapeará directamente a los 5 segmentos de memoria. El programa se pausa intencionalmente para permitir su inspección dinámica por parte de los alumnos.

```c
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int puerto = 80;               /* Segmento Data: Variable global inicializada */
int conexiones_pendientes;     /* Segmento BSS: Variable global no inicializada (inicia en 0) */

int main(int argc, char *argv[]) {
    int socket_fd = 5;         /* Pila (Stack): Variable local */
    char *buffer = malloc(1024); /* Montículo (Heap): Asignación dinámica de memoria */
    
    /* 5. Segmento de Código (Text): Las instrucciones compiladas */
    printf("Proceso en ejecución con PID: %d\n", getpid());
    printf("Ve a tu segunda terminal y ejecuta pmap. Luego presiona Enter aquí para terminar...\n");
    
    /* Pausa la ejecución en espera de entrada estándar */
    getchar(); 
    
    free(buffer);
    return 0;
}
```

## 2. Ejecución e Inspección (Terminal 1)
1. Compila el código fuente asegurando que el entorno genere el binario localmente:
   ```bash
   gcc servidor.c -o servidor
   ```
2. Ejecuta el binario generado:
   ```bash
   ./servidor
   ```
3. El programa se detendrá. **Toma nota del PID** (Identificador de Proceso) que se muestra en pantalla.

## 3. Análisis Dinámico (Terminal 2)
Sin cerrar la Terminal 1, abre una segunda terminal y ejecuta los siguientes comandos. (Sustituye `<PID>` por el número obtenido en el paso anterior).

### A. Análisis Estático con `size`
Muestra el tamaño exacto en bytes de los segmentos estáticos del archivo binario, independientemente de su ejecución.
```bash
size servidor
```
*Identifica la distribución de los segmentos `text`, `data` y `bss`.*

### B. Mapa de Memoria Dinámica con `pmap`
Consulta las estructuras del kernel para imprimir el mapa completo de la memoria virtual del proceso activo.
```bash
pmap <PID>
```
*Localiza las direcciones de memoria donde el sistema ubicó específicamente las etiquetas `[ heap ]` (Montículo) y `[ stack ]` (Pila).*

### C. Metadatos del Kernel en `/proc`
Despliega la información pura del bloque de control del proceso, incluyendo direcciones hexadecimales, permisos (ej. `r-xp` para lectura y ejecución) y rutas de bibliotecas cargadas dinámicamente (`libc`).
```bash
cat /proc/<PID>/maps
```

## 4. Finalización y Limpieza
Regresa a la **Terminal 1** y presiona `Enter`. Esto reanudará la ejecución hacia la instrucción `free(buffer)`, liberando la memoria dinámica solicitada y retornando 0. 
El sistema operativo destruirá la imagen en memoria y eliminará el Bloque de Control de Proceso (PCB).

---

# Identificadores de Proceso: PID y PPID

Este archivo forma parte del material de **Arquitectura Cliente Servidor**. A continuación se explica la teoría detrás de la identificación de procesos en el núcleo (kernel) de Linux, centrándose en el `programa01_print-pid.c`.

## Descripción Teórica

En los sistemas operativos basados en UNIX y GNU/Linux, cada tarea que entra en ejecución se convierte en un proceso. Para llevar el control, el sistema operativo le asigna a cada proceso un número entero único llamado **PID (Process ID)**.

A su vez, los procesos en Linux siguen una estructura jerárquica (un árbol de procesos). Salvo el proceso inicial del sistema (`init` o `systemd` que tiene el PID 1), todos los procesos son creados por otro proceso preexistente. Al proceso creador se le denomina proceso padre. El sistema operativo mantiene un registro del identificador del proceso padre, el cual se conoce como **PPID (Parent Process ID)**.

Comprender esta jerarquía es fundamental antes de abordar la creación explícita de nuevos procesos con la llamada al sistema `fork()`.

## Funciones del Sistema POSIX (Librería `<unistd.h>`)

Para obtener estos identificadores dentro de un programa en C, se utilizan las siguientes llamadas al sistema:

*   `getpid()`: Devuelve el identificador único del proceso que hace la llamada (PID).
*   `getppid()`: Devuelve el identificador del proceso padre (PPID) que invocó al proceso actual.

Ambas funciones retornan un tipo de dato `pid_t`, que comúnmente es equivalente a un entero.

## Instrucciones de Compilación y Ejecución

Si deseas probar el `programa01_print-pid.c` en tu entorno Linux, sigue estos pasos en la terminal:

1.  **Compilar el programa:**
    ```bash
    gcc programa01_print-pid.c -o programa01_print-pid
    ```

2.  **Ejecutar el programa:**
    ```bash
    ./programa01_print-pid
    ```

## Ejercicio Sugerido

Al ejecutar el programa, se sugiere que lo ejecutes varias veces para que observes que en cada ejecución, es un proceso nuevo. También notarás que te imprime el PPID, el cual no cambia. Para comprobar de qué proceso se trata, ejecuta el siguiente comando en la misma terminal donde corriste el programa:

```bash
ps -p <numero_del_ppid>
(Reemplaza <numero_del_ppid> con el valor que devolvió tu programa).

Deberías observar que el proceso padre es, generalmente, la shell de comandos que estás utilizando (por ejemplo, bash o zsh).
```
---

# Creación de Procesos: La llamada al sistema fork()

Este documento explica la teoría detrás de los programas enfocados en la bifurcación asíncrona de procesos, específicamente la serie de códigos que inician con `programa05_fork_v1.c`.

## Descripción Teórica

La llamada al sistema `fork()` es el mecanismo principal en los sistemas POSIX para la creación de nuevos procesos. Cuando un programa en C llama a `fork()`, el sistema operativo realiza una copia exacta del proceso actual (imagen en memoria, variables, descriptores de archivos, etc.). 

A partir de la instrucción `fork()`, existirán dos procesos ejecutándose de forma **concurrente y asíncrona**:
1.  **Proceso Padre:** El proceso original que hizo la llamada.
2.  **Proceso Hijo:** El nuevo proceso creado por el sistema.

### El comportamiento de retorno de fork()

La clave para programar con `fork()` radica en evaluar su valor de retorno dentro de una estructura de control (como un `if/else`), ya que la función devuelve un valor distinto a cada proceso, a pesar de que ambos continúan ejecutando el mismo código a partir del punto de bifurcación:

*   **En el Proceso Padre:** `fork()` devuelve el PID (un número mayor a 0) del proceso hijo recién creado.
*   **En el Proceso Hijo:** `fork()` devuelve exactamente el valor `0`.
*   **Error:** Si el sistema falla al crear el proceso, devuelve `-1`.

## Análisis de los Programas (Serie programa05)

### `programa05_fork_v1.c`
Este código demuestra la ejecución duplicada. Al no evaluar el valor de retorno, tanto el proceso padre como el hijo imprimirán todas las líneas de código posteriores a la bifurcación.

### `programa05_fork_v2.c`
Este código implementa la evaluación condicional. Podrán observar cómo aislar el comportamiento del proceso padre del comportamiento del proceso hijo, lo cual es la base para diseñar servidores concurrentes de red (donde el padre se queda escuchando conexiones y el hijo atiende a los clientes).

## Instrucciones de Compilación y Ejecución

Para compilar y ejecutar, por ejemplo, la versión 2:

1.  **Compilar:**
    ```bash
    gcc programa05_fork_v2.c -o fork_v2
    ```

2.  **Ejecutar:**
    ```bash
    ./fork_v2
    ```

## Notas para el análisis
Debido a la naturaleza asíncrona del planificador de procesos del sistema operativo (Scheduler), no está garantizado qué proceso ejecutará sus líneas de código primero. Si corren el programa varias veces, es posible que en algunas ejecuciones el resultado del padre se imprima antes que el del hijo, y en otras ocasiones ocurra al revés.
