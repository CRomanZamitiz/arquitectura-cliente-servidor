# Tema 3: Sockets Internet en lenguaje C

En este tema comenzaremos a trabajar con la arquitectura Cliente-Servidor utilizando Sockets de la familia `AF_INET`. 

A partir de este punto, **necesitaremos ejecutar procesos de servidor y de cliente de forma simultánea** para observar la comunicación en tiempo real y el comportamiento de las colas de red.

---

## 🛠️ Cómo abrir múltiples terminales en GitHub Codespaces

Para probar los programas de este directorio, la terminal de Codespaces (basada en Visual Studio Code) nos permite manejar múltiples consolas al mismo tiempo sin salir del navegador. Tienes dos formas muy prácticas de hacerlo:

### Opción 1: Pantalla dividida (Recomendada para pares 1 a 1)
Ideal para probar el primer par de sockets (ej. `programa04_servidor_internet.c` y su respectivo cliente), ya que te permite ver ambos procesos uno al lado del otro.

1. Abre tu primera terminal normalmente: **`Terminal`**.
2. En la esquina superior derecha del panel de esa terminal, haz clic en el **ícono de panel dividido** (parece un cuadrado partido a la mitad) o presiona las teclas `Ctrl` + `Shift` + `5`.
3. La pantalla de la terminal se dividirá en dos.
4. **Ejemplo de uso:** En la mitad izquierda ejecuta el servidor para que se quede "escuchando" en el puerto 4898, y en la mitad derecha lanza al cliente apuntando a la IP local (`127.0.0.1`).

### Opción 2: Múltiples pestañas (Recomendada para la Tarea 4)
Si necesitas lanzar un servidor y **múltiples clientes concurrentes** (por ejemplo, para probar la saturación del backlog en `listen()`), dividir la pantalla muchas veces puede reducir demasiado el espacio de lectura. En ese caso, es mejor usar pestañas:

1. En el panel de la terminal, en la esquina superior derecha, haz clic en el **botón con el símbolo de más (`+`)**.
2. Se creará una nueva terminal de `bash` completamente independiente.
3. En el lado derecho de tu panel de terminales, aparecerá una lista (`bash`, `bash`, `bash...`). Puedes alternar entre cada una de ellas simplemente haciendo clic, exactamente igual que como cambias de pestañas en tu navegador web.
