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
  std::ifstream file;
  std::vector<std::string_view> args(argv + 1, argv + argc);
  parse_args(args, verbose);

  return 0;
}