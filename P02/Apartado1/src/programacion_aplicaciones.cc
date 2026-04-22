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
 * @brief Programa principal
*/

#include "funciones.h"

int main(int argc, char* argv[]) {
  bool verbose = false;
  bool tamanio_minimo = false;
  std::string path;
  std::vector<std::string_view> args(argv + 1, argv + argc);
  parse_args(args, verbose, path, tamanio_minimo);

  auto result = read_all(path);

  if (!result.has_value()) {
    return result.error();
  }
  SafeMap safe_map = std::move(result.value());
  std::string_view contenido_archivo = safe_map.get();

  if (tamanio_minimo && contenido_archivo.size() < 1024) {
    std::cerr << "Error: El archivo es menor de 1024 bytes" << std::endl;
    return 1024;
  }

  std::string header = "Content-Length: ";
  header += std::to_string(contenido_archivo.size());
  send_response(header, contenido_archivo);

  return 0;
}