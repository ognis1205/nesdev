project (nescore)

set (PROJECT_LIBRARY ${PROJECT_NAME})

set (PROJECT_SRC_PATH src)

set (PROJECT_INC_PATH include)

set (PROJECT_COMPILE_FEATURES cxx_std_17)

set (PROJECT_COMPILE_DEFINITIONS "-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2")

file (
  GLOB_RECURSE
  NES_CORE_SRCS
  CONFIGURE_DEPENDS
  ${PROJECT_SRC_PATH}/*.cc ${PROJECT_SRC_PATH}/*.h ${PROJECT_INC_PATH}/*.h)

add_library (
  ${PROJECT_LIBRARY}
  STATIC
  ${NES_CORE_SRCS})

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
  ${PROJECT_INC_PATH}
  PRIVATE
  ${PROJECT_SRC_PATH})

add_subdirectory (tests)

add_test (NAME TestOpcodes COMMAND test_opcodes)

add_test (NAME TestPipeline COMMAND test_pipeline)

add_test (NAME TestTypes COMMAND test_types)
