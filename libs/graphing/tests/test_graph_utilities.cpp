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

#include "generator.h"
#include "graph_utilities.h"

using namespace Disa;

// Unit test for LevelTraversal using Google Test
TEST(test_graph_utilities, level_traversal_single_start_vertex) {
  Adjacency_Graph<false> graph_saad = create_graph_saad();

  std::vector<std::size_t> expected_levels = {0, 2, 2, 4, 4, 6, 1, 2, 1, 3, 3, 4, 3, 5, 5};
  EXPECT_EQ(expected_levels, level_traversal(graph_saad, 0));

  expected_levels = std::vector<std::size_t>({3, 1, 3, 1, 2, 3, 3, 2, 2, 2, 1, 1, 0, 2, 2});
  EXPECT_EQ(expected_levels, level_traversal(graph_saad, 12));
}

TEST(test_graph_utilities, level_traversal_multi_start_vertex) {
  Adjacency_Graph<false> graph_saad = create_graph_saad();

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
  Adjacency_Graph<false> graph_saad = create_graph_saad();

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

  Adjacency_Graph<false> graph_saad = create_graph_saad();

  // Single Vertex starts
  EXPECT_DEATH(level_traversal(Adjacency_Graph<false>(), 1), "./*");
  EXPECT_DEATH(level_traversal(graph_saad, 50), "./*");

  // Multi Vertex Starts
  std::queue<size_t> queue;
  std::vector<std::size_t> levels_death(graph_saad.size_vertex(), std::numeric_limits<std::size_t>::max());
  EXPECT_DEATH(level_traversal(Adjacency_Graph<false>(), queue, levels_death), "./*");
  levels_death.pop_back();
  EXPECT_DEATH(level_traversal(graph_saad, queue, levels_death), "./*");
  levels_death = std::vector<std::size_t>(graph_saad.size_vertex(), std::numeric_limits<std::size_t>::max());
  levels_death[0] -= 1;
  EXPECT_DEATH(level_traversal(graph_saad, queue, levels_death), "./*");
}

TEST(test_graph_utilities, level_expansion) {

  // Square grid, with 4 seed points.
  Adjacency_Graph<false> graph = create_graph_structured<false>(5);
  std::vector<std::size_t> colors;
  level_expansion(graph, {6, 8, 16, 18}, colors);
  std::vector<std::size_t> hand_computed_answer({0, 0, 1, 1, 1,
                                                 0, 0, 0, 1, 1,
                                                 2, 0, 2, 1, 3,
                                                 2, 2, 2, 3, 3,
                                                 2, 2, 3, 3, 3});
  EXPECT_EQ(colors, hand_computed_answer);

  // Second grid, with 3 'reverse' seed points.
  Adjacency_Subgraph subgraph(graph, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                                      15, 16, 17, 18, 19, 20, 21, 22, 23, 24});
  level_expansion(subgraph, {20, 12, 4}, colors);
  hand_computed_answer = std::vector<std::size_t>({1, 1, 2, 2, 2,
                                                   1, 1, 1, 2, 2,
                                                   1, 1, 1, 1, 2,
                                                   0, 1, 1, 1, 1,
                                                   0, 0, 1, 1, 1});
  EXPECT_EQ(colors, hand_computed_answer);

  // Death test.
  EXPECT_DEATH(level_expansion(Adjacency_Graph<false>(), {6, 8, 16, 18}, colors), "./*");
  EXPECT_DEATH(level_expansion(graph, {}, colors), "./*");
  EXPECT_DEATH(level_expansion(graph, {36}, colors), "./*");
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
  EXPECT_DEATH(pseudo_peripheral_vertex(Adjacency_Graph<false>()), "./*");
  EXPECT_DEATH(pseudo_peripheral_vertex(graph_saad, 16), "./*");
}

TEST(test_graph_utilities, eccentricity_graph) {
  Adjacency_Graph graph = create_graph_structured<false>(5);
  std::vector<std::vector<std::size_t> > eccentricity;
  eccentricity_graph(graph, eccentricity);

  // Weak test, ensure we are getting at least the values from the individual eccentricity_vertex_breadth_first calls.
  FOR(i_vertex, graph.size_vertex()) {
    std::vector<std::size_t> distance_answer;
    eccentricity_vertex_breadth_first(graph, i_vertex, distance_answer, i_vertex + 1);
    EXPECT_EQ(eccentricity[i_vertex], distance_answer);
  }
}

TEST(test_graph_utilities, eccentricity_vertex_breadth_first) {

  // Perform a test on a graph, and starting at 0.
  Adjacency_Graph graph = create_graph_structured<false>(5);
  std::vector<std::size_t> distances;
  eccentricity_vertex_breadth_first(graph, 0, distances);
  std::vector<std::size_t> hand_computed_answer({0, 1, 2, 3, 4,
                                                 1, 2, 3, 4, 5,
                                                 2, 3, 4, 5, 6,
                                                 3, 4, 5, 6, 7,
                                                 4, 5, 6, 7, 8});
  EXPECT_EQ(distances, hand_computed_answer);

  // Perform a test on a subgraph, capping the max vertex, and starting at a different vertex to the above.
  Adjacency_Subgraph subgraph(graph, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                                      15, 16, 17, 18, 19, 20, 21, 22, 23, 24});
  eccentricity_vertex_breadth_first(graph, 12, distances, 15);
  hand_computed_answer = std::vector<std::size_t> ({4, 3, 2, 3, 4,
                                                    3, 2, 1, 2, 3,
                                                    2, 1, 0, 1, 2});
  EXPECT_EQ(distances, hand_computed_answer);

  // Death test.
  EXPECT_DEATH(eccentricity_vertex_breadth_first(Adjacency_Graph<false>(), 0, distances), "./*");
  EXPECT_DEATH(eccentricity_vertex_breadth_first(subgraph, 1500, distances), "./*");
  EXPECT_DEATH(eccentricity_vertex_breadth_first(subgraph, 2, distances, 900), "./*");
  EXPECT_DEATH(eccentricity_vertex_breadth_first(subgraph, 10, distances, 1), "./*");
}
