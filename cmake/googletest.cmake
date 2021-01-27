find_package (Threads REQUIRED)

include (ExternalProject)

set (GTEST_VER 1.10.0)

set_directory_properties (PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/external)

externalproject_add (
  googletest
  URL https://github.com/google/googletest/archive/release-${GTEST_VER}.zip
  UPDATE_COMMAND  ""
  INSTALL_COMMAND "")

externalproject_get_property (googletest SOURCE_DIR)

set (GTEST_INCLUDE_PATH ${SOURCE_DIR}/googletest/include)

set (GMOCK_INCLUDE_PATH ${SOURCE_DIR}/googlemock/include)

externalproject_get_property (googletest BINARY_DIR)

set (GTEST_LIBRARY_PATH ${BINARY_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a)

set (GTEST_LIBRARY GTest::GTest)

add_library (${GTEST_LIBRARY} UNKNOWN IMPORTED)

set_target_properties (
  ${GTEST_LIBRARY}
  PROPERTIES
  IMPORTED_LOCATION ${GTEST_LIBRARY_PATH}
  INTERFACE_LINK_LIBRARIES Threads::Threads)

target_compile_features (
  ${GTEST_LIBRARY}
  INTERFACE
  ${NESDEV_COMPILE_FEATURES})

target_compile_definitions (
  ${GTEST_LIBRARY}
  INTERFACE
  ${NESDEV_COMPILE_DEFINITIONS})

add_dependencies (${GTEST_LIBRARY} googletest)

set (GMOCK_LIBRARY_PATH ${BINARY_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}gmock.a)

set (GMOCK_LIBRARY GTest::GMock)

add_library (${GMOCK_LIBRARY} UNKNOWN IMPORTED)

set_target_properties (
  ${GMOCK_LIBRARY}
  PROPERTIES
  IMPORTED_LOCATION ${GMOCK_LIBRARY_PATH}
  INTERFACE_LINK_LIBRARIES Threads::Threads)

target_compile_features (
  ${GMOCK_LIBRARY}
  INTERFACE
  ${NESDEV_COMPILE_FEATURES})

target_compile_definitions (
  ${GMOCK_LIBRARY}
  INTERFACE
  ${NESDEV_COMPILE_DEFINITIONS})

add_dependencies (${GMOCK_LIBRARY} googletest)
