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
 * Compilar: make en la carpeta Apartado2, y el ejecutable se creará en la carpeta build
 * Para probar el programa, ejecutar el comando curl http://localhost:8080 en otra terminal
 * mientras se ejecuta el programa con ./programacion_aplicaciones -p 8080 archivo.txt
*/

#include "funciones.h"

#include "funciones.h"

int main(int argc, char* argv[]) {
  bool verbose = false;
  std::string path;
  std::string puerto = "8080";
  std::vector<std::string_view> args(argv + 1, argv + argc);
  parse_args(args, verbose, path, puerto);

  auto socket_result = make_socket(std::stoi(puerto));
  if (!socket_result) {
    std::cerr << "Error: No se pudo crear el socket" << std::endl;
    return socket_result.error();
  }
  SafeFD socket = std::move(socket_result.value());

  if (listen_connection(socket) != 0) {
    std::cerr << "Error: No se pudo poner el socket a la escucha" << std::endl;
    return listen_connection(socket);
  }

  while (true) {
    sockaddr_in client_addr;
    auto client_result = accept_connection(socket, client_addr);
    if (!client_result) {
      std::cerr << "Error: No se pudo aceptar la conexión" << std::endl;
      return client_result.error();
    }
    SafeFD valor_socket_cliente = std::move(client_result.value());

    auto resultado_apertura = read_all(path);
    if (!resultado_apertura) {
      std::string mensaje_error = "Error: No se pudo leer el archivo";
      send_response(valor_socket_cliente, "Content-Length: " + std::to_string(mensaje_error.size()), mensaje_error);
      continue;
    }

    SafeMap safe_map = std::move(resultado_apertura.value());
    std::string_view contenido_archivo = safe_map.get();
    std::string header = "Content-Length: " + std::to_string(contenido_archivo.size() + 1);

    if (send_response(valor_socket_cliente, header, contenido_archivo) != 0) {
      std::cerr << "Error: No se pudo enviar la respuesta" << std::endl;
    }
  }

  return 0;
}