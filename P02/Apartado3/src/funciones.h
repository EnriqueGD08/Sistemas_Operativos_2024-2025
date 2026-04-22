/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Sistemas Operativos
 * Curso: 2º
 * Práctica 2
 * @autor: Enrique Gómez Díaz
 * @mail: alu0101550329@ull.edu.es
 * @date: 12/12/2024
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
#include <arpa/inet.h>
#include <sstream>

struct exec_environment {
  std::string request_path;
  std::string server_basedir;
  std::string remote_port;
  std::string remote_ip;
};

struct execute_program_error {
  int exit_code;
  int error_code;
};

void parse_args(const std::vector<std::string_view>& args, bool& verbose, std::string& puerto, std::string& directorio, bool& ruta_completa);
std::expected<SafeMap, int> read_all(const std::string& path);
int send_response(const SafeFD& socket, std::string_view header, std::string_view body);
std::string getenv(const std::string& name);
std::expected<SafeFD, int> make_socket(uint16_t port);
int listen_connection(const SafeFD& socket);
std::expected<SafeFD, int> accept_connection(const SafeFD& socket, sockaddr_in& client_addr);
std::expected<std::string, int> receive_request(const SafeFD& socket, size_t max_size);
std::expected<std::string, int> process_request(const std::string& peticion);
std::expected<std::string, execute_program_error> execute_program(const std::string& path, const exec_environment& env);