# Arquitectura Cliente Servidor

Bienvenidas y bienvenidos al repositorio oficial de la asignatura. Aquí encontrarán el código fuente de los programas en C y Java analizados en clase, organizados por unidades.

🌐 **Para consultar los temarios, apuntes en PDF y la bibliografía, visita el [Sitio Web de la Asignatura](https://cromanzamitiz.github.io/arquitectura-cliente-servidor/).**

---

## 💻 Laboratorio Virtual: Compilación y Ejecución en la Nube

Para nuestras prácticas, utilizaremos **GitHub Codespaces**. Esta herramienta les proporciona un entorno Linux (Ubuntu) real, directamente en su navegador web, sin necesidad de instalar máquinas virtuales, ni configurar compiladores locales o con problemas de compatibilidad en sus computadoras.

A través de esta terminal virtual, podrán compilar código en C, interactuar con el planificador de procesos del sistema operativo y probar la comunicación entre sockets de red.

### Pasos para iniciar tu entorno de laboratorio

**1. Lanzar Codespaces**
* En la parte superior de este repositorio, haz clic en el botón verde **`<> Code`**.
* Selecciona la pestaña **`Codespaces`**.
* Haz clic en el botón **`Create codespace on main`**.
* Se abrirá una nueva pestaña en tu navegador. El entorno tardará un par de minutos en configurarse la primera vez.
* Podría mostrarse una ventana emergente que les preguntara si confían en el repositorio. Deben seleccionar el botón "Trust Folder & Continue".

**2. Abrir la Terminal de Linux**
Una vez que cargue la interfaz (similar a Visual Studio Code), en la parte inferior se muestra **`Terminal`**.
* Verás aparecer una consola en la parte inferior de la pantalla con el prompt de comandos. A partir de este momento ya estás dentro de una máquina Linux.
* Por default, la terminal tiene como directorio actual de trabajo, el directorio raíz del repositorio arquitectura-cliente-servidor.

**3. Compilar y Ejecutar un Programa en C (Ejemplo)**
Vamos a compilar nuestro primer programa del Tema 1. Escribe los siguientes comandos en la terminal, presionando `Enter` después de cada uno:

*Paso A: Navegar a la carpeta del tema. Para este ejemplo será tema1*
```bash
cd tema1
```

*Paso B: Compilar el código fuente usando gcc*
```bash
gcc programa01_print-pid.c -o prueba_pid
```

*Paso C: Ejecutar el programa generado en la compilación del paso B*
```bash
./prueba_pid
```

Observarás la salida del programa directamente en tu terminal.

:warning: **Consideraciones Importantes!**

* Privilegios de Superusuario: En las prácticas avanzadas donde necesitemos manipular paquetes de red o Raw Sockets, este entorno les permite usar el comando sudo sin restricciones de contraseña.
* Tiempo de inactividad: Si cierran la pestaña, su Codespace se suspenderá automáticamente para ahorrar recursos. Pueden volver a iniciarlo en cualquier momento desde el mismo botón verde **`<> Code`** de GitHub.
