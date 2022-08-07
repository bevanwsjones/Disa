#----------------------------------------------------------------------------------------------------------------------
# MIT License
# Copyright (c) 2022 bevanwsjones
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

# ----------------------------------------------------------------------------------------------------------------------
# Compiler Setup
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------
# Library Recipe
# ----------------------------------------------------------------------------------------------------------------------

include_directories(include)

set(SOURCES
        include/Macros.h
        include/Vector.h
        src/main.cpp)

#add_library(disa ${SOURCES})

add_executable(disa ${SOURCES})

# ----------------------------------------------------------------------------------------------------------------------
# Testing
# ----------------------------------------------------------------------------------------------------------------------

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if (ENABLE_TEST)

  include(CTest)
  include(GoogleTest)

  add_executable(test_vector tests/TestVector.cpp)
  target_link_libraries(test_vector GTest::gtest_main)

  gtest_discover_tests(test_vector)
endif()
