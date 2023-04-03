// ---------------------------------------------------------------------------------------------------------------------
// MIT License
// Copyright (c) 2022 Bevan W.S. Jones
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
// Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ---------------------------------------------------------------------------------------------------------------------
// File Name:
// Description:
// ---------------------------------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "adjacency_subgraph.h"
#include "test_graph.h"

using namespace Disa;

TEST(Adjacency_Subgraph, constructor) {

  Adjacency_Graph graph = create_graph_saad();
  Adjacency_Subgraph subgraph(graph, {0, 6, 8, 13, 14});

  EXPECT_EQ(subgraph.size_vertex(), 5);

  EXPECT_EQ(subgraph[0].size(), 2);
  EXPECT_EQ(subgraph[0][0], 1);
  EXPECT_EQ(subgraph[0][1], 2);
  EXPECT_TRUE(subgraph.is_local(0));
  EXPECT_EQ(subgraph.local_global(0), 0);

  EXPECT_EQ(subgraph[1].size(), 2);
  EXPECT_EQ(subgraph[1][0], 0);
  EXPECT_EQ(subgraph[1][1], 2);
  EXPECT_TRUE(subgraph.is_local(1));
  EXPECT_EQ(subgraph.local_global(1), 6);

  EXPECT_EQ(subgraph[2].size(), 2);
  EXPECT_EQ(subgraph[2][0], 0);
  EXPECT_EQ(subgraph[2][1], 1);
  EXPECT_TRUE(subgraph.is_local(2));
  EXPECT_EQ(subgraph.local_global(2), 8);

  EXPECT_EQ(subgraph[3].size(), 1);
  EXPECT_EQ(subgraph[3][0], 4);
  EXPECT_TRUE(subgraph.is_local(3));
  EXPECT_EQ(subgraph.local_global(3), 13);

  EXPECT_EQ(subgraph[4].size(), 1);
  EXPECT_EQ(subgraph[4][0], 3);
  EXPECT_TRUE(subgraph.is_local(4));
  EXPECT_EQ(subgraph.local_global(4), 14);
};

//----------------------------------------------------------------------------------------------------------------------
// Element Access
//----------------------------------------------------------------------------------------------------------------------

TEST(Adjacency_Subgraph, data) {

  Adjacency_Subgraph subgraph;

  auto data = subgraph.data();
  EXPECT_EQ(std::get<0>(data), nullptr);
  EXPECT_EQ(std::get<1>(data), nullptr);
  EXPECT_EQ(std::get<2>(data), nullptr);
  EXPECT_EQ(std::get<3>(data), nullptr);

  subgraph.resize(4); // we have no edges yet, so the vertex list remains a nullptr.
  EXPECT_EQ(std::get<0>(data), nullptr);
  EXPECT_EQ(std::get<1>(data), nullptr);
  EXPECT_EQ(std::get<2>(data), nullptr);
  EXPECT_EQ(std::get<3>(data), nullptr);

  Adjacency_Graph graph = create_graph_saad();
  subgraph = Adjacency_Subgraph(graph, {0, 1, 2});
  data = subgraph.data();

  EXPECT_NE(std::get<0>(data), nullptr);
  EXPECT_NE(std::get<1>(data), nullptr);
  EXPECT_NE(std::get<2>(data), nullptr);
  //TODO - FIX :  EXPECT_NE(std::get<3>(data), nullptr);
}

//----------------------------------------------------------------------------------------------------------------------
// Modifiers
//----------------------------------------------------------------------------------------------------------------------

TEST(Adjacency_Subgraph, clear) {
  Adjacency_Graph graph = create_graph_saad();
  Adjacency_Subgraph subgraph(graph, {0, 6, 8, 13, 14});
  EXPECT_FALSE(subgraph.empty());
  auto capacities = subgraph.capacity();

  subgraph.clear();
  EXPECT_TRUE(subgraph.empty());
  EXPECT_TRUE(subgraph.is_parent(Adjacency_Graph()));
  EXPECT_EQ(std::get<0>(subgraph.capacity()), std::get<0>(capacities));
  EXPECT_EQ(std::get<1>(subgraph.capacity()), std::get<1>(capacities));
  EXPECT_EQ(std::get<2>(subgraph.capacity()), std::get<2>(capacities));
  EXPECT_EQ(std::get<3>(subgraph.capacity()), std::get<3>(capacities));
}

