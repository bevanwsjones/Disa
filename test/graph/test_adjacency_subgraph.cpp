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

#include "adjacency_subgraph.hpp"
#include "generator.hpp"
#include "gtest/gtest.h"

using namespace Disa;

TEST(Adjacency_Subgraph, constructor_no_level) {

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
}

TEST(Adjacency_Subgraph, constructor_level) {

  Adjacency_Graph graph = create_graph_saad();
  Adjacency_Subgraph subgraph(graph, {0, 6, 8}, 2);

  EXPECT_TRUE(subgraph.is_parent(graph));
  EXPECT_FALSE(subgraph.is_parent(Adjacency_Graph<false>()));
  EXPECT_EQ(subgraph.size_vertex(), 9);

  // global 0, 6, 8, 1, 2, 7, 9, 10, 12
  // local  0, 1, 2, 3, 4, 5, 6,  7,  8

  // Check local global mapping and local check.
  EXPECT_EQ(subgraph.local_global(0), 0);
  EXPECT_EQ(subgraph.vertex_level(0), 0);
  EXPECT_TRUE(subgraph.is_local(0));
  EXPECT_EQ(subgraph.local_global(1), 6);
  EXPECT_EQ(subgraph.vertex_level(1), 0);
  EXPECT_TRUE(subgraph.is_local(1));
  EXPECT_EQ(subgraph.local_global(2), 8);
  EXPECT_EQ(subgraph.vertex_level(2), 0);
  EXPECT_TRUE(subgraph.is_local(2));
  EXPECT_EQ(subgraph.local_global(3), 1);
  EXPECT_EQ(subgraph.vertex_level(3), 1);
  EXPECT_FALSE(subgraph.is_local(3));
  EXPECT_EQ(subgraph.local_global(4), 2);
  EXPECT_EQ(subgraph.vertex_level(4), 1);
  EXPECT_FALSE(subgraph.is_local(4));
  EXPECT_EQ(subgraph.local_global(5), 7);
  EXPECT_EQ(subgraph.vertex_level(5), 1);
  EXPECT_FALSE(subgraph.is_local(5));
  EXPECT_EQ(subgraph.local_global(6), 9);
  EXPECT_EQ(subgraph.vertex_level(6), 2);
  EXPECT_FALSE(subgraph.is_local(6));
  EXPECT_EQ(subgraph.local_global(7), 10);
  EXPECT_EQ(subgraph.vertex_level(7), 2);
  EXPECT_FALSE(subgraph.is_local(7));
  EXPECT_EQ(subgraph.local_global(8), 12);
  EXPECT_EQ(subgraph.vertex_level(8), 2);
  EXPECT_FALSE(subgraph.is_local(8));

  // check individual connectivities
  EXPECT_EQ(subgraph[0].size(), 2);
  EXPECT_EQ(subgraph[0][0], 1);
  EXPECT_EQ(subgraph[0][1], 2);

  EXPECT_EQ(subgraph[1].size(), 4);
  EXPECT_EQ(subgraph[1][0], 0);
  EXPECT_EQ(subgraph[1][1], 2);
  EXPECT_EQ(subgraph[1][2], 4);
  EXPECT_EQ(subgraph[1][3], 5);

  EXPECT_EQ(subgraph[2].size(), 4);
  EXPECT_EQ(subgraph[2][0], 0);
  EXPECT_EQ(subgraph[2][1], 1);
  EXPECT_EQ(subgraph[2][2], 3);
  EXPECT_EQ(subgraph[2][3], 5);

  EXPECT_EQ(subgraph[3].size(), 4);
  EXPECT_EQ(subgraph[3][0], 2);
  EXPECT_EQ(subgraph[3][1], 5);
  EXPECT_EQ(subgraph[3][2], 7);
  EXPECT_EQ(subgraph[3][3], 8);

  EXPECT_EQ(subgraph[4].size(), 3);
  EXPECT_EQ(subgraph[4][0], 1);
  EXPECT_EQ(subgraph[4][1], 5);
  EXPECT_EQ(subgraph[4][2], 6);

  EXPECT_EQ(subgraph[5].size(), 6);
  EXPECT_EQ(subgraph[5][0], 1);
  EXPECT_EQ(subgraph[5][1], 2);
  EXPECT_EQ(subgraph[5][2], 3);
  EXPECT_EQ(subgraph[5][3], 4);
  EXPECT_EQ(subgraph[5][4], 6);
  EXPECT_EQ(subgraph[5][5], 7);

  EXPECT_EQ(subgraph[6].size(), 3);
  EXPECT_EQ(subgraph[6][0], 4);
  EXPECT_EQ(subgraph[6][1], 5);
  EXPECT_EQ(subgraph[6][2], 7);

  EXPECT_EQ(subgraph[7].size(), 4);
  EXPECT_EQ(subgraph[7][0], 3);
  EXPECT_EQ(subgraph[7][1], 5);
  EXPECT_EQ(subgraph[7][2], 6);
  EXPECT_EQ(subgraph[7][3], 8);

  EXPECT_EQ(subgraph[8].size(), 2);
  EXPECT_EQ(subgraph[8][0], 3);
  EXPECT_EQ(subgraph[8][1], 7);
}

