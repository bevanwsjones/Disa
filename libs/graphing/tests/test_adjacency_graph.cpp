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
#include "generator.h"

using namespace Disa;

// ---------------------------------------------------------------------------------------------------------------------
// AdjacencyGraph
// ---------------------------------------------------------------------------------------------------------------------
// Public Member Functions
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_adjacency_graph, edge_list_construction) {

  Adjacency_Graph<false> graph = create_graph_hybrid();

  EXPECT_EQ(graph.size_vertex(), 8);
  EXPECT_EQ(graph.size_edge(), 12);

  EXPECT_EQ(graph[0].size(), 2);
  EXPECT_EQ(graph[1].size(), 3);
  EXPECT_EQ(graph[2].size(), 2);
  EXPECT_EQ(graph[3].size(), 3);
  EXPECT_EQ(graph[4].size(), 5);
  EXPECT_EQ(graph[5].size(), 3);
  EXPECT_EQ(graph[6].size(), 3);
  EXPECT_EQ(graph[7].size(), 3);

  EXPECT_EQ(graph[0][0], 1);
  EXPECT_EQ(graph[0][1], 3);

  EXPECT_EQ(graph[1][0], 0);
  EXPECT_EQ(graph[1][1], 2);
  EXPECT_EQ(graph[1][2], 4);

  EXPECT_EQ(graph[2][0], 1);
  EXPECT_EQ(graph[2][1], 5);

  EXPECT_EQ(graph[3][0], 0);
  EXPECT_EQ(graph[3][1], 4);
  EXPECT_EQ(graph[3][2], 6);

  EXPECT_EQ(graph[4][0], 1);
  EXPECT_EQ(graph[4][1], 3);
  EXPECT_EQ(graph[4][2], 5);
  EXPECT_EQ(graph[4][3], 6);
  EXPECT_EQ(graph[4][4], 7);

  EXPECT_EQ(graph[5][0], 2);
  EXPECT_EQ(graph[5][1], 4);
  EXPECT_EQ(graph[5][2], 7);

  EXPECT_EQ(graph[6][0], 3);
  EXPECT_EQ(graph[6][1], 4);
  EXPECT_EQ(graph[6][2], 7);

  EXPECT_EQ(graph[7][0], 4);
  EXPECT_EQ(graph[7][1], 5);
  EXPECT_EQ(graph[7][2], 6);
}

// ---------------------------------------------------------------------------------------------------------------------
// Element Access
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_adjacency_graph, at) {
  Adjacency_Graph<false> graph({{0, 1}, {2, 1}, {2, 3}, {3, 0}}); // place some edge backwards.

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
  Adjacency_Graph<false> graph({{0, 1}, {2, 1}, {2, 3}, {3, 0}}); // place some edge backwards.

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
  Adjacency_Graph<false> graph;
  EXPECT_EQ(graph.data().first, nullptr);
  EXPECT_EQ(graph.data().second, nullptr);

  graph.resize(4); // we have no edges yet, so the vertex list remains a nullptr.
  EXPECT_EQ(graph.data().first, nullptr);
  EXPECT_NE(graph.data().second, nullptr);

  graph.insert(std::make_pair(0, 1));
  EXPECT_NE(graph.data().first, nullptr);
  EXPECT_NE(graph.data().second, nullptr);
}

TEST(test_adjacency_graph, front){
  Adjacency_Graph<true> graph({{0, 1}, {1, 2}, {3, 2}, {3, 0}});
  EXPECT_EQ(graph.front().size(), 1);
  EXPECT_EQ(graph.front()[0], graph[0][0]);
}

TEST(test_adjacency_graph, back){
  Adjacency_Graph<false> graph({{0, 1}, {2, 1}, {2, 3}, {3, 0}});
  EXPECT_EQ(graph.back().size(), 2);
  EXPECT_EQ(graph.back()[0], graph[3][0]);
  EXPECT_EQ(graph.back()[1], graph[3][1]);
}

// ---------------------------------------------------------------------------------------------------------------------
// Iterators
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Capacity
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_adjacency_graph, empty) {
  Adjacency_Graph<false> undirected_graph;
  EXPECT_TRUE(undirected_graph.empty());
  undirected_graph.insert(std::make_pair(0, 1));
  EXPECT_FALSE(undirected_graph.empty());

  Adjacency_Graph<true> directed_graph;
  EXPECT_TRUE(directed_graph.empty());
  directed_graph.insert(std::make_pair(0, 1));
  EXPECT_FALSE(directed_graph.empty());
}

