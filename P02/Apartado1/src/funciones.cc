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

/**
 * @brief Parsea los argumentos de la línea de comandos
 * @param args Vector de argumentos
 * @param verbose Modo detallado
 * @param file Archivo
*/
void parse_args(const std::vector<std::string_view>& args, bool& verbose, std::string& path, bool& tamanio_minimo) {
  bool archivo = false;
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
    else if (*it == "-w") {
      tamanio_minimo = true;
    }
    else if (it[0] != "-") {
      if (archivo) {
        std::cerr << "Error: No se puede abrir más de un archivo" << std::endl;
        exit(1);
      }
      path = std::string(*it);
      archivo = true;
    }
  } 
  if (!archivo) {
    std::cerr << "Error: No se ha especificado ningún archivo" << std::endl;
    std::cout << "Use -h o --help para más información" << std::endl;
    exit(1);
  }
}

/**
 * @brief Lee el contenido de un archivo
 * @param path Ruta del archivo
 * @param file Archivo
 * @return Contenido del archivo
*/
std::expected<SafeMap, int> read_all(const std::string& path) {
  int fd = open(path.c_str(), O_RDONLY);
  if (fd == -1) {
    return std::unexpected(errno);
  }

  struct stat st;
  if (fstat(fd, &st) == -1) {
    close(fd);
    return std::unexpected(errno);
  }
  size_t length = st.st_size;

  void* mapped_region = mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
  if (mapped_region == MAP_FAILED) {
    close(fd);
    std::cerr << "Error: No se pudo abrir el mapa" << std::endl;
    return std::unexpected(errno);
  }

  close(fd);

  return SafeMap(std::string_view(static_cast<char*>(mapped_region), length));
}

/**
 * @brief Envia una respuesta HTTP
 * @param header Cabecera
 * @param body Cuerpo
*/
void send_response(const std::string& header, const std::string_view& body = {}) {
  std::cout << header << std::endl << std::endl;

  if (!body.empty()) {
      std::cout << body << std::endl;
  }
}
