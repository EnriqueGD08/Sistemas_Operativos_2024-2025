# Sistemas Operativos 2024-2025

Repositorio con las prácticas de la asignatura de Sistemas Operativos del Grado en Ingeniería Informática.

## Práctica 1

La primera práctica está formada por varios scripts en Bash orientados al análisis y filtrado de procesos del sistema.

### Apartado 1

Script que muestra una tabla de procesos a partir de la información de ps, con opciones para:

- mostrar la ayuda;
- incluir los procesos con SID 0;
- filtrar por usuario efectivo.

### Apartado 2

Amplía el apartado anterior para añadir más criterios de filtrado, como:

- varios usuarios separados por comas;
- procesos que tengan archivos abiertos en un directorio concreto;
- exclusión de procesos sin terminal;
- agrupación de la salida por sesiones.

### Apartado 3

Versión más completa del trabajo sobre procesos y sesiones, con nuevas opciones de ordenación y tratamiento de la salida:

- filtrado por usuario o directorio;
- listado de procesos o sesiones;
- ordenación por usuario, procesos o sesiones;
- opción de invertir el orden.

## Práctica 2

La segunda práctica desarrolla un servidor en C++23 usando sockets, acceso seguro a ficheros y utilidades auxiliares para gestionar recursos del sistema.

### Apartado 1

Programa que lee un archivo y devuelve su contenido con una cabecera Content-Length. En este apartado se trabaja con:

- lectura segura del fichero;
- gestión de errores mediante códigos de retorno;
- envío del contenido por salida estándar.

### Apartado 2

Servidor TCP que atiende conexiones en un puerto concreto y responde con el contenido de un archivo solicitado al iniciar el programa.

### Apartado 3

Evolución del servidor anterior para funcionar como un pequeño servidor web documental. Permite:

- procesar peticiones recibidas por red;
- servir archivos estáticos dentro de un directorio base;
- ejecutar programas situados en la ruta /bin/ con variables de entorno específicas;
- devolver respuestas HTTP ante errores de petición o ejecución.

## Estructura

- P01: scripts en Bash de la práctica 1.
- P02/Apartado1: primera versión del programa en C++.
- P02/Apartado2: servidor TCP básico.
- P02/Apartado3: servidor documental con ejecución de programas.
