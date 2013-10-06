.SUFFIXES:
.DEFAULT: default
.PHONY: default debug dist build clean graph config cmake
.NOTPARALLEL:
.SILENT:
.ONESHELL:

BUILD_TYPE := build
BUILD_DIR := build
ifeq ($(findstring debug,${MAKECMDGOALS}),debug)
BUILD_TYPE := debug
BUILD_DIR := debug
else
ifeq ($(findstring dist,${MAKECMDGOALS}),dist)
BUILD_TYPE := release
BUILD_DIR := release
endif
endif
ifeq ($(findstring cmake,${MAKECMDGOALS}),cmake)
$(warning CMake is no longer used, just run make)
endif

TARGETS := $(filter-out default debug dist build clean graph config cmake,${MAKECMDGOALS})
override MAKECMDGOALS :=

default:
	./configure ${BUILD_TYPE}
	ninja ${NINJA_ARGS} -C ${BUILD_DIR} ${TARGETS}

debug: default

dist: default

build: default

cmake: config

config:
	./configure build
	./configure dist
	./configure debug

clean:
	rm -rf debug release build

graph:
	ninja -C build -t graph ${TARGETS} | sed s\"`pwd`/\"\"g > deps.dot

ifneq ("${TARGETS}","")

% :: default
	@

endif
