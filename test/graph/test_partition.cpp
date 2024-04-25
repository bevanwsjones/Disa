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

#include "adjacency_graph.h"
#include "adjacency_subgraph.h"
#include "generator.h"
#include "partition.h"

using namespace Disa;

TEST(test_partition, multinode_level_set_expansion) {
  std::size_t number_vertices = 10;
  Adjacency_Graph<false> graph = create_graph_line<false>(number_vertices);
  std::vector<Adjacency_Subgraph> subgraph;

  // Start with a poorly split graph.
  subgraph.push_back(Adjacency_Subgraph(graph, {0, 1, 2, 3, 4, 5, 6}));
  subgraph.push_back(Adjacency_Subgraph(graph, {7, 8, 9}));
  multinode_level_set_expansion(graph, 2, subgraph);

  // The partitions may be unequal due to the underlying level expansion algorithm, so check acceptable range.
  EXPECT_TRUE(subgraph[0].size_vertex() == 4 || subgraph[0].size_vertex() == 5 || subgraph[0].size_vertex() == 6);
  EXPECT_TRUE(subgraph[0].size_edge() == 3 || subgraph[0].size_edge() == 4 || subgraph[0].size_edge() == 5);
  EXPECT_TRUE(subgraph[1].size_vertex() == 4 || subgraph[1].size_vertex() == 5 || subgraph[1].size_vertex() == 6);
  EXPECT_TRUE(subgraph[1].size_edge() == 3 || subgraph[1].size_edge() == 4 || subgraph[1].size_edge() == 5);

  // Check uniqueness
  std::vector<bool> in_partition(number_vertices, false);
  FOR_EACH(subgraph, subgraph)
    FOR(i_local_vertex, subgraph.size_vertex())
      in_partition[subgraph.local_global(i_local_vertex)] = true;
  EXPECT_FALSE(std::find(in_partition.begin(), in_partition.end(), true) == in_partition.end());

  // Invert problem and repeat.
  subgraph.clear();
  subgraph.push_back(Adjacency_Subgraph(graph, {0, 1, 2}));
  subgraph.push_back(Adjacency_Subgraph(graph, {3, 4, 5, 6, 7, 8, 9}));
  multinode_level_set_expansion(graph, 2, subgraph);

  // The partitions may be unequal due to the underlying level expansion algorithm, so check acceptable range.
  EXPECT_TRUE(subgraph[0].size_vertex() == 4 || subgraph[0].size_vertex() == 5 || subgraph[0].size_vertex() == 6);
  EXPECT_TRUE(subgraph[0].size_edge() == 3 || subgraph[0].size_edge() == 4 || subgraph[0].size_edge() == 5);
  EXPECT_TRUE(subgraph[1].size_vertex() == 4 || subgraph[1].size_vertex() == 5 || subgraph[1].size_vertex() == 6);
  EXPECT_TRUE(subgraph[1].size_edge() == 3 || subgraph[1].size_edge() == 4 || subgraph[1].size_edge() == 5);

  // Check uniqueness
  std::fill(in_partition.begin(), in_partition.end(), false);
  FOR_EACH(subgraph, subgraph)
    FOR(i_local_vertex, subgraph.size_vertex())
      in_partition[subgraph.local_global(i_local_vertex)] = true;
  EXPECT_FALSE(std::find(in_partition.begin(), in_partition.end(), true) == in_partition.end());

  // death tests.
  std::vector<Adjacency_Subgraph> empty;
  EXPECT_DEATH(multinode_level_set_expansion(graph, 4, empty), "./*");
  EXPECT_DEATH(multinode_level_set_expansion(Adjacency_Graph<false>(), 4, subgraph), "./*");
}

