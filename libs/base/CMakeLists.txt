
add_library(base
  any.cpp
  cmd_line.cpp
  contract.cpp
  format.cpp
  string_util.cpp
  unit_test.cpp
  )

target_compile_features(base PUBLIC cxx_std_${GECKO_CXX_STANDARD})

set_target_properties(base PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  POSITION_INDEPENDENT_CODE ON
)

add_library(Gecko::Base ALIAS base)