TEST(Adjacency_Subgraph, constructor_death) {
  Adjacency_Graph<false> graph = create_graph_structured<false>(2);
  EXPECT_DEATH(Adjacency_Subgraph(graph, {0, 1, 2, 3, 4}), "./*");
  EXPECT_DEATH(Adjacency_Subgraph(graph, {0, 0, 1}), "./*");
  EXPECT_DEATH(Adjacency_Subgraph(graph, {0, 5}), "./*");
}

//----------------------------------------------------------------------------------------------------------------------
// Element Access
//----------------------------------------------------------------------------------------------------------------------

TEST(Adjacency_Subgraph, data) {

  Adjacency_Subgraph subgraph;

  // Empty subgraph.
  auto data = subgraph.data();
  EXPECT_EQ(std::get<0>(data), nullptr);
  EXPECT_EQ(std::get<1>(data), nullptr);
  EXPECT_EQ(std::get<2>(data), nullptr);
  EXPECT_EQ(std::get<3>(data), nullptr);

  subgraph.resize(4);  // we have no edges yet, so the vertex list remains a nullptr.
  EXPECT_EQ(std::get<0>(data), nullptr);
  EXPECT_EQ(std::get<1>(data), nullptr);
  EXPECT_EQ(std::get<2>(data), nullptr);
  EXPECT_EQ(std::get<3>(data), nullptr);

  // Create a subgraph, but with no levels.
  Adjacency_Graph graph = create_graph_saad();
  subgraph = Adjacency_Subgraph(graph, {0, 8, 6});
  data = subgraph.data();
  EXPECT_NE(std::get<0>(data), nullptr);
  EXPECT_NE(std::get<1>(data), nullptr);
  EXPECT_NE(std::get<2>(data), nullptr);
  EXPECT_EQ(std::get<3>(data), nullptr);  // there are no levels

  // Create a subgraph with levels.
  subgraph = Adjacency_Subgraph(graph, {0, 8, 6}, 1);
  data = subgraph.data();
  EXPECT_NE(std::get<0>(data), nullptr);
  EXPECT_NE(std::get<1>(data), nullptr);
  EXPECT_NE(std::get<2>(data), nullptr);
  EXPECT_NE(std::get<3>(data), nullptr);
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
  EXPECT_TRUE(subgraph.is_parent(Adjacency_Graph<false>()));
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
  Adjacency_Graph<false> graph_0 = create_graph_saad();
  Adjacency_Graph<false> graph_1 = create_graph_structured<false>(3);
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
  Adjacency_Graph<false> graph = create_graph_structured<false>(3);

  // Test degree on primary subgraph.
  Adjacency_Subgraph subgraph(graph, {1, 3, 4, 5, 7});
  EXPECT_EQ(subgraph.degree(0), 1);
  EXPECT_EQ(subgraph.degree(1), 1);
  EXPECT_EQ(subgraph.degree(2), 4);
  EXPECT_EQ(subgraph.degree(3), 1);
  EXPECT_EQ(subgraph.degree(4), 1);

  // Test degree on both level and primary parts of the subgraph.
  subgraph = Adjacency_Subgraph(graph, {4}, 1);
  EXPECT_EQ(subgraph.degree(0), 4);
  EXPECT_EQ(subgraph.degree(1), 1);
  EXPECT_EQ(subgraph.degree(2), 1);
  EXPECT_EQ(subgraph.degree(3), 1);
  EXPECT_EQ(subgraph.degree(4), 1);
}

