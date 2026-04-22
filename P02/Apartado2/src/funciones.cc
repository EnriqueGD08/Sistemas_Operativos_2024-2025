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
void parse_args(const std::vector<std::string_view>& args, bool& verbose, std::string& path, std::string& puerto) {
  bool archivo = false;
  if (args.empty()) {
    std::cerr << "Error: No se ha especificado ningún archivo" << std::endl;
    std::cout << "Use -h o --help para más información" << std::endl;
    exit(1);
  }
  for (auto it = args.begin(), end = args.end(); it != end; ++it) {
    if (*it == "-h" || *it == "--help") {
      std::cout << "Ejecucion: docserver [-v | --verbose] [-h | --help]";
      std::cout << " [-p <puerto> | --port <puerto>]";
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
    } else if (*it == "-p" || *it == "--port") {
      it++;
      if (it == end) {
        std::cerr << "Error: Falta el puerto" << std::endl;
        exit(1);
      }
      puerto = std::string(*it);
    } else if (it[0] != "-") {
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
 * @brief Envía una respuesta
 * @param socket Socket
 * @param header Cabecera
 * @param body Cuerpo
 * @return Resultado
*/
int send_response(const SafeFD& socket, std::string_view header, std::string_view body = {}) {
  std::string response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: text/plain\r\n";
  response += header;
  response += "\r\n\r\n";
  response += body;
  //hacer un salto de linea en el mensaje para que se vea bien en el navegador
  response += "\n";

  if (send(socket.get(), response.data(), response.size(), 0) == -1) {
    std::cerr << "Error: No se pudo enviar la respuesta" << std::endl;
    return errno;
  }

  return 0;
}

/**
 * @brief Obtiene una variable de entorno
 * @param name Nombre de la variable
 * @return Valor de la variable
*/
std::string getenv(const std::string& name) {
  char* value = std::getenv(name.c_str());
  if (value == nullptr) {
    return "";
  }
  return std::string(value);
}

/**
 * @brief Crea un socket
 * @param port Puerto
 * @return Socket
*/
std::expected<SafeFD, int> make_socket(uint16_t port) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    std::cerr << "Error: No se pudo crear el socket" << std::endl;
    return std::unexpected(errno);
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
    close(fd);
    return std::unexpected(errno);
  }

  return SafeFD(fd);
}

/**
 * @brief Escucha una conexión
 * @param socket Socket
 * @return Conexión
*/
int listen_connection(const SafeFD& socket) {
  if (listen(socket.get(), 1) == -1) {
    std::cerr << "Error: No se pudo escuchar la conexión" << std::endl;
    return errno;
  }

  return 0;
}

/**
 * @brief Acepta una conexión
 * @param socket Socket
 * @param client_addr Dirección del cliente
 * @return Conexión
*/
std::expected<SafeFD, int> accept_connection(const SafeFD& socket, sockaddr_in& client_addr) {
  socklen_t client_addr_len = sizeof(client_addr);
  int client_fd = accept(socket.get(), reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
  if (client_fd == -1) {
    std::cerr << "Error: No se pudo aceptar la conexión" << std::endl;
    return std::unexpected(errno);
  }

  return SafeFD(client_fd);
}