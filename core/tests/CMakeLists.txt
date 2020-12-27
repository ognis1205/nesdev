find_package (Boost COMPONENTS system filesystem unit_test_framework REQUIRED)

# Testsuite for include/nes/cpre/opcodes.h src/opcodes.cc
set (PIPELINE_TEST_NAME test_opcodes)

add_executable (
  ${PIPELINE_TEST_NAME}
  ${PIPELINE_TEST_NAME}.cc)

target_compile_features (
  ${PIPELINE_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_FEATURES})

target_compile_definitions (
  ${PIPELINE_TEST_NAME}
  PRIVATE
  NES_CORE_TESTS_PRIVATE
  ${PROJECT_COMPILE_DEFINITIONS})

target_include_directories(
  ${PIPELINE_TEST_NAME}
  PRIVATE
  ../${PROJECT_SRC_PATH}
  ../${PROJECT_INC_PATH}
  ${Boost_INCLUDE_DIRS})

target_link_libraries(
  ${PIPELINE_TEST_NAME}
  ${PROJECT_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

# Testsuite for src/dettail/memory_bank.h src/detail/memory_bank.cc
set (MEMORY_BANK_TEST_NAME test_memory_bank)

add_executable (
  ${MEMORY_BANK_TEST_NAME}
  ${MEMORY_BANK_TEST_NAME}.cc)

target_compile_features (
  ${MEMORY_BANK_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_FEATURES})

target_compile_definitions (
  ${MEMORY_BANK_TEST_NAME}
  PRIVATE
  NES_CORE_TESTS_PRIVATE
  ${PROJECT_COMPILE_DEFINITIONS})

target_include_directories(
  ${MEMORY_BANK_TEST_NAME}
  PRIVATE
  ../${PROJECT_SRC_PATH}
  ../${PROJECT_INC_PATH}
  ${Boost_INCLUDE_DIRS})

target_link_libraries(
  ${MEMORY_BANK_TEST_NAME}
  ${PROJECT_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

# Testsuite for src/dettail/pipeline.h src/detail/pipeline.cc
set (PIPELINE_TEST_NAME test_pipeline)

add_executable (
  ${PIPELINE_TEST_NAME}
  ${PIPELINE_TEST_NAME}.cc)

target_compile_features (
  ${PIPELINE_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_FEATURES})

target_compile_definitions (
  ${PIPELINE_TEST_NAME}
  PRIVATE
  NES_CORE_TESTS_PRIVATE
  ${PROJECT_COMPILE_DEFINITIONS})

target_include_directories(
  ${PIPELINE_TEST_NAME}
  PRIVATE
  ../${PROJECT_SRC_PATH}
  ../${PROJECT_INC_PATH}
  ${Boost_INCLUDE_DIRS})

target_link_libraries(
  ${PIPELINE_TEST_NAME}
  ${PROJECT_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

# Testsuite for include/nes/core/types.h
set (TYPES_TEST_NAME test_types)

add_executable (
  ${TYPES_TEST_NAME}
  ${TYPES_TEST_NAME}.cc)

target_compile_features (
  ${TYPES_TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_FEATURES})

target_compile_definitions (
  ${TYPES_TEST_NAME}
  PRIVATE
  NES_CORE_TESTS_PRIVATE
  ${PROJECT_COMPILE_DEFINITIONS})

target_include_directories(
  ${TYPES_TEST_NAME}
  PRIVATE
  ../${PROJECT_SRC_PATH}
  ../${PROJECT_INC_PATH}
  ${Boost_INCLUDE_DIRS})

target_link_libraries(
  ${TYPES_TEST_NAME}
  ${PROJECT_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
