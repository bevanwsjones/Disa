// ----------------------------------------------------------------------------------------------------------------------
//  MIT License
//  Copyright (c) 2022 Bevan W.S. Jones
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
//  documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
//  permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
//  Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
//  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----------------------------------------------------------------------------------------------------------------------
//  File Name:
//  Description:
// ----------------------------------------------------------------------------------------------------------------------
//

#include "gtest/gtest.h"
#include "graph.h"

using namespace Disa;

//----------------------------------------------------------------------------------------------------------------------
// AdjacencyGraph
//--------------------------------------------------------------------------------------------------------------------
// Public Member Functions
//--------------------------------------------------------------------------------------------------------------------

TEST(test_adjacency_graph, edge_list_construction) {
  AdjacencyGraph graph({{0, 1}, {2, 1}, {2, 3}, {3, 0}}); // place some edge backwards.

  EXPECT_EQ(graph.size_vertices(), 4);
  EXPECT_EQ(graph.size_edge(), 4);

  EXPECT_EQ(graph[0].size(), 2);
  EXPECT_EQ(graph[1].size(), 2);
  EXPECT_EQ(graph[2].size(), 2);
  EXPECT_EQ(graph[3].size(), 2);

  EXPECT_EQ(graph[0][0], 1);
  EXPECT_EQ(graph[0][1], 3);
  EXPECT_EQ(graph[1][0], 0);
  EXPECT_EQ(graph[1][1], 2);
  EXPECT_EQ(graph[2][0], 1);
  EXPECT_EQ(graph[2][1], 3);
  EXPECT_EQ(graph[3][0], 0);
  EXPECT_EQ(graph[3][1], 2);
}

//--------------------------------------------------------------------------------------------------------------------
// Element Access
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
// Iterators
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
// Capacity
//--------------------------------------------------------------------------------------------------------------------

TEST(test_adjacency_graph, empty) {
  AdjacencyGraph graph;
  EXPECT_TRUE(graph.empty());
  graph.insert(std::make_pair(0, 1));
  EXPECT_FALSE(graph.empty());
}

TEST(test_adjacency_graph, size) {
  AdjacencyGraph graph;
  EXPECT_EQ(graph.size_vertices(), 0);
  EXPECT_EQ(graph.size_edge(), 0);
  EXPECT_EQ(graph.size().first, std::make_pair(0, 0).first);
  EXPECT_EQ(graph.size().second, std::make_pair(0, 0).second);

  graph.insert(std::make_pair(0, 1));
  EXPECT_EQ(graph.size_vertices(), 2);
  EXPECT_EQ(graph.size_edge(), 1);
  EXPECT_EQ(graph.size().first, std::make_pair(2, 1).first);
  EXPECT_EQ(graph.size().second, std::make_pair(2, 1).second);

  graph.insert(std::make_pair(1, 2));
  EXPECT_EQ(graph.size_vertices(), 3);
  EXPECT_EQ(graph.size_edge(), 2);
  EXPECT_EQ(graph.size().first, std::make_pair(3, 2).first);
  EXPECT_EQ(graph.size().second, std::make_pair(3, 2).second);

  graph.insert(std::make_pair(5, 6)); // Creates a set of unconnected vertices, total vertices 7.
  EXPECT_EQ(graph.size_vertices(), 7);
  EXPECT_EQ(graph.size_edge(), 3);
  EXPECT_EQ(graph.size().first, std::make_pair(7, 3).first);
  EXPECT_EQ(graph.size().second, std::make_pair(7, 3).second);
}

TEST(test_adjacency_graph, reserve_capacity) {
  AdjacencyGraph graph;
  EXPECT_EQ(graph.capacity().first, 0);
  EXPECT_EQ(graph.capacity().second, 0);

  graph.reserve(5);
  EXPECT_EQ(graph.capacity().first, 5);
  EXPECT_EQ(graph.capacity().second, 0);

  graph.reserve(10, 40);
  EXPECT_EQ(graph.capacity().first, 10);
  EXPECT_EQ(graph.capacity().second, 40);
}

TEST(test_adjacency_graph, shrink_to_fit) {
  EXPECT_TRUE(false);
}

//--------------------------------------------------------------------------------------------------------------------
// Modifiers
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
// Graph Operators
//--------------------------------------------------------------------------------------------------------------------


