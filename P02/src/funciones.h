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
#include <vector>
#include <string_view>
#include <expected>
#include <sstream>
#include <fstream>
#include <string>

void parse_args(const std::vector<std::string_view>& args, bool& verbose);
std::expected<std::string_view, int> read_all(const std::string& path);