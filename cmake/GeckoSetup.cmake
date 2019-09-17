# SPDX-License-Identifier: MIT
# Copyright contributors to the gecko project.

include(GNUInstallDirs)

set(tmpcxx 17)
if(CMAKE_CXX_STANDARD)
  set(tmpcxx ${CMAKE_CXX_STANDARD})
endif()
set(GECKO_CXX_STANDARD "${tmpcxx}" CACHE STRING "C++ standard to use")
unset(tmpcxx)
message("-- Using c++${GECKO_CXX_STANDARD}")

# "Standard" (conventional) options in cmake projects, but put
# the option here so cmake-gui shows it
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(CMAKE_DEBUG_POSTFIX "_d" CACHE STRING "Suffix for debug builds")

# not available per target?
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${PROJECT_SOURCE_DIR}/libs)

# including CTest adds the option for BUILD_TESTING which
# always wins, but this enables building tests when including
# as an external project (otherwise doesn't add the tests)
option(GECKO_EXTERN_BUILD_TESTS "Enables building the tests when building as external project" OFF)
# when tests are enabled based on the above and other tests, whether
# they are included in the default build or if they are placed in
# separate targets only
option(GECKO_COMPILE_TESTS_BY_DEFAULT "Enables building the unit tests by default" OFF)

# when including as an external library, whether to install static libs
# or not (ignored when BUILD_SHARED_LIBS is ON)
option(GECKO_EXTERN_INSTALL_STATIC_LIBS "Enables installing static libraries when building as an included external project" OFF)

# similarly, whether to build the applications or not
option(GECKO_EXTERN_BUILD_APPS "Enables building the applications when building as an included external project" OFF)

set(GECKO_LIB_SUFFIX "-${PROJECT_VERSION}" CACHE STRING "String added to all libraries")
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
