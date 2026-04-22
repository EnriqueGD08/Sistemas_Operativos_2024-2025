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
 * @brief Implementación de las funciones usadas en el programa principal
*/

#include "funciones.h"

/**
 * @brief Parsea los argumentos de la línea de comandos
 * @param args Vector de argumentos
 * @param verbose Modo detallado
 * @param file Archivo
*/
void parse_args(const std::vector<std::string_view>& args, bool& verbose, std::string& puerto, std::string& directorio, bool& ruta_completa) {

  for (auto it = args.begin(); it != args.end(); it++) {
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
      if (it == args.end()) {
        std::cerr << "Error: Falta el puerto" << std::endl;
        exit(1);
      }
      puerto = std::string(*it);
    } else if (*it == "-b" || *it == "--base") {
      it++;
      if (it == args.end()) {
        std::cerr << "Error: Falta el directorio base" << std::endl;
        exit(1);
      }
      directorio = std::string(*it);
    } //else if (*it == "-x") {
    //   ruta_completa = true;
    // }
    else if (it->front() != '-') {
      std::cerr << "Error: Argumento no reconocido" << std::endl;
      exit(1);
    }
  }

  if (directorio.empty()) {
    const char* directorio_aux = std::getenv("DOCSERVER_BASEDIR");
    if (directorio_aux) {
        directorio = std::string(directorio_aux);
    } else {
      char cwd[1024];
      if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        directorio = std::string(cwd);
      } else {
          std::cerr << "Error: No se pudo obtener el directorio actual de trabajo" << std::endl;
          exit(1);
      }
    }
  }
}

/**
 * @brief Lee el contenido de un archivo
 * @param path Ruta del archivo
 * @param file Archivo
 * @return Contenido del archivo
*/
std::expected<SafeMap, int> read_all(const std::string& path) {
  std::cout << "Leyendo archivo: " << path << std::endl;
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

/**
 * @brief Recibe una petición
 * @param socket Socket
 * @param max_size Tamaño máximo
 * @return Petición
*/
std::expected<std::string, int> receive_request(const SafeFD& socket, size_t max_size) {
  std::string peticion(max_size, '\0');
  ssize_t bytes_received = recv(socket.get(), &peticion[0], max_size, 0);
  if (bytes_received == -1) {
    std::cerr << "Error: No se pudo recibir la petición" << std::endl;
    return std::unexpected(errno);
  }
  peticion.resize(bytes_received);
  return peticion;
}

/**
 * @brief Procesa una petición
 * @param peticion Petición
 * @return Resultado
*/
std::expected<std::string, int> process_request(const std::string& peticion) {
  if (peticion.empty()) {
      std::cerr << "Error: Petición vacía" << std::endl;
      return std::unexpected(1);
  }

  std::istringstream iss(peticion);
  std::string method, file_path;

  iss >> method >> file_path;

  if (method != "GET") {
    std::cerr << "Error: Método no soportado" << std::endl;
    return std::unexpected(2);
  }

  if (file_path.empty() || file_path[0] != '/') {
    std::cerr << "Error: Ruta del archivo no válida" << std::endl;
    return std::unexpected(3);
  }

  return file_path;
}

/**
 * @brief Ejecuta un programa
 * @param path Ruta del programa
 * @param env Entorno de ejecución
 * @return Resultado
*/
std::expected<std::string, execute_program_error> execute_program(const std::string& path, const exec_environment& env) {
  std::cout << "Ejecutando programa: " << path << std::endl;
    if (access(path.c_str(), X_OK) != 0) {
        return std::unexpected(execute_program_error{0, errno});
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        return std::unexpected(execute_program_error{0, errno});
    }

    pid_t pid = fork();
    if (pid == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        return std::unexpected(execute_program_error{0, errno});
    }

    if (pid == 0) { // Child process
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        setenv("REQUEST_PATH", env.request_path.c_str(), 1);
        setenv("SERVER_BASEDIR", env.server_basedir.c_str(), 1);
        setenv("REMOTE_PORT", env.remote_port.c_str(), 1);
        setenv("REMOTE_IP", env.remote_ip.c_str(), 1);

        execl(path.c_str(), path.c_str(), nullptr);
        _exit(127); // If exec fails
    } else { // Parent process
        close(pipefd[1]);
        std::string output;
        char buffer[128];
        ssize_t count;
        while ((count = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, count);
        }
        close(pipefd[0]);

        int estado;
        waitpid(pid, &estado, 0);
        if (WIFEXITED(estado) && WEXITSTATUS(estado) == 0) {
            return output;
        } else {
            return std::unexpected(execute_program_error{WEXITSTATUS(estado), 0});
        }
    }
}