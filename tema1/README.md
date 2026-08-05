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
    gcc programa01_print-pid.c -o print_pid
    ```

2.  **Ejecutar el programa:**
    ```bash
    ./print_pid
    ```

## Ejercicio Sugerido

Al ejecutar el programa, notarás que te imprime el PPID. Para comprobar de qué proceso se trata, ejecuta el siguiente comando en la misma terminal donde corriste el programa:

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
