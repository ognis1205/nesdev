file (GLOB NESDEV_CORE_TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

set (NESDEV_CORE_TEST_INCLUDE_PATH ${NESDEV_CORE_SOURCE_DIR})

set (NESDEV_CORE_TEST_EXECUTER_NAME "RunAllUnitTests")

# GoogleTest includes source codes which violates 'comparison between signed and unsigned integer expressions'
# warning. To ignore these warning, add -Wno-sign-compare option.
if (CMAKE_COMPILER_IS_GNUCXX OR CLANG)
  add_compile_options (-Wno-sign-compare)
elseif (MSVC)
  add_compile_options (/wd4018)
endif ()

add_executable(
  ${NESDEV_CORE_TEST_EXECUTER_NAME}
  ${NESDEV_CORE_TEST_SOURCES})

target_compile_features (
  ${NESDEV_CORE_TEST_EXECUTER_NAME}
  PRIVATE
  ${NESDEV_COMPILE_FEATURES})

target_compile_definitions (
  ${NESDEV_CORE_TEST_EXECUTER_NAME}
  PRIVATE
  NESDEV_CORE_TEST
  ${NESDEV_COMPILE_DEFINITIONS})

target_include_directories(
  ${NESDEV_CORE_TEST_EXECUTER_NAME}
  PUBLIC
  ${GTEST_INCLUDE_PATH}
  ${GMOCK_INCLUDE_PATH}
  ${NESDEV_CORE_INCLUDE_PATH}
  ${NESDEV_CORE_TEST_INCLUDE_PATH})

target_link_libraries(
  ${NESDEV_CORE_TEST_EXECUTER_NAME}
  ${NESDEV_CORE_LIBRARY}
  GTest::GTest
  GTest::GMock)

add_dependencies(
  ${NESDEV_CORE_TEST_EXECUTER_NAME}
  googletest)
