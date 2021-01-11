/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_EXCEPTIONS_H_
#define _NESDEV_CORE_EXCEPTIONS_H_
#include <exception>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

// TODO: Implement error codes.
class Exception : public std::exception {
 public:
  const char* what() const noexcept override { return what_arg_.what(); }

 protected:
  static std::string Header(const std::string& name) {
    return "[nesdev::core::" + name + "]";
  }

  Exception(const char* what_arg) : what_arg_{what_arg} {}

 private:
  std::runtime_error what_arg_;
};

class InvalidAddress : public Exception {
 public:
  static InvalidAddress Occur(const std::string& what_arg, const Address& address) {
    std::stringstream ss;
    ss << "[0x"
       << std::uppercase
       << std::setfill('0')
       << std::setw(4)
       << std::hex
       << unsigned(address)
       << "]";
    return InvalidAddress((Exception::Header("InvalidAddress") + " " + what_arg + " " + ss.str()).c_str());
  }

 private:
  InvalidAddress(const char* what_arg) : Exception(what_arg) {}
};

class InvalidOpcode : public Exception {
 public:
  static InvalidOpcode Occur(const std::string& what_arg, const Byte& byte) {
    std::stringstream ss;
    ss << "[0x"
       << std::uppercase
       << std::setfill('0')
       << std::setw(2)
       << std::hex
       << unsigned(byte)
       << "]";
    return InvalidOpcode((Exception::Header("InvalidOpcode") + " " + what_arg + " " + ss.str()).c_str());
  }

 private:
  InvalidOpcode(const char* what_arg) : Exception(what_arg) {}
};

class InvalidHeader : public Exception {
 public:
  static InvalidHeader Occur(const std::string& what_arg) {
    return InvalidHeader((Exception::Header("InvalidHeader") + " " + what_arg).c_str());
  }

 private:
  InvalidHeader(const char* what_arg) : Exception(what_arg) {}
};

class NotImplemented : public Exception {
 public:
  static NotImplemented Occur(const std::string& what_arg, const Byte& byte) {
    std::stringstream ss;
    ss << "[0x"
       << std::uppercase
       << std::setfill('0')
       << std::setw(2)
       << std::hex
       << unsigned(byte)
       << "]";
    return NotImplemented((Exception::Header("NotImplemented") + " " + what_arg + " " + ss.str()).c_str());
  }

 private:
  NotImplemented(const char* what_arg) : Exception(what_arg) {}
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_EXCEPTIONS_H_
