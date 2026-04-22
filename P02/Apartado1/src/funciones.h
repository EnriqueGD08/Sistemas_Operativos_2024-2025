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
 * @brief Declaración de funciones
*/

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <expected>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include "safe.h"
#include "safe_map.h"

void parse_args(const std::vector<std::string_view>& args, bool& verbose, std::string& path, bool& tamanio_minimo);
std::expected<SafeMap, int> read_all(const std::string& path);
void send_response(const std::string& header, const std::string_view& body);