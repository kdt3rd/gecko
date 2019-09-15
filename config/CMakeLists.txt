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

find_library(GECKO_ATOMIC_LIBRARY NAMES atomic atomic.so.1 libatomic.so.1)
mark_as_advanced(GECKO_ATOMIC_LIBRARY)

add_library(${PROJECT_NAME}_config INTERFACE)

target_link_libraries(${PROJECT_NAME}_config INTERFACE ${CMAKE_DL_LIBS} ${GECKO_RT_LIBRARY} ${GECKO_ATOMIC_LIBRARY})

install(TARGETS ${PROJECT_NAME}_config EXPORT ${PROJECT_NAME})

add_library(${PROJECT_NAME}::config ALIAS ${PROJECT_NAME}_config)
