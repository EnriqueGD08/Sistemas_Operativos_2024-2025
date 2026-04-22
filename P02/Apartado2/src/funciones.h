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
#include <netinet/in.h>
#include "safe.h"
#include "safe_map.h"

void parse_args(const std::vector<std::string_view>& args, bool& verbose, std::string& path, std::string& puerto);
std::expected<SafeMap, int> read_all(const std::string& path);
int send_response(const SafeFD& socket, std::string_view header, std::string_view body);
std::string getenv(const std::string& name);
std::expected<SafeFD, int> make_socket(uint16_t port);
int listen_connection(const SafeFD& socket);
std::expected<SafeFD, int> accept_connection(const SafeFD& socket, sockaddr_in& client_addr);