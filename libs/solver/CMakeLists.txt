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
# Description: Build definition for the solver library of Disa, and the associated tests.
# ----------------------------------------------------------------------------------------------------------------------
# Library Definition
# ----------------------------------------------------------------------------------------------------------------------

set(SOURCES        
        "direct_lower_upper_factorisation.h"
        "direct_lower_upper_factorisation.hpp"
        "direct.h"
        "solver_fixed_point.h"
        "solver_fixed_point.cpp"
        "solver_iterative.h"
        "solver_utilities.h"
        "solver_utilities.hpp"
        "solver.h"
        "solver.cpp"
        )

set(LIBRARIES
        core
        )

add_library(solver ${SOURCES})
target_link_libraries(solver ${LIBRARIES})
set_target_properties(solver PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(solver PUBLIC .)

# ----------------------------------------------------------------------------------------------------------------------
# Testing
# ----------------------------------------------------------------------------------------------------------------------

if(ENABLE_TEST)

set(TEST_PROBLEMS 
        "test/laplace_2d.h")

  add_executable(test_direct "test/test_direct.cpp" ${TEST_PROBLEMS})
  target_link_libraries(test_direct GTest::gtest_main solver)
  gtest_discover_tests(test_direct)


  add_executable(test_solver "test/test_solver.cpp")
  target_link_libraries(test_solver GTest::gtest_main solver)
  gtest_discover_tests(test_solver)

  add_executable(test_solver_utilities "test/test_solver_utilities.cpp")
  target_link_libraries(test_solver_utilities GTest::gtest_main solver)
  gtest_discover_tests(test_solver_utilities)

endif ()