TEST(Adjacency_Subgraph, resize) {
  Adjacency_Graph graph = create_graph_saad();
  Adjacency_Subgraph subgraph(graph, {0, 6, 8, 13, 14});
  std::size_t size_edge = subgraph.size_edge();

  // Expand the graph
  subgraph.resize(7);
  EXPECT_FALSE(subgraph.is_parent(graph));
  EXPECT_EQ(subgraph.size_vertex(), 7);
  EXPECT_EQ(subgraph.size_edge(), size_edge);
  EXPECT_EQ(subgraph.local_global(0), 0);
  EXPECT_EQ(subgraph.local_global(1), 6);
  EXPECT_EQ(subgraph.local_global(2), 8);
  EXPECT_EQ(subgraph.local_global(3), 13);
  EXPECT_EQ(subgraph.local_global(4), 14);
  // Vertices after are in an uninitialised state.

  subgraph = Adjacency_Subgraph(graph, {0, 6, 8, 13, 14});

  // Keep the subgraph the same size.
  subgraph.resize(5);
  EXPECT_TRUE(subgraph.is_parent(graph));
  EXPECT_EQ(subgraph.size_vertex(), 5);
  EXPECT_EQ(subgraph.size_edge(), size_edge);
  EXPECT_EQ(subgraph.local_global(0), 0);
  EXPECT_EQ(subgraph.local_global(1), 6);
  EXPECT_EQ(subgraph.local_global(2), 8);
  EXPECT_EQ(subgraph.local_global(3), 13);
  EXPECT_EQ(subgraph.local_global(4), 14);

  // Shrink the subgraph the same size.
  subgraph.resize(3);
  EXPECT_TRUE(subgraph.is_parent(graph));
  EXPECT_EQ(subgraph.size_vertex(), 3);
  EXPECT_EQ(subgraph.size_edge(), 3);
  EXPECT_EQ(subgraph.local_global(0), 0);
  EXPECT_EQ(subgraph.local_global(1), 6);
  EXPECT_EQ(subgraph.local_global(2), 8);
}

TEST(Adjacency_Subgraph, swap) {
  Adjacency_Graph graph_0 = create_graph_saad();
  Adjacency_Graph graph_1 = create_graph_structured(3);
  Adjacency_Subgraph subgraph_0(graph_0, {0, 6, 8, 1, 7});
  Adjacency_Subgraph subgraph_1(graph_1, {4, 5, 7, 8});

  subgraph_0.swap(subgraph_1);
  EXPECT_TRUE(subgraph_0.is_parent(graph_1));
  EXPECT_EQ(subgraph_0.size_vertex(), 4);
  EXPECT_EQ(subgraph_0.size_edge(), 4);
  EXPECT_EQ(subgraph_0.local_global(0), 4);
  EXPECT_EQ(subgraph_0.local_global(1), 5);
  EXPECT_EQ(subgraph_0.local_global(2), 7);
  EXPECT_EQ(subgraph_0.local_global(3), 8);

  EXPECT_TRUE(subgraph_1.is_parent(graph_0));
  EXPECT_EQ(subgraph_1.size_vertex(), 5);
  EXPECT_EQ(subgraph_1.size_edge(), 7);
  EXPECT_EQ(subgraph_1.local_global(0), 0);
  EXPECT_EQ(subgraph_1.local_global(1), 6);
  EXPECT_EQ(subgraph_1.local_global(2), 8);
  EXPECT_EQ(subgraph_1.local_global(3), 1);
  EXPECT_EQ(subgraph_1.local_global(4), 7);
}

//----------------------------------------------------------------------------------------------------------------------
// Graph Operators
//----------------------------------------------------------------------------------------------------------------------

TEST(Adjacency_Subgraph, degree) {
  EXPECT_TRUE(false);
}

TEST(Adjacency_Subgraph, is_local) {
  EXPECT_TRUE(false);
}

TEST(Adjacency_Subgraph, local_global) {
  EXPECT_TRUE(false);
}

TEST(Adjacency_Subgraph, vertex_level) {
  EXPECT_TRUE(false);
}














