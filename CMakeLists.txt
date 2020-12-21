cmake_minimum_required (VERSION 3.0)

# Report AppleClang separately from Clang. Their version numbers are different.
# https://cmake.org/cmake/help/v3.0/policy/CMP0025.html
if (POLICY CMP0025)
  cmake_policy (SET CMP0025 NEW)
endif ()

project (nes)

set (NES_VERSION 1.0.0)

set (CMAKE_VERBOSE_MAKEFILE ON)

enable_testing ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
  set (CLANG 1)
endif ()

if (CMAKE_COMPILER_IS_GNUCXX OR CLANG)
  add_compile_options (
    -Wall
    -Wextra
    -Werror
    -pedantic)
elseif (MSVC)
  add_compile_options (
    /W4
    /WX)
endif ()

#TODO: Install ASAN and fix this compile options.
#add_compile_options (
#  -fsanitize=address
#  -fsanitize=undefined
#  -fno-sanitize-recover
#  -fstack-protector)

message (
  NOTICE
  "The following compile features/definitions must be specified in subdirectories:"
  " -std=c++17"
  " -D_GLIBCXX_DEBUG"
  " -D_GLIBCXX_DEBUG_PEDANTIC"
  " -D_FORTIFY_SOURCE=2")

add_subdirectory (core)