TEST(Adjacency_Subgraph, is_local) {
  Adjacency_Graph<false> graph = create_graph_structured<false>(3);

  // Test is_local on primary subgraph.
  Adjacency_Subgraph subgraph(graph, {1, 3, 4, 5, 7});
  EXPECT_TRUE(subgraph.is_local(0));
  EXPECT_TRUE(subgraph.is_local(1));
  EXPECT_TRUE(subgraph.is_local(2));
  EXPECT_TRUE(subgraph.is_local(3));
  EXPECT_TRUE(subgraph.is_local(4));

  // Test is_local on both level and primary parts of the subgraph.
  subgraph = Adjacency_Subgraph(graph, {4}, 1);
  EXPECT_TRUE(subgraph.is_local(0));
  EXPECT_FALSE(subgraph.is_local(1));
  EXPECT_FALSE(subgraph.is_local(2));
  EXPECT_FALSE(subgraph.is_local(3));
  EXPECT_FALSE(subgraph.is_local(4));

  EXPECT_DEATH(subgraph.is_local(10) == false, "./*");
}

TEST(Adjacency_Subgraph, is_parent) {
  Adjacency_Subgraph subgraph;
  Adjacency_Graph graph = create_graph_structured<false>(3);
  EXPECT_TRUE(subgraph.is_parent(Adjacency_Graph<false>()));
  EXPECT_FALSE(subgraph.is_parent(graph));

  subgraph = Adjacency_Subgraph(graph, {1, 3, 4, 5, 7});
  EXPECT_FALSE(subgraph.is_parent(Adjacency_Graph<false>()));
  EXPECT_TRUE(subgraph.is_parent(graph));
}

TEST(Adjacency_Subgraph, local_global) {
  Adjacency_Graph graph = create_graph_structured<false>(3);

  // Test local_global on primary subgraph.
  Adjacency_Subgraph subgraph(graph, {1, 3, 4, 5, 7});
  EXPECT_EQ(subgraph.local_global(0), 1);
  EXPECT_EQ(subgraph.local_global(1), 3);
  EXPECT_EQ(subgraph.local_global(2), 4);
  EXPECT_EQ(subgraph.local_global(3), 5);
  EXPECT_EQ(subgraph.local_global(4), 7);

  // Test local_global on both level and primary parts of the subgraph.
  subgraph = Adjacency_Subgraph(graph, {4}, 1);
  EXPECT_EQ(subgraph.local_global(0), 4);
  EXPECT_EQ(subgraph.local_global(1), 1);
  EXPECT_EQ(subgraph.local_global(2), 3);
  EXPECT_EQ(subgraph.local_global(3), 5);
  EXPECT_EQ(subgraph.local_global(4), 7);

  EXPECT_DEATH(subgraph.local_global(7) == 0, "./*");
}

