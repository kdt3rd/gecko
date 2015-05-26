.SUFFIXES:
.DEFAULT: all
.ONESHELL:
.NOTPARALLEL:
.SILENT:

.PHONY: all clean graph config release build debug
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

ifeq ("$(wildcard ${LIVE_CONFIG})","")
NEED_CONFIG := config
endif

TARGETS := $(filter-out all clean graph config release build debug,${MAKECMDGOALS})
MAKECMDGOALS :=

all: ${LIVE_CONFIG}

config:
	echo "Generating Build Files..."
	constructor

release/: ${NEED_CONFIG}

release: release/
	@cd /home/kimball/Development/OSS/gecko/release; /home/kimball/bin/ninja ${TARGETS}

build/: ${NEED_CONFIG}

build: build/
	@cd /home/kimball/Development/OSS/gecko/build; /home/kimball/bin/ninja ${TARGETS}

debug/: ${NEED_CONFIG}

debug: debug/
	@cd /home/kimball/Development/OSS/gecko/debug; /home/kimball/bin/ninja ${TARGETS}


${TARGETS}: all ;

clean:
	@echo "Cleaning..."
	@cd /home/kimball/Development/OSS/gecko/release; /home/kimball/bin/ninja -t clean
	@cd /home/kimball/Development/OSS/gecko/build; /home/kimball/bin/ninja -t clean
	@cd /home/kimball/Development/OSS/gecko/debug; /home/kimball/bin/ninja -t clean

