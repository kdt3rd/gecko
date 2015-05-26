.SUFFIXES:
.DEFAULT: default
.PHONY: default debug release mingw build clean graph config
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

TARGETS := $(filter-out default debug release build mingw clean graph config,${MAKECMDGOALS})
MAKECMDGOALS :=

default: ${BUILD_DIR}/
	ninja ${NINJA_ARGS} -C ${BUILD_DIR} ${TARGETS}

debug: debug/
	ninja ${NINJA_ARGS} -C ${BUILD_DIR} ${TARGETS}

mingw: mingw/
	ninja ${NINJA_ARGS} -C ${BUILD_DIR} ${TARGETS}

release: release/
	ninja ${NINJA_ARGS} -C ${BUILD_DIR} ${TARGETS}

build: build/
	ninja ${NINJA_ARGS} -C ${BUILD_DIR} ${TARGETS}

debug/:
	constructor

mingw/:
	constructor

release/:
	constructor

build/:
	constructor --verbose

config:
	constructor

clean:
	rm -rf debug release build mingw

graph:
	ninja -C build -t graph ${TARGETS} | sed s\"`pwd`/\"\"g > deps.dot

% :: default
	@
