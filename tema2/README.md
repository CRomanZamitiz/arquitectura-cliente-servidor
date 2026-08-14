# Tema 2: Estados de un Proceso

Este documento es su guía de laboratorio interactivo. Dado que las sesiones teóricas se desarrollan en el aula, esta guía les permitirá poner en práctica los conceptos desde sus propios dispositivos. Podrán probar los comandos en tiempo real y observar directamente cómo el sistema operativo administra el ciclo de vida de los procesos en la terminal.

---

## 🎯 Objetivo Práctico

Aterrizar la teoría sobre el ciclo de vida de un proceso (Nuevo, Listo, En Ejecución, Bloqueado, Terminado) visualizando y administrando estos estados dentro del entorno Linux (Ubuntu) proporcionado por GitHub Codespaces.

---

## 🛠️ Comandos de Monitoreo

Para "espiar" el comportamiento del sistema operativo, utilizaremos las siguientes herramientas en la terminal:

* **`ps`**: Toma una "fotografía" del estado actual de los procesos. Prueben ejecutar `ps -l` (para ver el formato largo con detalles) o `ps aux` (para listar absolutamente todos los procesos del sistema).
* **`top`**: Muestra una lista dinámica y en tiempo real de los procesos en ejecución, funcionando de manera muy similar al Administrador de Tareas de Windows. Presionen la tecla `q` para salir de esta vista.

---

## 🧪 Guía de Experimentación (El Laboratorio)

Abran la terminal de su Codespace y sigan estos pasos uno por uno para provocar cambios de estado de manera controlada:

1. **Crear un proceso largo:** Escriban el comando `sleep 300` y presionen *Enter*. Esto le indica al sistema que "duerma" durante 5 minutos. Notarán que la terminal se queda esperando y no les permite ingresar nuevos comandos.
2. **Suspender el proceso:** Presionen la combinación de teclas `Ctrl` + `Z`. Verán un mensaje en pantalla indicando que el proceso ha sido *Detenido (Stopped)*. El proceso acaba de pasar a un estado suspendido y la terminal vuelve a estar libre para ustedes.
3. **Verificar el estado:** Escriban `ps -l`. Busquen su proceso `sleep` en la lista y observen qué letra aparece en la columna `S` o `STAT`.
4. **Reactivar en segundo plano:** Escriban el comando `bg` (background). Esto hace que el proceso vuelva a estar "En Ejecución", pero trabajando en el fondo sin bloquearles el uso de la terminal.
5. **Terminar un proceso:** Si desean finalizar (matar) el proceso antes de que se cumplan los 5 minutos, busquen el PID (Process ID) en la primera columna que les arrojó el comando `ps` y escriban `kill -9 <PID>` (sustituyendo `<PID>` por el número correspondiente).

---

## 📖 Glosario de Salidas (Interpretación)

Al ejecutar el comando `ps -l`, la columna **S** (State) les arrojará una letra. Utilicen esta lista para interpretar en qué estado se encuentra cada proceso:

* **R (Running / Runnable):** El proceso se está ejecutando actualmente o está listo en la fila de espera para utilizar el procesador.
* **S (Interruptible Sleep):** El proceso está durmiendo o bloqueado, generalmente esperando a que ocurra un evento o esperando una entrada de información (como en el caso de nuestro comando `sleep`).
* **T (Stopped):** El proceso fue detenido temporalmente por la intervención del usuario o por una señal del sistema (como cuando presionamos `Ctrl` + `Z`).
* **Z (Zombie):** Un proceso que ya terminó su ejecución de forma definitiva, pero cuyo proceso "padre" aún no ha leído su estado de salida ni ha limpiado su registro de la tabla del sistema.
