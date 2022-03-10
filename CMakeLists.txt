set(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 3.19)
project(gtot)

set(CMAKE_CXX_STANDARD 17)

# Require CERN ROOT.
find_package(ROOT REQUIRED)

add_executable(gtot main.cpp GRANDEventADC.cpp GRANDEventADC.h Traces.h GRANDEventVoltage.cpp GRANDEventVoltage.h GRANDRun.cpp GRANDRun.h)

# This is the "manual" way to add ROOT, the old "make" style, in case one does not source root (like me)
exec_program(root-config ARGS --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS)
exec_program(root-config ARGS --libs OUTPUT_VARIABLE ROOT_CXX_LIBS)

set (CMAKE_CXX_FLAGS ${ROOT_CXX_FLAGS})
set (CMAKE_EXE_LINKER_FLAGS ${ROOT_CXX_LIBS})
