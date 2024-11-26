/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Sistemas Operativos
 * Curso: 2º
 * Práctica 2
 * @autor: Enrique Gómez Díaz
 * @mail: alu0101550329@ull.edu.es
 * @date: 28/11/2024
 * @brief Implementación de las funciones usadas en el programa principal
*/

#include "funciones.h"

void parse_args(const std::vector<std::string_view>& args, bool& verbose) {
  if (args.empty()) {
    std::cerr << "Error: No se ha especificado ningún archivo" << std::endl;
    std::cout << "Use -h o --help para más información" << std::endl;
    exit(1);
  }
  for (auto it = args.begin(), end = args.end(); it != end; ++it) {
    if (*it == "-h" || *it == "--help") {
      std::cout << "Ejecucion: docserver [-v | --verbose] [-h | --help]";
      std::cout << " ARCHIVO" << std::endl;
      std::cout << "Opciones:" << std::endl;
      std::cout << "-v | --verbose:" << std::endl;
      std::cout << "activará el modo detallado. En este ";
      std::cout << "modo el que el programa funciona con normalidad, pero";
      std::cout << ", adicionalmente, muestra mensajes informativos por ";
      std::cout << "la salida de error sobre las funciones de la librería";
      std::cout << " del sistema que va utilizando." << std::endl;
      std::cout << "ARCHIVO:" << std::endl;
      std::cout << "Content-Length: <TAMAÑO>" << std::endl << std::endl;
      std::cout << "<contenido del archivo...>" << std::endl;
      exit(0);
  } else if (*it == "-v" || *it == "--verbose") {
    verbose = true;
  }
  // Procesar otras opciones...
  else if (it[0] != "-") {
    // Añadir a un vector los argumentos adicionales
    // que no son opciones. Por ejemplo, nombres de archivo.
  }
  // ...
}
}