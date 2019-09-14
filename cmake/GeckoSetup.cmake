
include(GNUInstallDirs)

set(tmpcxx 17)
if(CMAKE_CXX_STANDARD)
  set(tmpcxx ${CMAKE_CXX_STANDARD})
endif()
set(GECKO_CXX_STANDARD "${tmpcxx}" CACHE STRING "C++ standard to use")
unset(tmpcxx)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_TESTING "Enable test code" ON)

set(CMAKE_DEBUG_POSTFIX "_d" CACHE STRING "Suffix for debug builds")

# not available per target?
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${PROJECT_SOURCE_DIR}/libs)

set(GECKO_LIB_SUFFIX "-${GECKO_VERSION}" CACHE STRING "String added to all libraries")
set(GECKO_STATIC_LIB_SUFFIX "_static" CACHE STRING "Suffix for static libraries")

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
endif()
#if(NOT CMAKE_INSTALL_RPATH)
#  message(WARNING "TODO Set install rpath")
#endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS ": Setting build type to 'RelWithDebInfo'")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(NOT TARGET Threads::Threads)
  set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
  set(THREADS_PREFER_PTHREAD_FLAG TRUE)
  find_package(Threads)
  if(NOT Threads_FOUND)
    message(FATAL_ERROR ": Threading library is required")
  endif()
endif()

#######################################

function(GECKO_UNIT_TEST test_name)
  set(options)
  set(singleargs)
  set(multiargs SOURCES LIBS ARGS)
  cmake_parse_arguments(GK_CURTEST "${options}" "${singleargs}" "${multiargs}" ${ARGN})

  string(REPLACE "." "_" testtarg test_${test_name})

  add_executable(${testtarg} ${GK_CURTEST_SOURCES})
  if(GK_CURTEST_LIBS)
    target_link_libraries(${testtarg} ${GK_CURTEST_LIBS})
  endif()

  add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${testtarg}> ${GK_CURTEST_ARGS})

  add_dependencies(build_tests ${testtarg})
endfunction()
