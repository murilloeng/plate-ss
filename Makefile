ifeq ($(OS),Windows_NT)
include make/Makefile-msvc.mk
else
include make/Makefile-unix.mk
endif