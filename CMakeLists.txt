cmake_minimum_required (VERSION 3.0)

# Report AppleClang separately from Clang. Their version numbers are different.
# https://cmake.org/cmake/help/v3.0/policy/CMP0025.html
if (POLICY CMP0025)
  cmake_policy (SET CMP0025 NEW)
endif ()

project (
  nesdev
  VERSION
  1.0.0
  DESCRIPTION
  "NesDev is a modular cycle-accurate NES emulator development toolkit for C++."
  HOMEPAGE_URL
  ""
  LANGUAGES CXX C)

set (CMAKE_VERBOSE_MAKEFILE ON)

set (CMAKE_CXX_STANDARD 17)

set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_CXX_EXTENSIONS OFF)

set (NESDEV_COMPILE_FEATURES cxx_std_17)

#TODO: Check if GLIBCXX_DEBUG flag can work together with GTest/GMock.
#set (NESDEV_COMPILE_DEFINITIONS "-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2")
set (NESDEV_COMPILE_DEFINITIONS "-D_FORTIFY_SOURCE=2")

enable_testing ()

option (
  WITH_EXAMPLE
  "When -DWITH_EXAMPLE directive specified to cmake command, NES/SDL2 implementation sub-project will be maked along with the library")

include (cmake/googletest.cmake)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
  set (CLANG 1)
endif ()

#TODO: Find -Wno-type-limits equivalent warning flag for MSVC
if (CMAKE_COMPILER_IS_GNUCXX OR CLANG)
  add_compile_options (-Wall -Wextra -Werror -Wno-type-limits -pedantic)
elseif (MSVC)
  add_compile_options (/W4 /WX)
endif ()

#TODO: Install ASAN and fix this compile options.
#add_compile_options (-fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector)

add_subdirectory (core)

if (WITH_EXAMPLE)
  add_subdirectory (example)
endif (WITH_EXAMPLE)