TEST(LevelTraversalTest, SimpleTest) {
  std::size_t number_vertices = 40;
  auto subgraph_2 = recursive_graph_bisection(create_graph_line<false>(number_vertices), 2);
  auto subgraph_3 = recursive_graph_bisection(create_graph_line<false>(number_vertices), 3);
  auto subgraph_4 = recursive_graph_bisection(create_graph_line<false>(number_vertices), 4);
  auto subgraph_5 = recursive_graph_bisection(create_graph_line<false>(number_vertices), 5);

  EXPECT_EQ(subgraph_2.size(), 2);
  EXPECT_EQ(subgraph_2[0].size_vertex(), 20);
  EXPECT_EQ(subgraph_2[0].size_edge(), 19);
  EXPECT_EQ(subgraph_2[1].size_vertex(), 20);
  EXPECT_EQ(subgraph_2[1].size_edge(), 19);
  std::vector<bool> in_partition(number_vertices, false);
  FOR_EACH(subgraph, subgraph_2)
    FOR(i_local_vertex, subgraph.size_vertex())
      in_partition[subgraph.local_global(i_local_vertex)] = true;
  EXPECT_FALSE(std::find(in_partition.begin(), in_partition.end(), true) == in_partition.end());

  EXPECT_EQ(subgraph_3.size(), 3);
  EXPECT_EQ(subgraph_3[0].size_vertex(), 10);
  EXPECT_EQ(subgraph_3[0].size_edge(), 9);
  EXPECT_EQ(subgraph_3[1].size_vertex(), 20);
  EXPECT_EQ(subgraph_3[1].size_edge(), 19);
  EXPECT_EQ(subgraph_3[2].size_vertex(), 10);
  EXPECT_EQ(subgraph_3[2].size_edge(), 9);
  std::fill(in_partition.begin(), in_partition.end(), false);
  FOR_EACH(subgraph, subgraph_3)
    FOR(i_local_vertex, subgraph.size_vertex())
      in_partition[subgraph.local_global(i_local_vertex)] = true;
  EXPECT_FALSE(std::find(in_partition.begin(), in_partition.end(), true) == in_partition.end());

  EXPECT_EQ(subgraph_4.size(), 4);
  EXPECT_EQ(subgraph_4[0].size_vertex(), 10);
  EXPECT_EQ(subgraph_4[0].size_edge(), 9);
  EXPECT_EQ(subgraph_4[1].size_vertex(), 10);
  EXPECT_EQ(subgraph_4[1].size_edge(), 9);
  EXPECT_EQ(subgraph_4[2].size_vertex(), 10);
  EXPECT_EQ(subgraph_4[2].size_edge(), 9);
  EXPECT_EQ(subgraph_4[3].size_vertex(), 10);
  EXPECT_EQ(subgraph_4[3].size_edge(), 9);
  std::fill(in_partition.begin(), in_partition.end(), false);
  FOR_EACH(subgraph, subgraph_4)
    FOR(i_local_vertex, subgraph.size_vertex())
      in_partition[subgraph.local_global(i_local_vertex)] = true;
  EXPECT_FALSE(std::find(in_partition.begin(), in_partition.end(), true) == in_partition.end());

  EXPECT_EQ(subgraph_5.size(), 5);
  EXPECT_EQ(subgraph_5[0].size_vertex(), 5);
  EXPECT_EQ(subgraph_5[0].size_edge(), 4);
  EXPECT_EQ(subgraph_5[1].size_vertex(), 10);
  EXPECT_EQ(subgraph_5[1].size_edge(), 9);
  EXPECT_EQ(subgraph_5[2].size_vertex(), 10);
  EXPECT_EQ(subgraph_5[2].size_edge(), 9);
  EXPECT_EQ(subgraph_5[3].size_vertex(), 10);
  EXPECT_EQ(subgraph_5[3].size_edge(), 9);
  EXPECT_EQ(subgraph_5[4].size_vertex(), 5);
  EXPECT_EQ(subgraph_5[4].size_edge(), 4);
  std::fill(in_partition.begin(), in_partition.end(), false);
  FOR_EACH(subgraph, subgraph_5)
    FOR(i_local_vertex, subgraph.size_vertex())
      in_partition[subgraph.local_global(i_local_vertex)] = true;
  EXPECT_FALSE(std::find(in_partition.begin(), in_partition.end(), true) == in_partition.end());
}
