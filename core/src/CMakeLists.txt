add_library (
  ${PROJECT_LIBRARY}
  STATIC
  ${NES_CORE_SOURCES})

target_compile_features (
  ${PROJECT_LIBRARY}
  PRIVATE
  ${PROJECT_COMPILE_FEATURES})

target_compile_definitions (
  ${PROJECT_LIBRARY}
  PRIVATE
  ${PROJECT_COMPILE_DEFINITIONS})

target_include_directories (
  ${PROJECT_LIBRARY}
  PUBLIC
  ${PROJECT_INCLUDE_PATH}
  PRIVATE
  ${PROJECT_SOURCE_DIR})
