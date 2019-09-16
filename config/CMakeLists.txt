# SPDX-License-Identifier: MIT
# Copyright contributors to the gecko project.

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckLibraryExists)
include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
include(CheckSymbolExists)

check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(windows.h HAVE_WINDOWS_H)

set(GECKO_LIB_DEPENDENCIES ${CMAKE_DL_LIBS})

if (NOT WIN32)
  set(CMAKE_REQUIRED_LIBRARIES_SAVE ${CMAKE_REQUIRED_LIBRARIES})
  set(CMAKE_REQUIRED_LIBRARIES rt)
  CHECK_SYMBOL_EXISTS(_POSIX_TIMERS "unistd.h;time.h" GECKO_POSIX_TIMERS)
  set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES_SAVE})
  if(GECKO_POSIX_TIMERS)
    find_library(GECKO_RT_LIBRARY NAMES rt)
    mark_as_advanced(GECKO_RT_LIBRARY)
  endif()
else()
  find_library(GECKO_RT_LIBRARY NAMES winmm)
  mark_as_advanced(GECKO_RT_LIBRARY)
endif()

add_library(${PROJECT_NAME}_config INTERFACE)
if(CMAKE_DL_LIBS)
  target_link_libraries(${PROJECT_NAME}_config INTERFACE ${CMAKE_DL_LIBS})
endif()
if(GECKO_RT_LIBRARY)
  target_link_libraries(${PROJECT_NAME}_config INTERFACE ${GECKO_RT_LIBRARY})
endif()

# TODO: The CheckAtomic.cmake that is in llvm is far more complete
find_library(GECKO_ATOMIC_LIBRARY NAMES atomic atomic.so.1 libatomic libatomic.so libatomic.so.1)
mark_as_advanced(GECKO_ATOMIC_LIBRARY)
if(GECKO_ATOMIC_LIBRARY)
  target_link_libraries(${PROJECT_NAME}_config INTERFACE ${GECKO_ATOMIC_LIBRARY})
endif()

# MSVC Does not enable stack unwinding by default, which seems
# criminal in a modern era
if(MSVC)
  target_compile_options(${PROJECT_NAME}_config INTERFACE "/EHsc")
endif()
if(WIN32)
  target_link_libraries(${PROJECT_NAME}_config INTERFACE kernel32)
endif()

install(TARGETS ${PROJECT_NAME}_config EXPORT ${PROJECT_NAME})
add_library(${PROJECT_NAME}::config ALIAS ${PROJECT_NAME}_config)
