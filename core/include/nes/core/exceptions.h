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

// TODO: Implement error codes.
class Exception : public std::exception {
 public:
  const char* what() const noexcept override { return what_arg_.what(); }

 protected:
  static std::string Header(const std::string& name) {
    return "[nes::core::" + name + "]";
  }

  Exception(const char* what_arg) : what_arg_{what_arg} {}

 private:
  std::runtime_error what_arg_;
};

class InvalidAddress : public Exception {
 public:
  static InvalidAddress Occur(const std::string& what_arg) {
    return InvalidAddress((Exception::Header("InvalidAddress") + " " + what_arg).c_str());
  }

 private:
  InvalidAddress(const char* what_arg) : Exception(what_arg) {}
};

class NotImplemented : public Exception {
 public:
  static NotImplemented Occur(const std::string& what_arg) {
    return NotImplemented((Exception::Header("NotImplemented") + " " + what_arg).c_str());
  }

 private:
  NotImplemented(const char* what_arg) : Exception(what_arg) {}
};

}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_EXCEPTIONS_H_
