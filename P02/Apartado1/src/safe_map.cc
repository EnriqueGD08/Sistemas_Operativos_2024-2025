#include "safe_map.h"

/**
 * @brief Constructor de la clase SafeMap
 * @param sv Vista de cadena
*/
SafeMap::SafeMap(std::string_view sv) noexcept {
  sv_ = sv;
}

/**
 * @brief Constructor por defecto de la clase SafeMap
*/
SafeMap::SafeMap() noexcept {
  sv_ = "";
}

/**
 * @brief Constructor de movimiento de la clase SafeMap
 * @param other Objeto a mover
*/
SafeMap::SafeMap(SafeMap&& other) noexcept {
  sv_ = other.sv_;
  other.sv_ = "";
}

/**
 * @brief Destructor de la clase SafeMap
*/
SafeMap::~SafeMap() noexcept {
  if (is_valid()) {
    munmap(const_cast<char*>(sv_.data()), sv_.size());
  }
}

/**
 * @brief Comprueba si la vista de cadena es válida
 * @return true si es válido, false en caso contrario
*/
[[nodiscard]] bool SafeMap::is_valid() const noexcept {
  return !sv_.empty();
}

/**
 * @brief Devuelve la vista de cadena
 * @return Vista de cadena
*/
[[nodiscard]] std::string_view SafeMap::get() const noexcept {
  return sv_;
}