TEST(test_adjacency_graph, size_undirected) {
  Adjacency_Graph<false> graph;
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

TEST(test_adjacency_graph, size_directed) {
  Adjacency_Graph<true> graph;
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

TEST(test_adjacency_graph, reserve_capacity_undirected) {
  Adjacency_Graph<false> graph;
  EXPECT_EQ(graph.capacity().first, 0);
  EXPECT_EQ(graph.capacity().second, 0);

  graph.reserve(5);
  EXPECT_EQ(graph.capacity().first, 5);
  EXPECT_EQ(graph.capacity().second, 0);

  graph.reserve(10, 40);
  EXPECT_EQ(graph.capacity().first, 10);
  EXPECT_EQ(graph.capacity().second, 40);
}

TEST(test_adjacency_graph, reserve_capacity_directed) {
  Adjacency_Graph<true> graph;
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
  Adjacency_Graph<false> graph;
  EXPECT_EQ(graph.capacity().first, 0);
  EXPECT_EQ(graph.capacity().second, 0);

  graph.reserve(5);
  EXPECT_EQ(graph.capacity().first, 5);
  EXPECT_EQ(graph.capacity().second, 0);

  graph.shrink_to_fit();
  EXPECT_EQ(graph.capacity().first, 0);
  EXPECT_EQ(graph.capacity().second, 0);
}

// ---------------------------------------------------------------------------------------------------------------------
// Modifiers
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_adjacency_graph, clear_undirected) {
  Adjacency_Graph<false> graph({{0, 1}, {2, 1}, {2, 3}, {3, 0}});

  graph.clear();
  EXPECT_TRUE(graph.empty());
  EXPECT_EQ(graph.size_edge(), 0);
  EXPECT_EQ(graph.size_vertex(), 0);
  EXPECT_EQ(graph.capacity().first, std::make_pair(4, 4).first);   // make sure we have not lost capacity.
  EXPECT_EQ(graph.capacity().second, std::make_pair(4, 4).second); // make sure we have not lost capacity.
}

TEST(test_adjacency_graph, clear_directed) {
  Adjacency_Graph<false> graph({{0, 1}, {2, 1}, {2, 3}, {3, 0}});

  graph.clear();
  EXPECT_TRUE(graph.empty());
  EXPECT_EQ(graph.size_edge(), 0);
  EXPECT_EQ(graph.size_vertex(), 0);
  EXPECT_EQ(graph.capacity().first, std::make_pair(4, 4).first);   // make sure we have not lost capacity.
  EXPECT_EQ(graph.capacity().second, std::make_pair(4, 4).second); // make sure we have not lost capacity.
}

TEST(test_adjacency_graph, insert_undirected) {
  Adjacency_Graph<false> graph;

  // insert a pentagram out of order, with some edges flipped
  EXPECT_TRUE(graph.insert({0, 1}));
  EXPECT_TRUE(graph.insert({3, 4}));
  EXPECT_TRUE(graph.insert({0, 4}));
  EXPECT_TRUE(graph.insert({2, 3}));
  EXPECT_TRUE(graph.insert({1, 2}));

  // Add two additional vertices, as an 'appendage'.
  EXPECT_TRUE(graph.insert({3, 6}));
  EXPECT_TRUE(graph.insert({5, 6}));

  // check ascending order has been maintained.
  EXPECT_EQ(graph.size_edge(), 7);
  EXPECT_EQ(graph.size_vertex(), 7);

  // Attempt to insert an existing edge
  EXPECT_FALSE(graph.insert({1, 2}));

  // Connect across
  EXPECT_TRUE(graph.insert({0, 3}));
  EXPECT_TRUE(graph.insert({0, 2}));

  // Check graph
  EXPECT_EQ(graph.size_edge(), 9);
  EXPECT_EQ(graph.size_vertex(), 7);
  EXPECT_EQ(graph[0].size(), 4);
  EXPECT_EQ(graph[1].size(), 2);
  EXPECT_EQ(graph[2].size(), 3);
  EXPECT_EQ(graph[3].size(), 4);
  EXPECT_EQ(graph[4].size(), 2);
  EXPECT_EQ(graph[5].size(), 1);
  EXPECT_EQ(graph[6].size(), 2);

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
  EXPECT_EQ(graph[3][3], 6);
  EXPECT_EQ(graph[4][0], 0);
  EXPECT_EQ(graph[4][1], 3);
  EXPECT_EQ(graph[5][0], 6);
  EXPECT_EQ(graph[6][0], 3);
  EXPECT_EQ(graph[6][1], 5);

  // death test - invalid edge
  EXPECT_DEATH(graph.insert({1, 1}), ".*");
}

TEST(test_adjacency_graph, insert_directed) {
  Adjacency_Graph<true> graph;

  // insert a pentagram out of order, with some edges flipped
  EXPECT_TRUE(graph.insert({0, 1}));
  EXPECT_TRUE(graph.insert({3, 4}));
  EXPECT_TRUE(graph.insert({0, 4}));
  EXPECT_TRUE(graph.insert({2, 3}));
  EXPECT_TRUE(graph.insert({1, 2}));

  // Add three additional vertices, as an 'appendage'.
  EXPECT_TRUE(graph.insert({3, 6}));
  EXPECT_TRUE(graph.insert({5, 6}));
  EXPECT_TRUE(graph.insert({6, 0}));

  // check ascending order has been maintained.
  EXPECT_EQ(graph.size_edge(), 8);
  EXPECT_EQ(graph.size_vertex(), 7);

  // Attempt to insert an existing edge
  EXPECT_FALSE(graph.insert({1, 2}));
 
  // Connect across
  EXPECT_TRUE(graph.insert({0, 3}));
  EXPECT_TRUE(graph.insert({0, 2}));

  // Check graph
  EXPECT_EQ(graph.size_edge(), 10);
  EXPECT_EQ(graph.size_vertex(), 7);
 
  EXPECT_EQ(graph[0].size(), 4);
  EXPECT_EQ(graph[1].size(), 1);
  EXPECT_EQ(graph[2].size(), 1);
  EXPECT_EQ(graph[3].size(), 2);
  EXPECT_EQ(graph[4].size(), 0);
  EXPECT_EQ(graph[5].size(), 1);
  EXPECT_EQ(graph[6].size(), 1);
  EXPECT_EQ(graph[0][0], 1); 
  EXPECT_EQ(graph[0][1], 2);
  EXPECT_EQ(graph[0][2], 3);
  EXPECT_EQ(graph[0][3], 4);
  EXPECT_EQ(graph[1][0], 2);
  EXPECT_EQ(graph[2][0], 3);
  EXPECT_EQ(graph[3][0], 4);
  EXPECT_EQ(graph[3][1], 6); 
  EXPECT_EQ(graph[5][0], 6);
  EXPECT_EQ(graph[6][0], 0);
  
  // death test - invalid edge
  EXPECT_DEATH(graph.insert({1, 1}), ".*");
}

TEST(test_adjacency_graph, erase_if_undirected) {
  Adjacency_Graph<false> saad = create_graph_saad();
  auto erase_if_odd = [](const auto& i_vertex){return i_vertex%2 != 0;};
  Adjacency_Graph<false> answer({{0, 3}, {0, 4}, {1, 3}, {2, 5}, {3, 4}, {5, 6}});
  answer.resize(7);

  saad.erase_if(erase_if_odd);
  FOR(i_vertex, answer.size_vertex())
    FOR(i_adjacent, answer[i_vertex].size())
      EXPECT_EQ(answer[i_vertex][i_adjacent], saad[i_vertex][i_adjacent]);
}

TEST(test_adjacency_graph, erase_if_directed) {
  Adjacency_Graph<true> square_graph = create_graph_structured<true>(4);
  
  // remap 1 -> 0, 3 -> 1, 5 -> 2, 7 -> 3, 9 -> 4, 11 -> 5, 13 -> 6, 15 -> 7
  auto erase_if_even = [](const auto& i_vertex){return i_vertex%2 == 0;};
  Adjacency_Graph<true> answer({{0, 2}, {1, 3}, {2, 4}, {3, 5}, {4, 6}, {5, 7}}); // only up connection survive
  answer.resize(8); // should be one extra vertex which is not connected to anything. 

  square_graph.erase_if(erase_if_even);
  FOR(i_vertex, answer.size_vertex())
    FOR(i_adjacent, answer[i_vertex].size())
      EXPECT_EQ(answer[i_vertex][i_adjacent], square_graph[i_vertex][i_adjacent]);
}


TEST(test_adjacency_graph, resize) {

  // Test sizing up.
  Adjacency_Graph<false> graph;
  graph.resize(5);
  EXPECT_EQ(graph.size_edge(), 0);
  EXPECT_EQ(graph.size_vertex(), 5);

  // Test size down, start with a fully connected pentagon graph.
  graph = Adjacency_Graph<false>({{0, 1}, {1, 2}, {2, 3}, {3, 4}, {0, 4}, {0, 2}, {0, 3}, {1, 3}, {1, 4}, {2, 4}});

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
  Adjacency_Graph<false> graph_0({{0, 1}, {1, 2}, {2, 3}, {0, 3}});  // quadrilateral graph
  Adjacency_Graph<false> graph_1({{0, 1}, {1, 2}, {0, 2}});          // 2-simplex graph

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

// ---------------------------------------------------------------------------------------------------------------------
// Lookup
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_adjacency_graph, contains_directed) {
  Adjacency_Graph<true> graph({{0, 1}, {1, 0}, {2, 1}, {2, 3}, {0, 3}, {3, 1}});

  EXPECT_TRUE(graph.contains({0, 1}));
  EXPECT_TRUE(graph.contains({1, 0}));
  EXPECT_FALSE(graph.contains({1, 2}));
  EXPECT_TRUE(graph.contains({2, 1}));
  EXPECT_TRUE(graph.contains({2, 3}));
  EXPECT_FALSE(graph.contains({3, 2}));
  EXPECT_TRUE(graph.contains({0, 3}));
  EXPECT_FALSE(graph.contains({3, 0}));
  EXPECT_FALSE(graph.contains({1, 3}));
  EXPECT_TRUE(graph.contains({3, 1}));
  EXPECT_FALSE(graph.contains({0, 2}));
  EXPECT_FALSE(graph.contains({2, 0}));
}

TEST(test_adjacency_graph, contains_undirected) {
  Adjacency_Graph<false> graph({{0, 1}, {1, 2}, {2, 3}, {0, 3}, {1, 3}});

  EXPECT_TRUE(graph.contains({0, 1}));
  EXPECT_TRUE(graph.contains({1, 0}));
  EXPECT_TRUE(graph.contains({1, 2}));
  EXPECT_TRUE(graph.contains({2, 1}));
  EXPECT_TRUE(graph.contains({2, 3}));
  EXPECT_TRUE(graph.contains({3, 2}));
  EXPECT_TRUE(graph.contains({0, 3}));
  EXPECT_TRUE(graph.contains({3, 0}));
  EXPECT_TRUE(graph.contains({1, 3}));
  EXPECT_TRUE(graph.contains({3, 1}));
  EXPECT_FALSE(graph.contains({0, 2}));
  EXPECT_FALSE(graph.contains({2, 0}));
}


// ---------------------------------------------------------------------------------------------------------------------
// Graph Operators
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_adjacency_graph, degree) {

  Adjacency_Graph<false> graph = create_graph_hybrid();

  EXPECT_EQ(graph.degree(0), 2);
  EXPECT_EQ(graph.degree(3), 3);
  EXPECT_EQ(graph.degree(4), 5);
  EXPECT_DEATH(1 == graph.degree(50), "./*");
}

