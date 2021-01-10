project (nesdev-core)

set (NESDEV_CORE_LIBRARY ${PROJECT_NAME})

set (NESDEV_CORE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set (NESDEV_CORE_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

file (
  GLOB_RECURSE
  NESDEV_CORE_SOURCES
  CONFIGURE_DEPENDS
  ${NESDEV_CORE_SOURCE_DIR}/*.cc ${NESDEV_CORE_SOURCE_DIR}/*.h ${NESDEV_CORE_INCLUDE_PATH}/*.h)

add_subdirectory (src)

add_subdirectory (tests)

#add_subdirectory (examples)
