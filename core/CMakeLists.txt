project (nes-core)

file (
  GLOB_RECURSE
  NES_CORE_SRCS
  CONFIGURE_DEPENDS
  src/*.cc src/*.h include/*.h)

add_library (
  ${PROJECT_NAME}
  STATIC
  ${NES_CORE_SRCS})

target_compile_features (
  ${PROJECT_NAME}
  PRIVATE
  cxx_std_17)

target_compile_definitions (
  ${PROJECT_NAME}
  PRIVATE
  -D_GLIBCXX_DEBUG
  -D_GLIBCXX_DEBUG_PEDANTIC
  -D_FORTIFY_SOURCE=2)

target_include_directories (
  ${PROJECT_NAME}
  PUBLIC
  include
  PRIVATE
  src)

add_subdirectory (test)

add_test (
  NAME
  TestPipeline
  COMMAND
  test_pipeline)

