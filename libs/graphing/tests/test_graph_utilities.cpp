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

#include "graph_utilities.h"
#include "test_graph.h"

using namespace Disa;

// Unit test for LevelTraversal using Google Test
TEST(test_graph_utilities, level_traversal_single_start_vertex) {
  Adjacency_Graph graph_saad = create_graph_saad();

  std::vector<std::size_t> expected_levels = {0, 2, 2, 4, 4, 6, 1, 2, 1, 3, 3, 4, 3, 5, 5};
  EXPECT_EQ(expected_levels, level_traversal(graph_saad, 0));

  expected_levels = std::vector<std::size_t>({3, 1, 3, 1, 2, 3, 3, 2, 2, 2, 1, 1, 0, 2, 2});
  EXPECT_EQ(expected_levels, level_traversal(graph_saad, 12));
}

TEST(test_graph_utilities, level_traversal_multi_start_vertex) {
  Adjacency_Graph graph_saad = create_graph_saad();

  std::vector<std::size_t> levels(graph_saad.size_vertex(), std::numeric_limits<std::size_t>::max());
  levels[0] = 0;
  levels[1] = 0;
  levels[3] = 0;
  std::queue<size_t> queue({0, 1, 3});
  level_traversal(graph_saad, queue, levels);
  std::vector<std::size_t> expected_levels = {0, 0, 2, 0, 2, 2, 1, 1, 1, 2, 1, 1, 1, 2, 1};
  EXPECT_EQ(expected_levels, levels);
}

TEST(test_graph_utilities, level_traversal_end_at_level) {
  Adjacency_Graph graph_saad = create_graph_saad();

  const std::size_t& size_t_max = std::numeric_limits<std::size_t>::max();
  std::vector<std::size_t> expected_levels = {0, 2, 2, 4, 4, size_t_max, 1, 2, 1, 3, 3, 4, 3, 5, 5};
  EXPECT_EQ(expected_levels, level_traversal(graph_saad, 0, 5));


  std::vector<std::size_t> levels(graph_saad.size_vertex(), std::numeric_limits<std::size_t>::max());
  levels[0] = 0;
  levels[1] = 0;
  levels[3] = 0;
  std::queue<size_t> queue({0, 1, 3});
  level_traversal(graph_saad, queue, levels, 1);
  expected_levels = {0, 0, size_t_max, 0, size_t_max, size_t_max, 1, 1, 1, size_t_max, 1, 1, 1, size_t_max, 1};
  EXPECT_EQ(expected_levels, levels);
}

TEST(test_graph_utilities, level_traversal_death){

  Adjacency_Graph graph_saad = create_graph_saad();

  // Single Vertex starts
  EXPECT_DEATH(level_traversal(Adjacency_Graph(), 1), "./*");
  EXPECT_DEATH(level_traversal(graph_saad, 50), "./*");

  // Multi Vertex Starts
  std::queue<size_t> queue;
  std::vector<std::size_t> levels_death(graph_saad.size_vertex(), std::numeric_limits<std::size_t>::max());
  EXPECT_DEATH(level_traversal(Adjacency_Graph(), queue, levels_death), "./*");
  levels_death.pop_back();
  EXPECT_DEATH(level_traversal(graph_saad, queue, levels_death), "./*");
  levels_death = std::vector<std::size_t>(graph_saad.size_vertex(), std::numeric_limits<std::size_t>::max());
  levels_death[0] -= 1;
  EXPECT_DEATH(level_traversal(graph_saad, queue, levels_death), "./*");
}

TEST(test_graph_utilities, pseudo_peripheral_vertex) {

  Adjacency_Graph graph_saad = create_graph_saad();

  // Check to get the end vertices.
  EXPECT_EQ(pseudo_peripheral_vertex(graph_saad, 11), 5);
  EXPECT_EQ(pseudo_peripheral_vertex(graph_saad, 2), 0);

  // Insert a new edge, then search from the otheside of the graph, this should be the new peripheral vertex.
  graph_saad.insert({0, 15});
  EXPECT_EQ(pseudo_peripheral_vertex(graph_saad, 5), 15);

  // Death test.
  EXPECT_DEATH(pseudo_peripheral_vertex(Adjacency_Graph()), "./*");
  EXPECT_DEATH(pseudo_peripheral_vertex(graph_saad, 16), "./*");
}