TEST(Adjacency_Subgraph, reorder) {
  // Check empty graph calls
  Adjacency_Subgraph subgraph;
  EXPECT_NO_FATAL_FAILURE(subgraph.reorder(std::vector<std::size_t>()));
  EXPECT_DEATH(subgraph.reorder(std::vector<std::size_t>({0, 1})), "./*");

  Adjacency_Graph graph = create_graph_structured<false>(3);  // pentagon with diagonal connection.
  subgraph = Adjacency_Subgraph(graph, {4}, 1);
  subgraph.update_levels(graph, 2);

  // Check death conditions
  EXPECT_DEATH(subgraph.reorder(std::vector<std::size_t>({0, 1})), "./*");  // check under size crashes
  EXPECT_DEATH(subgraph.reorder(std::vector<std::size_t>({10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0})),
               "./*");  // check oversize crashes
  EXPECT_DEATH(subgraph.reorder(std::vector<std::size_t>({9, 8, 7, 6, 5, 4, 8, 2, 1, 0})),
               "./*");  // check duplicate crashes
  EXPECT_DEATH(subgraph.reorder(std::vector<std::size_t>({9, 8, 7, 6, 5, 4, 3, 10, 1, 0})),
               "./*");  // check index > size

  // Actual reordering check, needs to re-order and ensure sorted lists are created.
  //                         global index   4  1  3  5  7  0  2  6  8
  //                            old index   0  1  2  3  4  5  6  7  8
  std::vector<std::size_t> new_numbering = {4, 1, 3, 5, 7, 0, 2, 6, 8};
  Adjacency_Subgraph old_subgraph = subgraph.reorder(new_numbering);
  EXPECT_TRUE(subgraph.is_parent(graph));
  EXPECT_EQ(subgraph.size().first, 9);
  EXPECT_EQ(subgraph.size().second, 12);

  // 0 : 1, 2, 3, 4  -> 4 : 1, 3, 5, 7
  EXPECT_EQ(subgraph[4].size(), 4);
  EXPECT_EQ(subgraph[4][0], 1);
  EXPECT_EQ(subgraph[4][1], 3);
  EXPECT_EQ(subgraph[4][2], 5);
  EXPECT_EQ(subgraph[4][3], 7);
  EXPECT_EQ(subgraph.vertex_level(4), 0);
  EXPECT_EQ(subgraph.local_global(4), 4);

  // 1 : 0, 2, 4     -> 1 : 0, 2, 4
  EXPECT_EQ(subgraph[1].size(), 3);
  EXPECT_EQ(subgraph[1][0], 0);
  EXPECT_EQ(subgraph[1][1], 2);
  EXPECT_EQ(subgraph[1][2], 4);
  EXPECT_EQ(subgraph.vertex_level(1), 1);
  EXPECT_EQ(subgraph.local_global(1), 1);

  // 2 : 4, 5, 7     -> 3 : 0, 4, 6
  EXPECT_EQ(subgraph[3].size(), 3);
  EXPECT_EQ(subgraph[3][0], 0);
  EXPECT_EQ(subgraph[3][1], 4);
  EXPECT_EQ(subgraph[3][2], 6);
  EXPECT_EQ(subgraph.vertex_level(3), 1);
  EXPECT_EQ(subgraph.local_global(3), 3);

  // 3 : 6, 0, 8     -> 5 : 2, 4, 8
  EXPECT_EQ(subgraph[5].size(), 3);
  EXPECT_EQ(subgraph[5][0], 2);
  EXPECT_EQ(subgraph[5][1], 4);
  EXPECT_EQ(subgraph[5][2], 8);
  EXPECT_EQ(subgraph.vertex_level(5), 1);
  EXPECT_EQ(subgraph.local_global(5), 5);

  // 4 : 0, 7, 8     -> 7 : 4, 6, 8
  EXPECT_EQ(subgraph[7].size(), 3);
  EXPECT_EQ(subgraph[7][0], 4);
  EXPECT_EQ(subgraph[7][1], 6);
  EXPECT_EQ(subgraph[7][2], 8);
  EXPECT_EQ(subgraph.vertex_level(7), 1);
  EXPECT_EQ(subgraph.local_global(7), 7);

  // 5 : 1, 2        -> 0 : 1, 3
  EXPECT_EQ(subgraph[0].size(), 2);
  EXPECT_EQ(subgraph[0][0], 1);
  EXPECT_EQ(subgraph[0][1], 3);
  EXPECT_EQ(subgraph.vertex_level(0), 2);
  EXPECT_EQ(subgraph.local_global(0), 0);

  // 6 : 1, 3        -> 2 : 1, 5
  EXPECT_EQ(subgraph[2].size(), 2);
  EXPECT_EQ(subgraph[2][0], 1);
  EXPECT_EQ(subgraph[2][1], 5);
  EXPECT_EQ(subgraph.vertex_level(2), 2);
  EXPECT_EQ(subgraph.local_global(2), 2);

  // 7 : 2, 4         -> 6 : 3, 7
  EXPECT_EQ(subgraph[6].size(), 2);
  EXPECT_EQ(subgraph[6][0], 3);
  EXPECT_EQ(subgraph[6][1], 7);
  EXPECT_EQ(subgraph.vertex_level(6), 2);
  EXPECT_EQ(subgraph.local_global(6), 6);

  // 8 : 3, 4         -> 8 : 5, 7
  EXPECT_EQ(subgraph[8].size(), 2);
  EXPECT_EQ(subgraph[8][0], 5);
  EXPECT_EQ(subgraph[8][1], 7);
  EXPECT_EQ(subgraph.vertex_level(8), 2);
  EXPECT_EQ(subgraph.local_global(8), 8);
}

