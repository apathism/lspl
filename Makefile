###############################################################################
# Makefile for LSPL project
##############################################################################

all: tools

deps:
	cd deps/ && ./bootstrap.sh

# Core

core: deps
	make -C core linux64
core-mac: aot
	make -C core mac
core-mac-xcode: aot
	make -C core mac-xcode

tools: core
	make -C tools linux64

install:
	cp core/build-linux64/liblspl.so /usr/lib/
	cp -r core/src/main/lspl /usr/include
	cp -r deps/aot /usr/share

