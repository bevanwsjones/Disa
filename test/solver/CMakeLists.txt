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
# Description: Build definition for the unit tests for the solver library of Disa.
# ----------------------------------------------------------------------------------------------------------------------
# Library Definition
# ----------------------------------------------------------------------------------------------------------------------

set(TEST_PROBLEMS "laplace_2d.h")

add_executable(test_direct "test_direct.cpp" ${TEST_PROBLEMS})
target_link_libraries(test_direct GTest::gtest_main solver)
gtest_discover_tests(test_direct)

add_executable(test_solver "test_solver.cpp")
target_link_libraries(test_solver GTest::gtest_main solver)
gtest_discover_tests(test_solver)

add_executable(test_solver_utilities "test_solver_utilities.cpp")
target_link_libraries(test_solver_utilities GTest::gtest_main solver)
gtest_discover_tests(test_solver_utilities)