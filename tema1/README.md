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

## Ejercicio Sugerido para Alumnos

Al ejecutar el programa, notarás que te imprime el PPID. Para comprobar de qué proceso se trata, ejecuta el siguiente comando en la misma terminal donde corriste el programa:

```bash
ps -p <numero_del_ppid>