TEST(test_adjacency_graph, reorder) {

  // Check empty graph calls
  Adjacency_Graph<false> graph;
  EXPECT_NO_FATAL_FAILURE(graph.reorder(std::vector<std::size_t>()));
  EXPECT_DEATH(graph.reorder(std::vector<std::size_t>({0, 1})), "./*");

  graph = Adjacency_Graph<false>({{0, 1}, {1, 2}, {2, 3}, {3, 4}, {0, 3}, {0, 4}}); // pentagon with diagonal connection.

  // Check death conditions
  EXPECT_DEATH(graph.reorder(std::vector<std::size_t>({0, 1})), "./*");               // check under size crashes
  EXPECT_DEATH(graph.reorder(std::vector<std::size_t>({1, 0, 3, 2, 5, 4})), "./*");   // check oversize crashes
  EXPECT_DEATH(graph.reorder(std::vector<std::size_t>({1, 3, 3, 2, 4})), "./*");      // check duplicate crashes
  EXPECT_DEATH(graph.reorder(std::vector<std::size_t>({1, 3, 9, 2, 4})), "./*");      // check index > size

  // Actual reordering check, needs to re-order and ensure sorted lists are created.
  //                            old index   0  1  2  3  4
  std::vector<std::size_t> new_numbering = {2, 3, 4, 0, 1};
  Adjacency_Graph old_graph = graph.reorder(new_numbering);
  EXPECT_EQ(graph.size().first, 5);
  EXPECT_EQ(graph.size().second, 6);

  // 0 : 1, 3, 4 -> 2 : 0, 1, 3
  EXPECT_EQ(graph[2].size(), 3);
  EXPECT_EQ(graph[2][0], 0);
  EXPECT_EQ(graph[2][1], 1);
  EXPECT_EQ(graph[2][2], 3);

  // 1 : 0, 2    -> 3 : 2, 4
  EXPECT_EQ(graph[3].size(), 2);
  EXPECT_EQ(graph[3][0], 2);
  EXPECT_EQ(graph[3][1], 4);

  // 2 : 1, 3    -> 4 : 0, 3
  EXPECT_EQ(graph[4].size(), 2);
  EXPECT_EQ(graph[4][0], 0);
  EXPECT_EQ(graph[4][1], 3);

  // 3 : 0, 2, 4 -> 0 : 1, 2, 4
  EXPECT_EQ(graph[0].size(), 3);
  EXPECT_EQ(graph[0][0], 1);
  EXPECT_EQ(graph[0][1], 2);
  EXPECT_EQ(graph[0][2], 4);

  // 4 : 0, 3    -> 1 : 0, 2
  EXPECT_EQ(graph[1].size(), 2);
  EXPECT_EQ(graph[1][0], 0);
  EXPECT_EQ(graph[1][1], 2);
}
