# Working on Cmake 3.18.
cmake_minimum_required(VERSION 3.18)

# Project Name.
project(nes-emulator)

# Locate All .cc Files in 'src' Directory.
file(GLOB NES_EMULATOR_SRCS src/*.cc)

# Find SDL2 Package.
find_package(SDL2 REQUIRED)

# Add Path to SDL2 Package to Include Path.
include_directories(${SDL2_INCLUDE_DIRS})

# Define Exucutable as 'nes'.
add_executable(nes ${NES_EMULATOR_SRCS})

# Link SDL2 Package.
target_link_libraries(nes ${SDL2_LIBRARIES})
