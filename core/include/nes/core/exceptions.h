/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_EXCEPTIONS_H_
#define _NES_CORE_EXCEPTIONS_H_
#include <exception>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include "nes/core/types.h"

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
  static InvalidAddress Occur(const std::string& what_arg, const Address& address) {
    std::stringstream ss;
    ss << "[0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << address << "]";
    return InvalidAddress((Exception::Header("InvalidAddress") + " " + what_arg + " " + ss.str()).c_str());
  }

 private:
  InvalidAddress(const char* what_arg) : Exception(what_arg) {}
};

class NotImplemented : public Exception {
 public:
  static NotImplemented Occur(const std::string& what_arg, const Byte& byte) {
    std::stringstream ss;
    ss << "[0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << byte << "]";
    return NotImplemented((Exception::Header("NotImplemented") + " " + what_arg + " " + ss.str()).c_str());
  }

 private:
  NotImplemented(const char* what_arg) : Exception(what_arg) {}
};

}  // namespace core
}  // namespace nes
#endif  // ifndef _NES_CORE_EXCEPTIONS_H_
