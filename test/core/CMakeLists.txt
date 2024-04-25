
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
# Description: Build definition for the test for the core library of Disa.
# ----------------------------------------------------------------------------------------------------------------------
# Test Definition
# ----------------------------------------------------------------------------------------------------------------------


add_executable(test_matrix_dense test_matrix_dense.cpp)
target_link_libraries(test_matrix_dense PRIVATE GTest::gtest_main core)
gtest_discover_tests(test_matrix_dense)

add_executable(test_matrix_sparse test_matrix_sparse.cpp)
target_link_libraries(test_matrix_sparse PRIVATE GTest::gtest_main core)
gtest_discover_tests(test_matrix_sparse)

add_executable(test_scalar test_scalar.cpp)
target_link_libraries(test_scalar PRIVATE GTest::gtest_main core)
gtest_discover_tests(test_scalar)

add_executable(test_vector_dense test_vector_dense.cpp)
target_link_libraries(test_vector_dense PRIVATE GTest::gtest_main core)
gtest_discover_tests(test_vector_dense)

add_executable(test_vector_operators test_vector_operators.cpp)
target_link_libraries(test_vector_operators PRIVATE GTest::gtest_main core)
gtest_discover_tests(test_vector_operators)