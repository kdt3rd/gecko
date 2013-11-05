.SUFFIXES:
.DEFAULT: default
.PHONY: default debug release mingw build clean graph config cmake
.NOTPARALLEL:
.SILENT:
.ONESHELL:

BUILD_TYPE := build
BUILD_DIR := build
ifeq ($(findstring debug,${MAKECMDGOALS}),debug)
BUILD_TYPE := debug
BUILD_DIR := debug
else
ifeq ($(findstring release,${MAKECMDGOALS}),release)
BUILD_TYPE := release
BUILD_DIR := release
else
ifeq ($(findstring mingw,${MAKECMDGOALS}),mingw)
BUILD_TYPE := mingw
BUILD_DIR := mingw
endif
endif
endif

TARGETS := $(filter-out default debug release build mingw clean graph config cmake,${MAKECMDGOALS})
override MAKECMDGOALS :=

default:
	./configure ${BUILD_TYPE}
	ninja ${NINJA_ARGS} -C ${BUILD_DIR} ${TARGETS}

debug: default

mingw: default

release: default

build: default

cmake: config

config:
	./configure build
	./configure release
	./configure debug
	./configure mingw

clean:
	rm -rf debug release build mingw

graph:
	ninja -C build -t graph ${TARGETS} | sed s\"`pwd`/\"\"g > deps.dot

ifneq ("${TARGETS}","")

% :: default
	@

endif
