/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_EXCEPTIONS_H_
#define _NES_CORE_EXCEPTIONS_H_
#include <exception>
#include <stdexcept>
#include <string>

namespace nes {
namespace core {

class Exception : public std::exception {
 public:
  const char* what() const noexcept override {
    return m.what();
  }

  const int id;

 protected:
  Exception(int id_, const char* what_arg) : id{id_}, m{what_arg} {}

  static std::string name(const std::string& ename, int id_) {
    return "[json.exception." + ename + "." + std::to_string(id_) + "] ";
  }

 private:
  std::runtime_error m;
};

}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_EXCEPTIONS_H_
