.SUFFIXES:
.DEFAULT: all
.ONESHELL:
.NOTPARALLEL:
.SILENT:

.PHONY: all clean graph config release build debug win64 cppcheck tidy
LIVE_CONFIG := build

ifeq ($(findstring release,${MAKECMDGOALS}),release)
LIVE_CONFIG := release
endif
ifeq ($(findstring build,${MAKECMDGOALS}),build)
LIVE_CONFIG := build
endif
ifeq ($(findstring debug,${MAKECMDGOALS}),debug)
LIVE_CONFIG := debug
endif
ifeq ($(findstring debug,${MAKECMDGOALS}),win64)
LIVE_CONFIG := win64
endif

ifeq ("$(wildcard ${LIVE_CONFIG})","")
NEED_CONFIG := config
endif

TARGETS := $(filter-out all clean graph config release build debug win64 cppcheck tidy,${MAKECMDGOALS})
MAKECMDGOALS :=

all: ${LIVE_CONFIG}

config:
	echo "Generating Build Files..."
	constructor

release/: ${NEED_CONFIG}

release: release/
	@cd release; ninja ${TARGETS}

build/: ${NEED_CONFIG}

build: build/
	@cd build; ninja ${TARGETS}

debug/: ${NEED_CONFIG}

debug: debug/
	@cd debug; ninja ${TARGETS}

win64cross-release/: ${NEED_CONFIG}

win64: win64cross-release/
	@cd win64cross-release; ninja ${TARGETS}

build/compile_commands.json: build/
	@cd build; ninja -t compdb cxx > compile_commands.json

cppcheck: build/compile_commands.json
	@cppcheck --project=build/compile_commands.json --enable=all --std=c++11

tidy: build/compile_commands.json
	@clang-tidy -checks='*' --header-filter=.* -p build $(shell cat build/compile_commands.json|grep '\"file\":' | cut -d'"' -f 4) > clang_tidy_warnings.log
	@echo "warnings in clang_tidy_warnings.log..."

${TARGETS} :: all ;

graph:
	@cd ${LIVE_CONFIG}; ninja -t graph ${TARGETS} | sed s\"`pwd`/\"\"g > ../deps.dot

clean:
	@echo "Cleaning..."
	@rm -rf release build debug

