.SUFFIXES:
.DEFAULT: all
.ONESHELL:
.NOTPARALLEL:
.SILENT:

.PHONY: all clean graph config release build debug win64 cppcheck tidy cmds format
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
LIVE_CONFIG := win64cross-release
endif

ifeq ("$(wildcard ${LIVE_CONFIG})","")
NEED_CONFIG := config
endif

TARGETS := $(filter-out all clean graph config release build debug win64 cppcheck tidy cmds format,${MAKECMDGOALS})
MAKECMDGOALS :=

all: $(LIVE_CONFIG)

config:
	echo "Generating Build Files..."
	constructor

%/build.ninja:
	@constructor

$(LIVE_CONFIG): $(LIVE_CONFIG)/build.ninja
	@cd $(LIVE_CONFIG); ninja $(TARGETS)

build/compile_commands.json: build/build.ninja
	@cd build; ninja -t compdb cxx > compile_commands.json

cmds: build/compile_commands.json

cppcheck: cmds
	@cppcheck --project=build/compile_commands.json --enable=all --std=c++11

tidy: cmds
	@clang-tidy -enable-check-profile -p build $(shell cat build/compile_commands.json|grep '\"file\":' | cut -d'"' -f 4) > clang_tidy_warnings.log
	@echo "warnings in clang_tidy_warnings.log..."

# NB: We are NOT doing the .c files because the sqlite3 file is all-in-one and quite ... large and takes forever
format:
	@find . -path ./build -prune -o -path ./debug -prune -o -path ./release -prune -o -path ./win64cross-release -prune -o \( -name '*.cpp' -o -name '*.mm' -o -name '*.h' \) -exec clang-format -style=file -verbose -i {} \;

$(TARGETS) :: all ;

graph:
	@cd $(LIVE_CONFIG); ninja -t graph $(TARGETS) | sed s\"`pwd`/\"\"g > ../deps.dot

clean:
	@echo "Cleaning..."
	@rm -rf release build debug

