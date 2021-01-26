project (nessdl2)

add_executable (
  ${PROJECT_NAME}
  src/main.cc)

target_link_libraries (
  ${PROJECT_NAME}
  PRIVATE
  Nesdev::core)

target_compile_features (
  ${PROJECT_NAME}
  PRIVATE
  cxx_std_17)

set_target_properties (
  ${PROJECT_NAME}
  PROPERTIES
  CXX_STANDARD          17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS        NO)
