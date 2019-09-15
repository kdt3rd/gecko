# SPDX-License-Identifier: MIT
# Copyright contributors to the gecko project.

#######################################

### Boiler plate around defining a simple program
### and adding that to the list of tests to run
function(GECKO_UNIT_TEST testsrc)
  set(options SLOW)
  set(singleargs LIB)
  set(multiargs EXTRA_SOURCES EXTRA_LIBS ARGS)
  cmake_parse_arguments(GK_CURTEST "${options}" "${singleargs}" "${multiargs}" ${ARGN})

  get_filename_component(testtarg ${testsrc} NAME_WE)
  set(uniqtestname ${testtarg})
  set(locallibs)
  if(GK_CURTEST_LIB)
    set(testtarg ${PROJECT_NAME}_${GK_CURTEST_LIB}_${testtarg})
    set(uniqtestname ${PROJECT_NAME}.${GK_CURTEST_LIB}.${uniqtestname})
    set(locallibs ${PROJECT_NAME}::${GK_CURTEST_LIB})
  else()
    set(testtarg ${PROJECT_NAME}_${testtarg})
    set(uniqtestname ${PROJECT_NAME}.${uniqtestname})
  endif()
  if(GK_CURTEST_SLOW)
    set(uniqtestname slow.${uniqtestname})
  endif()

  add_executable(${testtarg} ${testsrc} ${GK_CURTEST_EXTRA_SOURCES})
  if(locallibs OR GK_CURTEST_EXTRA_LIBS)
    target_link_libraries(${testtarg} ${locallibs} ${GK_CURTEST_EXTRA_LIBS})
  endif()

  add_test(NAME ${uniqtestname} COMMAND $<TARGET_FILE:${testtarg}> ${GK_CURTEST_ARGS})
  add_dependencies(build_tests ${testtarg})
endfunction()

#######################################

### Boiler plate around defining a library
### and setting all the common options, aliases and install setup
function(GECKO_LIBRARY_DECLARE libname)
  set(options)
  set(singleargs GENERATED_DIR)
  set(multiargs SOURCES PUBLIC_HEADERS PRIVATE_HEADERS DEPENDENCIES PRIVATE_DEPENDENCIES ARGS)
  cmake_parse_arguments(GK_CURLIB "${options}" "${singleargs}" "${multiargs}" ${ARGN})
  # be a good citizen in external projects
  set(uniqname ${PROJECT_NAME}_${libname})

  add_library(${uniqname}
    ${GK_CURLIB_SOURCES}
    ${GK_CURLIB_PRIVATE_HEADERS}
    ${GK_CURLIB_PUBLIC_HEADERS}
  )
  
  target_compile_features(${uniqname} PUBLIC cxx_std_${GECKO_CXX_STANDARD})
  # TODO
  #if(BUILD_SHARED_LIBS)
  #  target_compile_definitions(${uniqname} PRIVATE GECKO_EXPORT_DLL)
  #  if(WIN32)
  #    target_compile_definitions(${uniqname} PUBLIC GECKO_IMPORT_DLL)
  #  endif()
  #endif()
  if(GK_CURLIB_GENERATED_DIR)
    target_include_directories(${uniqname} INTERFACE $<BUILD_INTERFACE:${GK_CURLIB_GENERATED_DIR}>)
  endif()
  target_include_directories(${uniqname} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/libs>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
  )
  target_link_libraries(${uniqname} PUBLIC ${GK_CURLIB_DEPENDENCIES})
  if(GK_CURLIB_PRIVATE_DEPENDENCIES)
    target_link_libraries(${uniqname} PRIVATE ${GK_CURLIB_PRIVATE_DEPENDENCIES})
  endif()
  set_target_properties(${uniqname} PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
  )
  # MSVC Does not enable stack unwinding by default, which seems
  # criminal in a modern era
  if (MSVC)
    target_compile_options(${uniqname} PUBLIC "/EHsc")
  endif()
  set_property(TARGET ${uniqname} PROPERTY PUBLIC_HEADER ${GK_CURLIB_PUBLIC_HEADERS})

# TODO: Handle shared libraries, and either set the execpath
#       for windows or output to common folder to handle the
#       missing rpath features...
#  if(BUILD_SHARED_LIBS)
#    set_target_properties(${uniqname} PROPERTIES
#      SOVERSION ${GECKO_SOVERSION}
#      VERSION ${GECKO_LIB_VERSION}
#    )
#  endif()

  set_target_properties(${uniqname} PROPERTIES
    OUTPUT_NAME "${libname}${GECKO_LIB_SUFFIX}"
    EXPORT_NAME ${libname}
    RUNTIME_OUTPUT_DIRECTORY "${${PROJECT_NAME}_BINARY_DIR}/bin"
  )

  #####################################
  # Create a library alias that should be used
  add_library(${PROJECT_NAME}::${libname} ALIAS ${uniqname})

  #####################################
  # Set up for install
  install(TARGETS ${uniqname}
    EXPORT ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  )

endfunction()
