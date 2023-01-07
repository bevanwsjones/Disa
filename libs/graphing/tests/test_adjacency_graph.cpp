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

#include "gtest/gtest.h"
#include "adjacency_graph.h"

using namespace Disa;

//----------------------------------------------------------------------------------------------------------------------
// AdjacencyGraph
//--------------------------------------------------------------------------------------------------------------------
// Public Member Functions
//--------------------------------------------------------------------------------------------------------------------

TEST(test_adjacency_graph, edge_list_construction) {
  AdjacencyGraph graph({{0, 1}, {2, 1}, {2, 3}, {3, 0}}); // place some edge backwards.

  EXPECT_EQ(graph.size_vertex(), 4);
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

TEST(test_adjacency_graph, at) {
  AdjacencyGraph graph({{0, 1}, {2, 1}, {2, 3}, {3, 0}}); // place some edge backwards.

  EXPECT_EQ(graph[0][0], 1);
  EXPECT_EQ(graph[0][1], 3);
  EXPECT_EQ(graph[1][0], 0);
  EXPECT_EQ(graph[1][1], 2);
  EXPECT_EQ(graph[2][0], 1);
  EXPECT_EQ(graph[2][1], 3);
  EXPECT_EQ(graph[3][0], 0);
  EXPECT_EQ(graph[3][1], 2);

  EXPECT_DEATH(auto test_0 = graph[-1], "./*");
  EXPECT_DEATH(auto test_1 = graph[4], "./*");
}

TEST(test_adjacency_graph, access_operator) {
  AdjacencyGraph graph({{0, 1}, {2, 1}, {2, 3}, {3, 0}}); // place some edge backwards.

  EXPECT_EQ(graph[0][0], 1);
  EXPECT_EQ(graph[0][1], 3);
  EXPECT_EQ(graph[1][0], 0);
  EXPECT_EQ(graph[1][1], 2);
  EXPECT_EQ(graph[2][0], 1);
  EXPECT_EQ(graph[2][1], 3);
  EXPECT_EQ(graph[3][0], 0);
  EXPECT_EQ(graph[3][1], 2);

  EXPECT_DEATH(auto test_2 = graph[-1], "./*");
  EXPECT_DEATH(auto test_3 = graph[4], "./*");
}

TEST(test_adjacency_graph, data) {
  AdjacencyGraph graph;
  EXPECT_EQ(graph.data().first, nullptr);
  EXPECT_EQ(graph.data().second, nullptr);

  graph.resize(4); // we have no edges yet, so the vertex list remains a nullptr.
  EXPECT_EQ(graph.data().first, nullptr);
  EXPECT_NE(graph.data().second, nullptr);

  graph.insert(std::make_pair(0, 1));
  EXPECT_NE(graph.data().first, nullptr);
  EXPECT_NE(graph.data().second, nullptr);
}

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
  EXPECT_EQ(graph.size_vertex(), 0);
  EXPECT_EQ(graph.size_edge(), 0);
  EXPECT_EQ(graph.size().first, std::make_pair(0, 0).first);
  EXPECT_EQ(graph.size().second, std::make_pair(0, 0).second);

  graph.insert(std::make_pair(0, 1));
  EXPECT_EQ(graph.size_vertex(), 2);
  EXPECT_EQ(graph.size_edge(), 1);
  EXPECT_EQ(graph.size().first, std::make_pair(2, 1).first);
  EXPECT_EQ(graph.size().second, std::make_pair(2, 1).second);

  graph.insert(std::make_pair(1, 2));
  EXPECT_EQ(graph.size_vertex(), 3);
  EXPECT_EQ(graph.size_edge(), 2);
  EXPECT_EQ(graph.size().first, std::make_pair(3, 2).first);
  EXPECT_EQ(graph.size().second, std::make_pair(3, 2).second);

  graph.insert(std::make_pair(5, 6)); // Creates a set of unconnected vertices, total vertices 7.
  EXPECT_EQ(graph.size_vertex(), 7);
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

//--------------------------------------------------------------------------------------------------------------------
// Modifiers
//--------------------------------------------------------------------------------------------------------------------

TEST(test_adjacency_graph, clear) {
  AdjacencyGraph graph({{0, 1}, {2, 1}, {2, 3}, {3, 0}});

  graph.clear();
  EXPECT_TRUE(graph.empty());
  EXPECT_EQ(graph.size_edge(), 0);
  EXPECT_EQ(graph.size_vertex(), 0);
  EXPECT_EQ(graph.capacity().first, std::make_pair(4, 4).first);   // make sure we have not lost capacity.
  EXPECT_EQ(graph.capacity().second, std::make_pair(4, 4).second); // make sure we have not lost capacity.
}

TEST(test_adjacency_graph, insert) {
  AdjacencyGraph graph;

  // insert a pentagram out of order, with some edges flipped
  EXPECT_TRUE(graph.insert({0, 1}));
  EXPECT_TRUE(graph.insert({3, 4}));
  EXPECT_TRUE(graph.insert({0, 4}));
  EXPECT_TRUE(graph.insert({2, 3}));
  EXPECT_TRUE(graph.insert({1, 2}));

  // check ascending order has been maintained.
  EXPECT_EQ(graph.size_edge(), 5);
  EXPECT_EQ(graph.size_vertex(), 5);
  FOR(i_vertex, graph.size_vertex()) {
    EXPECT_EQ(graph[i_vertex].size(), 2);
    if(i_vertex == 0){
      EXPECT_EQ(graph[i_vertex][0], 1);
      EXPECT_EQ(graph[i_vertex][1], graph.size_vertex() - 1);
    }
    else if(i_vertex == graph.size_vertex() - 1) {
      EXPECT_EQ(graph[i_vertex][0], 0);
      EXPECT_EQ(graph[i_vertex][1], i_vertex - 1);
    }
    else{
      EXPECT_EQ(graph[i_vertex][0], i_vertex - 1);
      EXPECT_EQ(graph[i_vertex][1], i_vertex + 1);
    }
  }

  // Attempt to insert an existing edge
  EXPECT_FALSE(graph.insert({1, 2}));

  // Connect across
  EXPECT_TRUE(graph.insert({0, 3}));
  EXPECT_TRUE(graph.insert({0, 2}));

  // Check graph
  EXPECT_EQ(graph.size_edge(), 7);
  EXPECT_EQ(graph.size_vertex(), 5);
  EXPECT_EQ(graph[0].size(), 4);
  EXPECT_EQ(graph[1].size(), 2);
  EXPECT_EQ(graph[2].size(), 3);
  EXPECT_EQ(graph[3].size(), 3);
  EXPECT_EQ(graph[4].size(), 2);

  EXPECT_EQ(graph[0][0], 1);
  EXPECT_EQ(graph[0][1], 2);
  EXPECT_EQ(graph[0][2], 3);
  EXPECT_EQ(graph[0][3], 4);
  EXPECT_EQ(graph[1][0], 0);
  EXPECT_EQ(graph[1][1], 2);
  EXPECT_EQ(graph[2][0], 0);
  EXPECT_EQ(graph[2][1], 1);
  EXPECT_EQ(graph[2][2], 3);
  EXPECT_EQ(graph[3][0], 0);
  EXPECT_EQ(graph[3][1], 2);
  EXPECT_EQ(graph[3][2], 4);
  EXPECT_EQ(graph[4][0], 0);
  EXPECT_EQ(graph[4][1], 3);

  // death test - invalid edge
  EXPECT_DEATH(graph.insert({1, 1}), ".*");
}

TEST(test_adjacency_graph, resize) {

  // Test sizing up.
  AdjacencyGraph graph;
  graph.resize(5);
  EXPECT_EQ(graph.size_edge(), 0);
  EXPECT_EQ(graph.size_vertex(), 5);

  // Test size down, start with a fully connected pentagon graph.
  graph = AdjacencyGraph({{0, 1}, {1, 2}, {2, 3}, {3, 4}, {0, 4}, {0, 2}, {0, 3}, {1, 3}, {1, 4}, {2, 4}});

  graph.resize(3); // get rid of all two vertices - it's now a 2-simplex.
  EXPECT_EQ(graph.size_edge(), 3);
  EXPECT_EQ(graph.size_vertex(), 3);
  EXPECT_EQ(graph[0].size(), 2);
  EXPECT_EQ(graph[0][0], 1);
  EXPECT_EQ(graph[0][1], 2);
  EXPECT_EQ(graph[1].size(), 2);
  EXPECT_EQ(graph[1][0], 0);
  EXPECT_EQ(graph[1][1], 2);
  EXPECT_EQ(graph[2].size(), 2);
  EXPECT_EQ(graph[2][0], 0);
  EXPECT_EQ(graph[2][1], 1);

  // check zero sizing.
  graph.resize(0);
  EXPECT_EQ(graph.size_edge(), 0);
  EXPECT_EQ(graph.size_vertex(), 0);
}

TEST(test_adjacency_graph, swap) {
  AdjacencyGraph graph_0({{0, 1}, {1, 2},{2, 3}, {0, 3}});  // quadrilateral graph
  AdjacencyGraph graph_1({{0, 1}, {1, 2},{0, 2}});          // 2-simplex graph

  graph_0.swap(graph_1);
  EXPECT_EQ(graph_0.size_vertex(), 3);
  EXPECT_EQ(graph_0.size_edge(), 3);
  EXPECT_EQ(graph_1.size_vertex(), 4);
  EXPECT_EQ(graph_1.size_edge(), 4);

  EXPECT_DOUBLE_EQ(graph_0[0][0], 1);
  EXPECT_DOUBLE_EQ(graph_0[0][1], 2);
  EXPECT_DOUBLE_EQ(graph_0[1][0], 0);
  EXPECT_DOUBLE_EQ(graph_0[1][1], 2);
  EXPECT_DOUBLE_EQ(graph_0[2][0], 0);
  EXPECT_DOUBLE_EQ(graph_0[2][1], 1);
  EXPECT_DOUBLE_EQ(graph_1[0][0], 1);
  EXPECT_DOUBLE_EQ(graph_1[0][1], 3);
  EXPECT_DOUBLE_EQ(graph_1[1][0], 0);
  EXPECT_DOUBLE_EQ(graph_1[1][1], 2);
  EXPECT_DOUBLE_EQ(graph_1[2][0], 1);
  EXPECT_DOUBLE_EQ(graph_1[2][1], 3);
  EXPECT_DOUBLE_EQ(graph_1[3][0], 0);
  EXPECT_DOUBLE_EQ(graph_1[3][1], 2);
}

//--------------------------------------------------------------------------------------------------------------------
// Lookup
//--------------------------------------------------------------------------------------------------------------------

TEST(test_adjacency_graph, contains) {
  AdjacencyGraph graph({{0, 1}, {1, 2},{2, 3}, {0, 3}, {1, 3}});

  EXPECT_TRUE(graph.contains({0, 1}));
  EXPECT_TRUE(graph.contains({1, 2}));
  EXPECT_TRUE(graph.contains({2, 3}));
  EXPECT_TRUE(graph.contains({0, 3}));
  EXPECT_TRUE(graph.contains({1, 3}));
  EXPECT_FALSE(graph.contains({0, 2}));
}

//--------------------------------------------------------------------------------------------------------------------
// Graph Operators
//--------------------------------------------------------------------------------------------------------------------

TEST(test_adjacency_graph, reorder) {
  AdjacencyGraph graph({{0, 1}, {1, 2},{2, 3}, {3, 4}, {0, 4}});
  std::vector<std::size_t> new_numbering = {2, 3, 4, 0, 1};
  graph.reorder(new_numbering);

  std::cout<<"\n---";
  std::cout<<graph;
  std::cout<<"\n---\n";
}
