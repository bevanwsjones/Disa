# ----------------------------------------------------------------------------------------------------------------------
# MIT License
# Copyright (c) 2022 Bevan W.S. Jones
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
# documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# ----------------------------------------------------------------------------------------------------------------------
# File Name: CMakeLists.txt
# Description: Root CMake file, which defining the project and various build recipes.
# ----------------------------------------------------------------------------------------------------------------------
# Project Definition
# ----------------------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.22)

project(disa LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)

# ----------------------------------------------------------------------------------------------------------------------
# Options
# ----------------------------------------------------------------------------------------------------------------------

option(ENABLE_TEST "Turn on to enable tests" ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDISA_DEBUG")
add_compile_definitions("DISA_DEBUG")

# ----------------------------------------------------------------------------------------------------------------------
# Compiler Setup
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------
# Testing
# ----------------------------------------------------------------------------------------------------------------------

if(ENABLE_TEST)
  include(FetchContent)
  FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG release-1.12.1)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()
  include(CTest)
  include(GoogleTest)
endif ()


# ----------------------------------------------------------------------------------------------------------------------
# Project Setup
# ----------------------------------------------------------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(${PROJECT_SOURCE_DIR}/src/core)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/graph)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/solver)

if(ENABLE_TEST)
  add_subdirectory(${PROJECT_SOURCE_DIR}/test/core)
  add_subdirectory(${PROJECT_SOURCE_DIR}/test/graph)
  add_subdirectory(${PROJECT_SOURCE_DIR}/test/solver)
endif()

# ----------------------------------------------------------------------------------------------------------------------
# Library Recipe
# ----------------------------------------------------------------------------------------------------------------------

set(LIBRARIES 
    core
    graph 
    solver
)

add_library(disa INTERFACE)
target_link_libraries(disa INTERFACE ${LIBRARIES})
set_target_properties(disa PROPERTIES LINKER_LANGUAGE CXX)