TEST(Adjacency_Subgraph, update_levels_add) {
  Adjacency_Graph graph = create_graph_structured<false>(3);
  Adjacency_Subgraph subgraph(graph, {4});
  std::shared_ptr<std::vector<std::size_t>> i_global_local = std::make_shared<std::vector<std::size_t>>();

  subgraph.update_levels(graph, 1, i_global_local);
  EXPECT_FALSE(i_global_local == nullptr);
  EXPECT_FALSE(i_global_local->empty());
  EXPECT_TRUE(subgraph.is_parent(graph));

  // Check graph details
  EXPECT_EQ(subgraph.size_vertex(), 5);
  EXPECT_EQ(subgraph.size_edge(), 4);
  EXPECT_EQ(subgraph.vertex_level(0), 0);
  EXPECT_EQ(subgraph.vertex_level(1), 1);
  EXPECT_EQ(subgraph.vertex_level(2), 1);
  EXPECT_EQ(subgraph.vertex_level(3), 1);
  EXPECT_EQ(subgraph.vertex_level(4), 1);
  FOR(i_vertex, subgraph.size_vertex())
  EXPECT_EQ((*i_global_local)[subgraph.local_global(i_vertex)], i_vertex);
  EXPECT_EQ((*i_global_local)[0], std::numeric_limits<std::size_t>::max());
  EXPECT_EQ((*i_global_local)[2], std::numeric_limits<std::size_t>::max());
  EXPECT_EQ((*i_global_local)[6], std::numeric_limits<std::size_t>::max());
  EXPECT_EQ((*i_global_local)[8], std::numeric_limits<std::size_t>::max());

  subgraph.update_levels(graph, 2, i_global_local);
  EXPECT_TRUE(subgraph.is_parent(graph));
  EXPECT_EQ(subgraph.size_vertex(), 9);
  EXPECT_EQ(subgraph.size_edge(), 12);

  // Check graph details
  EXPECT_EQ(subgraph.vertex_level(0), 0);
  EXPECT_EQ(subgraph.vertex_level(1), 1);
  EXPECT_EQ(subgraph.vertex_level(2), 1);
  EXPECT_EQ(subgraph.vertex_level(3), 1);
  EXPECT_EQ(subgraph.vertex_level(4), 1);
  EXPECT_EQ(subgraph.vertex_level(5), 2);
  EXPECT_EQ(subgraph.vertex_level(6), 2);
  EXPECT_EQ(subgraph.vertex_level(7), 2);
  EXPECT_EQ(subgraph.vertex_level(8), 2);
  FOR(i_vertex, subgraph.size_vertex())
  EXPECT_EQ((*i_global_local)[subgraph.local_global(i_vertex)], i_vertex);

  EXPECT_DEATH(subgraph.update_levels(Adjacency_Graph<false>(), 1, i_global_local), "./*");
  EXPECT_NO_THROW(Adjacency_Subgraph(graph, {4}).update_levels(graph, 1););  // ensure default does not throw.
}

