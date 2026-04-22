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
 * @brief Implementacion de la clase SafeFD
*/

#include "safe.h"

/**
 * @brief Constructor de la clase SafeFD
 * @param fd Descriptor de archivo
*/
SafeFD::SafeFD(int fd) noexcept {
  fd_ = fd;
}

/**
 * @brief Constructor por defecto de la clase SafeFD
*/
SafeFD::SafeFD() noexcept {
  fd_ = -1;
}

/**
 * @brief Constructor de movimiento de la clase SafeFD
 * @param other Objeto a mover
*/
SafeFD::SafeFD(SafeFD&& other) noexcept {
  fd_ = other.fd_;
  other.fd_ = -1;
}

/**
 * @brief Destructor de la clase SafeFD
*/
SafeFD::~SafeFD() noexcept {
  if (is_valid()) {
    close(fd_);
  }
}

/**
 * @brief Comprueba si el descriptor de archivo es válido
 * @return true si es válido, false en caso contrario
*/
[[nodiscard]] bool SafeFD::is_valid() const noexcept {
  return fd_ >= 0;
}

/**
 * @brief Devuelve el descriptor de archivo
 * @return Descriptor de archivo
*/
[[nodiscard]] int SafeFD::get() const noexcept {
  return fd_;
}