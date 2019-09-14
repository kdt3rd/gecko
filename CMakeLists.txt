# SPDX-License-Identifier: MIT
# Copyright contributors to the gecko project.

cmake_minimum_required(VERSION 3.10)

project(Gecko VERSION 1.0 LANGUAGES CXX C)

include(cmake/GeckoSetup.cmake)

add_subdirectory(libs)

if(BUILD_TESTING)
  enable_testing()
  include(CTest)

  # we don't build the tests by default, and cmake
  # doesn't yet have a mechanism to add dependencies to
  # built-in targets (i.e. make test), so we have to do a
  # bit of extra work, make a custom target that all the
  # test binaries will mark themselves as dependents of
  # then we can also add run_tests and run_valgrind targets
  # to build everything and then run the tests
  add_custom_target(build_tests)
  
  add_subdirectory(tests EXCLUDE_FROM_ALL)
  
  add_custom_target(run_tests COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG>)
  add_dependencies(run_tests build_tests)
  
  add_custom_target(run_valgrind
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG> --force-new-ctest-process --test-action memcheck
    COMMAND cat "${CMAKE_BINARY_DIR}/Testing/Temporary/MemoryChecker.*.log"
  )
  add_dependencies(run_valgrind build_tests)
endif()
