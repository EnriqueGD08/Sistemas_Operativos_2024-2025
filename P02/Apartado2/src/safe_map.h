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
 * @brief Declaración de la clase SafeMap
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

class SafeMap {
 public:
  explicit SafeMap(std::string_view sv) noexcept;
  explicit SafeMap() noexcept;
  SafeMap(const SafeMap&) = delete;
  SafeMap& operator=(const SafeMap&) = delete;
  SafeMap(SafeMap&& other) noexcept;

  SafeMap& operator=(SafeMap&& other) noexcept {
    if (this != &other) {
      if (!sv_.empty()) {
        munmap(const_cast<char*>(sv_.data()), sv_.size());
      }
      sv_ = other.sv_;
      other.sv_ = "";
    }
    return *this;
  }
  
  ~SafeMap() noexcept;
  [[nodiscard]] bool is_valid() const noexcept;
  [[nodiscard]] std::string_view get() const noexcept;

 private: 
  std::string_view sv_;
};