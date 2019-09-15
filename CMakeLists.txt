# SPDX-License-Identifier: MIT
# Copyright contributors to the gecko project.

cmake_minimum_required(VERSION 3.10)

project(gecko
  VERSION 0.1.0
  DESCRIPTION "Collection of tools for user interface and media processing"
  HOMEPAGE_URL "https://github.com/kdt3rd/gecko"
  LANGUAGES C CXX ASM)

####
#### NB: Look at this file for all the relevant options that can be
####     set to control the enabled components of the build
####
include(cmake/GeckoSetup.cmake)
include(cmake/UtilityFunctions.cmake)

# generate a "config" library that will hold all the
# common link and includes
add_subdirectory(config)

# always build the libraries
add_subdirectory(libs)
# only build the applications if requested or this is the top-level
if(GECKO_EXTERN_BUILD_APPS OR CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
#  add_subdirectory(apps)
endif()

#######################################
# Set up main install / export
include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
install(EXPORT ${PROJECT_NAME}
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  FILE ${PROJECT_NAME}Config.cmake
  NAMESPACE ${PROJECT_NAME}::
  EXPORT_LINK_INTERFACE_LIBRARIES
)

#######################################
# Set up unit tests
include(CTest)
if(BUILD_TESTING AND (GECKO_EXTERN_BUILD_TESTS OR CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR))
  enable_testing()

  # we don't build the tests by default, and cmake
  # doesn't yet have a mechanism to add dependencies to
  # built-in targets (i.e. make test), so we have to do a
  # bit of extra work, make a custom target that all the
  # test binaries will mark themselves as dependents of
  # then we can also add run_tests and run_valgrind targets
  # to build everything and then run the tests
  add_custom_target(build_tests)
  add_custom_target(build_slow_tests)
  
  add_subdirectory(tests EXCLUDE_FROM_ALL)
  
  add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG> -E "slow.*"
    VERBATIM
  )
  add_dependencies(run_tests build_tests)
  
  add_custom_target(run_slow_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG> -R "slow.*"
    VERBATIM
  )
  add_dependencies(run_slow_tests build_tests)
  
  add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG>
    VERBATIM
  )
  add_dependencies(run_all_tests build_tests)
  
  add_custom_target(run_valgrind
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG> --force-new-ctest-process --test-action memcheck
    COMMAND cat "${CMAKE_BINARY_DIR}/Testing/Temporary/MemoryChecker.*.log"
  )
  add_dependencies(run_valgrind build_tests)
endif()
