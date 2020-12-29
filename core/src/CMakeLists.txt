add_library (
  ${NESDEV_CORE_LIBRARY}
  STATIC
  ${NESDEV_CORE_SOURCES})

add_library (
  Nesdev::core
  ALIAS
  ${NESDEV_CORE_LIBRARY})

target_compile_features (
  ${NESDEV_CORE_LIBRARY}
  PRIVATE
  ${NESDEV_COMPILE_FEATURES})

target_compile_definitions (
  ${NESDEV_CORE_LIBRARY}
  PRIVATE
  ${NESDEV_COMPILE_DEFINITIONS})

target_include_directories (
  ${NESDEV_CORE_LIBRARY}
  PUBLIC
  ${NESDEV_CORE_INCLUDE_PATH}
  PRIVATE
  ${NESDEV_CORE_SOURCE_DIR})
