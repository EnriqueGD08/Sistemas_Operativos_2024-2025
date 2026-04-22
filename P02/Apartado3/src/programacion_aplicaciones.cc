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
 * @brief Programa principal
 * Compilar: make en la carpeta Apartado3, y el ejecutable se creará en la carpeta build
 * Para probar el programa, ejecutar el comando socat STDIO TCP:127.0.0.1:8080 en otra terminal y se puede probar GET /archivo.txt  y  GET /bin/time
 * mientras se ejecuta el programa con ./docserver -p 8080
*/

#include "funciones.h"

int main(int argc, char* argv[]) {
  bool verbose = false;
  bool ruta_completa = false;
  std::string puerto = "8080";
  std::string directorio;
  std::vector<std::string_view> args(argv + 1, argv + argc);
  parse_args(args, verbose, puerto, directorio, ruta_completa);

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
    sockaddr_in direccion_cliente;
    auto resultado_cliente = accept_connection(socket, direccion_cliente);
    if (!resultado_cliente) {
      std::cerr << "Error: No se pudo aceptar la conexión" << std::endl;
      return resultado_cliente.error();
    }
    SafeFD valor_socket_cliente = std::move(resultado_cliente.value());

    auto resultado_peticion = receive_request(valor_socket_cliente, 1024);
    if (!resultado_peticion) {
      if (resultado_peticion.error() == ECONNRESET) {
        std::cerr << "Error: Conexión reiniciada por el cliente" << std::endl;
      } else {
        std::cerr << "Error: No se pudo recibir la petición" << std::endl;
        return resultado_peticion.error();
      }
      continue;
    }

    auto resultado_directorio = process_request(resultado_peticion.value());
    if (!resultado_directorio) {
      std::string mensaje_error = "Error: Petición no válida";
      send_response(valor_socket_cliente, "Content-Length: " + std::to_string(mensaje_error.size()), mensaje_error);
      continue;
    }

    std::string file_path = resultado_directorio.value();
    if (file_path.starts_with("/bin/")) {
      exec_environment env = {
        .request_path = file_path,
        .server_basedir = directorio,
        .remote_port = std::to_string(ntohs(direccion_cliente.sin_port)),
        .remote_ip = inet_ntoa(direccion_cliente.sin_addr)
      };

      auto resultado_ejecucion = execute_program(directorio + file_path, env);
      if (!resultado_ejecucion) {
        std::string mensaje_error = "500 Internal Server Error";
        send_response(valor_socket_cliente, "Content-Length: " + std::to_string(mensaje_error.size()), mensaje_error);
        continue;
      }

      std::string_view contenido_programa = resultado_ejecucion.value();
      std::string header = "Content-Length: " + std::to_string(contenido_programa.size() + 1);
      if (send_response(valor_socket_cliente, header, contenido_programa) != 0) {
        std::cerr << "Error: No se pudo enviar la respuesta" << std::endl;
      }
    } else {
      auto resultado_apertura = read_all(directorio + file_path);
      if (!resultado_apertura) {
        std::string mensaje_error = "Error: No se pudo leer el archivo";
        send_response(valor_socket_cliente, "Content-Length: " + std::to_string(mensaje_error.size()), mensaje_error);
        continue;
      }

      SafeMap safe_map = std::move(resultado_apertura.value());
      std::string_view contenido_archivo = safe_map.get();
      std::string header = "Content-Length: " + std::to_string(contenido_archivo.size() + 1);
      // //ruta completa
      // if (ruta_completa) {
      //   header += ("\n" + directorio + file_path);
      // }
      if (send_response(valor_socket_cliente, header, contenido_archivo) != 0) {
        std::cerr << "Error: No se pudo enviar la respuesta" << std::endl;
      }
    }
  }

  return 0;
}