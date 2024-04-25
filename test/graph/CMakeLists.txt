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
# Description: Build definition for the tests for the graph library of Disa.
# ----------------------------------------------------------------------------------------------------------------------
# Test Definition
# ----------------------------------------------------------------------------------------------------------------------

add_executable(test_adjacency_graph test_adjacency_graph.cpp)
target_link_libraries(test_adjacency_graph GTest::gtest_main graph)
gtest_discover_tests(test_adjacency_graph)

add_executable(test_adjacency_subgraph test_adjacency_subgraph.cpp)
target_link_libraries(test_adjacency_subgraph GTest::gtest_main graph)
gtest_discover_tests(test_adjacency_subgraph)

add_executable(test_edge test_edge.cpp)
target_link_libraries(test_edge GTest::gtest_main graph)
gtest_discover_tests(test_edge)

add_executable(test_partition test_partition.cpp)
target_link_libraries(test_partition GTest::gtest_main graph)
gtest_discover_tests(test_partition)

add_executable(test_reorder test_reorder.cpp)
target_link_libraries(test_reorder GTest::gtest_main graph)
gtest_discover_tests(test_reorder)

add_executable(test_graph_utilities test_graph_utilities.cpp)
target_link_libraries(test_graph_utilities GTest::gtest_main graph)
gtest_discover_tests(test_graph_utilities)
