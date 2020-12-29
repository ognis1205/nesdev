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

enable_testing ()

include (cmake/googletest.cmake)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
  set (CLANG 1)
endif ()

if (CMAKE_COMPILER_IS_GNUCXX OR CLANG)
  add_compile_options (-Wall -Wextra -Werror -pedantic)
elseif (MSVC)
  add_compile_options (/W4 /WX)
endif ()

#TODO: Install ASAN and fix this compile options.
#add_compile_options (-fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector)

add_subdirectory (core)
