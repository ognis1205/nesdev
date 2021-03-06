find_package (
  SDL2
  REQUIRED)

find_package (
  SDL2_ttf
  REQUIRED)

file (
  GLOB_RECURSE
  NES_SOURCES
  CONFIGURE_DEPENDS
  src/*.cc
  src/*.h)

set (NES nes)

set (NES_TEST_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/data)

add_executable (
  ${NES}
  ${NES_SOURCES})

target_include_directories (
  ${NES}
  PRIVATE
  ${NESDEV_CORE_INCLUDE_PATH}
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_TTF_INCLUDE_DIRS})

target_link_libraries (
  ${NES}
  PRIVATE
  Nesdev::core
  ${SDL2_LIBRARIES}
  ${SDL2_TTF_LIBRARIES})

target_compile_features (
  ${NES}
  PRIVATE
  cxx_std_17)

set_target_properties (
  ${NES}
  PROPERTIES
  CXX_STANDARD          17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS        NO)

add_custom_command (
  TARGET
  ${NES}
  POST_BUILD
  COMMAND
  ${CMAKE_COMMAND} -E copy_directory ${NES_TEST_RESOURCES} $<TARGET_FILE_DIR:${NES}>/data)
