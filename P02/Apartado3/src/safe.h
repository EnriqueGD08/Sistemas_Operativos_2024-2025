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
 * @brief Declaración de la clase SafeFD
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

class SafeFD {
 public:
  explicit SafeFD(int fd) noexcept;
  explicit SafeFD() noexcept;
  SafeFD(const SafeFD&) = delete;
  SafeFD& operator=(const SafeFD&) = delete;
  SafeFD(SafeFD&& other) noexcept;

  SafeFD& operator=(SafeFD&& other) noexcept {
    if (this != &other && fd_ != other.fd_) {
      // Cerrar el descriptor de archivo actual
      close(fd_);
      // Mover el descriptor de archivo de 'other' a este objeto
      fd_ = other.fd_;
      other.fd_ = -1;
    }
    return *this;
  }
  
  ~SafeFD() noexcept;
  [[nodiscard]] bool is_valid() const noexcept;
  [[nodiscard]] int get() const noexcept;

 private: 
  int fd_;
};