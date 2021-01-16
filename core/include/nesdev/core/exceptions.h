/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_EXCEPTIONS_H_
#define _NESDEV_CORE_EXCEPTIONS_H_
#include <cassert>
#include <exception>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include "nesdev/core/macros.h"
#include "nesdev/core/types.h"

namespace nesdev {
namespace core {

// TODO: Implement error codes.
class Exception : public std::exception {
 public:
  const char* what() const noexcept override { return what_arg_.what(); }

 NESDEV_CORE_PROTECTED_UNLESS_TESTED:
  static std::string Header(const std::string& name) {
    return "[nesdev::core::" + name + "]";
  }

  Exception(const char* what_arg) : what_arg_{what_arg} {}

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  std::runtime_error what_arg_;
};

class AssertionFailed : public Exception {
 public:
  static AssertionFailed Occur(const std::string& what_arg) {
    return AssertionFailed((Exception::Header("AssertionFalied") + " " + what_arg).c_str());
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  AssertionFailed(const char* what_arg) : Exception(what_arg) {}
};

class Assertion {
 public:
  static void CAssert(bool expr, const char* msg) {
    if (!expr) NESDEV_CORE_THROW(AssertionFailed::Occur(msg));
  }
};

#if !defined(NESDEV_CORE_CASSERT)
#  define NESDEV_CORE_CASSERT(expr, msg) nesdev::core::Assertion::CAssert(expr, msg)
#endif

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

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
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

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  InvalidOpcode(const char* what_arg) : Exception(what_arg) {}
};

class InvalidROM : public Exception {
 public:
  static InvalidROM Occur(const std::string& what_arg) {
    return InvalidROM((Exception::Header("InvalidROM") + " " + what_arg).c_str());
  }

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  InvalidROM(const char* what_arg) : Exception(what_arg) {}
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

 NESDEV_CORE_PRIVATE_UNLESS_TESTED:
  NotImplemented(const char* what_arg) : Exception(what_arg) {}
};

}  // namespace core
}  // namespace nesdev
#endif  // ifndef _NESDEV_CORE_EXCEPTIONS_H_
