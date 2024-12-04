set(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 3.16)
project(gtot)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH ${ROOT_INCLUDE_DIRS} ${ROOT_LIBRARY_DIR})

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
#execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")

#set(CMAKE_MODULE_PATH ${})
#find_package(Vdt)
set(VDT_INCLUDE_DIR $ENV{ROOTSYS}/include)
set(VDT_LIBRARY $ENV{ROOTSYS}/lib)

# Require CERN ROOT.
find_package(ROOT CONFIG REQUIRED)
#include(CMakePrintHelpers)
#cmake_print_variables($ENV{ROOTSYS})
#cmake_print_variables(ROOT_LIBRARY_DIR)

add_executable(gtot main.cpp src/TADC.cpp inc/TADC.h inc/Traces.h inc/Traces_fv2.h src/TRawVoltage.cpp inc/TRawVoltage.h src/TRun.cpp inc/TRun.h inc/LinkDef.h inc/gtot.h inc/gp13.h
        inc/main_functions.h src/TRunRawVoltage.cpp inc/TRunRawVoltage.h)

# If includes are not set, set them the old way
# This is the "manual" way to add ROOT, the old "make" style, in case one does not source root (like me)
if(NOT CMAKE_CXX_FLAGS MATCHES "-I")
    exec_program(root-config ARGS --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS)
    exec_program(root-config ARGS --libs OUTPUT_VARIABLE ROOT_CXX_LIBS)

    set (CMAKE_CXX_FLAGS ${ROOT_CXX_FLAGS})
    set (CMAKE_EXE_LINKER_FLAGS ${ROOT_CXX_LIBS})
endif()

# A dictionary is needed for vector<vector<short>> etc.
add_library(gtot_vectors SHARED)
target_include_directories(gtot_vectors PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
root_generate_dictionary(G__gtot_vectors inc/gtot_vector_link.h MODULE gtot_vectors LINKDEF inc/LinkDef.h)
target_link_libraries(gtot PUBLIC ROOT::Core ROOT::Tree gtot_vectors)