TEST(Adjacency_Subgraph, update_levels_remove) {
  Adjacency_Graph<false> graph = create_graph_structured<false>(3);
  Adjacency_Subgraph subgraph(graph, {4}, 2);
  std::shared_ptr<std::vector<std::size_t>> i_global_local = std::make_shared<std::vector<std::size_t>>();

  subgraph.update_levels(graph, 1, i_global_local);
  EXPECT_FALSE(i_global_local == nullptr);
  EXPECT_FALSE(i_global_local->empty());
  EXPECT_TRUE(subgraph.is_parent(graph));

  // Check graph details
  EXPECT_EQ(subgraph.size_vertex(), 5);
  EXPECT_EQ(subgraph.size_edge(), 4);
  EXPECT_EQ(subgraph.vertex_level(0), 0);
  EXPECT_EQ(subgraph.vertex_level(1), 1);
  EXPECT_EQ(subgraph.vertex_level(2), 1);
  EXPECT_EQ(subgraph.vertex_level(3), 1);
  EXPECT_EQ(subgraph.vertex_level(4), 1);
  FOR(i_vertex, subgraph.size_vertex())
  EXPECT_EQ((*i_global_local)[subgraph.local_global(i_vertex)], i_vertex);
  EXPECT_EQ((*i_global_local)[0], std::numeric_limits<std::size_t>::max());
  EXPECT_EQ((*i_global_local)[2], std::numeric_limits<std::size_t>::max());
  EXPECT_EQ((*i_global_local)[6], std::numeric_limits<std::size_t>::max());
  EXPECT_EQ((*i_global_local)[8], std::numeric_limits<std::size_t>::max());

  subgraph.update_levels(graph, 0);
  EXPECT_TRUE(subgraph.is_parent(graph));
  EXPECT_EQ(subgraph.size_vertex(), 1);
  EXPECT_EQ(subgraph.size_edge(), 0);

  // Check graph details
  EXPECT_EQ(subgraph.vertex_level(0), 0);
  EXPECT_EQ(subgraph.local_global(0), 4);

  EXPECT_DEATH(subgraph.update_levels(Adjacency_Graph<false>(), 1, i_global_local), "./*");
  EXPECT_NO_THROW(Adjacency_Subgraph(graph, {4}, 2).update_levels(graph, 1););  // ensure default does not throw.
}

TEST(Adjacency_Subgraph, vertex_level) {
  Adjacency_Graph<false> graph = create_graph_structured<false>(3);

  // Test local_global on primary subgraph.
  Adjacency_Subgraph subgraph(graph, {1, 3, 4, 5, 7});
  EXPECT_EQ(subgraph.vertex_level(0), 0);
  EXPECT_EQ(subgraph.vertex_level(1), 0);
  EXPECT_EQ(subgraph.vertex_level(2), 0);
  EXPECT_EQ(subgraph.vertex_level(3), 0);
  EXPECT_EQ(subgraph.vertex_level(4), 0);

  // Test local_global on both level and primary parts of the subgraph.
  std::shared_ptr<std::vector<std::size_t>> i_global_local = std::make_shared<std::vector<std::size_t>>();
  subgraph = Adjacency_Subgraph(graph, {4});
  subgraph.update_levels(graph, 2, i_global_local);
  EXPECT_EQ(subgraph.vertex_level((*i_global_local)[0]), 2);
  EXPECT_EQ(subgraph.vertex_level((*i_global_local)[1]), 1);
  EXPECT_EQ(subgraph.vertex_level((*i_global_local)[2]), 2);
  EXPECT_EQ(subgraph.vertex_level((*i_global_local)[3]), 1);
  EXPECT_EQ(subgraph.vertex_level((*i_global_local)[4]), 0);
  EXPECT_EQ(subgraph.vertex_level((*i_global_local)[5]), 1);
  EXPECT_EQ(subgraph.vertex_level((*i_global_local)[6]), 2);
  EXPECT_EQ(subgraph.vertex_level((*i_global_local)[7]), 1);
  EXPECT_EQ(subgraph.vertex_level((*i_global_local)[8]), 2);
  EXPECT_DEATH(subgraph.vertex_level(10) == 0, "./*");
}
