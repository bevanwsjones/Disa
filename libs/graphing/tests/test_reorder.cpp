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
#include "reorder.h"

using namespace Disa;

class test_reorder : public ::testing::Test {
protected:
  Adjacency_Graph graph_saad;  // Testing graph taken from Yousef Saad, 2003.

  void SetUp() override {
    graph_saad = AdjacencyGraph({{0, 6}, {0, 8},
                                 {1, 7}, {1, 8}, {1, 10}, {1, 12},
                                 {2, 6}, {2, 7}, {2, 9},
                                 {3, 11}, {3, 12}, {3, 14},
                                 {4, 9}, {4, 10}, {4, 11}, {4, 13},
                                 {5, 13}, {5, 14},
                                 {6, 7}, {6, 8},
                                 {7, 8}, {7, 9}, {7, 10},
                                 {9, 10},
                                 {10, 11}, {10, 12},
                                 {11, 12}, {11, 13}, {11, 14},
                                 {13, 14}});
  }
};

// ---------------------------------------------------------------------------------------------------------------------
// Level-Set Orderings
// ---------------------------------------------------------------------------------------------------------------------

TEST_F(test_reorder, breadth_first) {

  // test problem
  // 0 - 1 - 2
  // |   |   |
  // 3 - 4 - 5
  //  \ / \ /
  //   6 - 7
  Adjacency_Graph graph({{0, 1}, {0, 3}, {1, 2}, {1, 4}, {2, 5}, {3, 4}, {3, 6}, {4, 5}, {4, 6}, {4, 7}, {5, 7},
                         {6, 7}});

  // Reorder with a new root of index 5.
  std::vector<std::size_t> reorder = breadth_first(graph, 5);

  EXPECT_EQ(reorder.size(), graph.size_vertex());
  EXPECT_EQ(reorder[0], 7);
  EXPECT_EQ(reorder[1], 4);
  EXPECT_EQ(reorder[2], 1);
  EXPECT_EQ(reorder[3], 5);
  EXPECT_EQ(reorder[4], 2);
  EXPECT_EQ(reorder[5], 0);
  EXPECT_EQ(reorder[6], 6);
  EXPECT_EQ(reorder[7], 3);

  EXPECT_DEATH(breadth_first(graph, 10), "./*");
  EXPECT_TRUE(breadth_first(AdjacencyGraph()).empty()); // ensure empty graphs returns empty reorder.
}

TEST_F(test_reorder, cuthill_mckee) {

  // Test search vertex (should be old vertex 0)
  // Old Index:   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14
  // New Index:   0,  5,  4, 11,  9, 14,  1,  4,  2,  6,  7, 10,  8, 12, 13
  std::vector<std::size_t> reorder = cuthill_mckee(graph_saad);
  EXPECT_EQ(reorder.size(), graph_saad.size_vertex());
  EXPECT_EQ(reorder[0], 0);
  EXPECT_EQ(reorder[1], 5);
  EXPECT_EQ(reorder[2], 3);
  EXPECT_EQ(reorder[3], 11);
  EXPECT_EQ(reorder[4], 9);
  EXPECT_EQ(reorder[5], 14);
  EXPECT_EQ(reorder[6], 1);
  EXPECT_EQ(reorder[7], 4);
  EXPECT_EQ(reorder[8], 2);
  EXPECT_EQ(reorder[9], 6);
  EXPECT_EQ(reorder[10], 7);
  EXPECT_EQ(reorder[11], 10);
  EXPECT_EQ(reorder[12], 8);
  EXPECT_EQ(reorder[13], 12);
  EXPECT_EQ(reorder[14], 13);

  // Test parse vertex
  // New Index: 0,  1,  2,  3,  4,  5,  6,  7, 8,  9, 10, 11, 12, 13, 14
  // Old Index: 4,  8,  0, 14,  6, 12,  1,  3, 5,  2,  7, 10, 11,  9, 13
  reorder = cuthill_mckee(graph_saad, 2);
  EXPECT_EQ(reorder.size(), graph_saad.size_vertex());
  EXPECT_EQ(reorder[0], 4);
  EXPECT_EQ(reorder[1], 8);
  EXPECT_EQ(reorder[2], 0);
  EXPECT_EQ(reorder[3], 14);
  EXPECT_EQ(reorder[4], 6);
  EXPECT_EQ(reorder[5], 12);
  EXPECT_EQ(reorder[6], 1);
  EXPECT_EQ(reorder[7], 3);
  EXPECT_EQ(reorder[8], 5);
  EXPECT_EQ(reorder[9], 2);
  EXPECT_EQ(reorder[10], 7);
  EXPECT_EQ(reorder[11], 10);
  EXPECT_EQ(reorder[12], 11);
  EXPECT_EQ(reorder[13], 9);
  EXPECT_EQ(reorder[14], 13);

  EXPECT_DEATH(cuthill_mckee_reverse(graph_saad, 36), "./*");
  EXPECT_TRUE(cuthill_mckee_reverse(AdjacencyGraph()).empty()); // ensure empty graphs returns empty reorder.
}

TEST_F(test_reorder, cuthill_mckee_reverse) {

  // Test search vertex (should be old vertex 0)
  // New Index:  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14
  // Old Index: 14,  9, 11,  3,  5,  0, 13, 10, 12,  8,  7,  4,  6,  2,  1
  std::vector<std::size_t> reorder = cuthill_mckee_reverse(graph_saad);
  EXPECT_EQ(reorder.size(), graph_saad.size_vertex());
  EXPECT_EQ(reorder[0], 14);
  EXPECT_EQ(reorder[1], 9);
  EXPECT_EQ(reorder[2], 11);
  EXPECT_EQ(reorder[3], 3);
  EXPECT_EQ(reorder[4], 5);
  EXPECT_EQ(reorder[5], 0);
  EXPECT_EQ(reorder[6], 13);
  EXPECT_EQ(reorder[7], 10);
  EXPECT_EQ(reorder[8], 12);
  EXPECT_EQ(reorder[9], 8);
  EXPECT_EQ(reorder[10], 7);
  EXPECT_EQ(reorder[11], 4);
  EXPECT_EQ(reorder[12], 6);
  EXPECT_EQ(reorder[13], 2);
  EXPECT_EQ(reorder[14], 1);

  // Test parse vertex
  // New Index:  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14
  // Old Index: 10,  6, 14,  0,  8,  2, 13, 11,  9, 12,  7,  4,  3,  5,  1
  reorder = cuthill_mckee_reverse(graph_saad, 2);
  EXPECT_EQ(reorder.size(), graph_saad.size_vertex());
  EXPECT_EQ(reorder[0], 10);
  EXPECT_EQ(reorder[1], 6);
  EXPECT_EQ(reorder[2], 14);
  EXPECT_EQ(reorder[3], 0);
  EXPECT_EQ(reorder[4], 8);
  EXPECT_EQ(reorder[5], 2);
  EXPECT_EQ(reorder[6], 13);
  EXPECT_EQ(reorder[7], 11);
  EXPECT_EQ(reorder[8], 9);
  EXPECT_EQ(reorder[9], 12);
  EXPECT_EQ(reorder[10], 7);
  EXPECT_EQ(reorder[11], 4);
  EXPECT_EQ(reorder[12], 3);
  EXPECT_EQ(reorder[13], 5);
  EXPECT_EQ(reorder[14], 1);

  EXPECT_DEATH(cuthill_mckee_reverse(graph_saad, 36), "./*");
  EXPECT_TRUE(greedy_multicolouring(AdjacencyGraph()).empty()); // ensure empty graphs returns empty reorder.
}

// ---------------------------------------------------------------------------------------------------------------------
// Multicolor Ordering
// ---------------------------------------------------------------------------------------------------------------------

TEST_F(test_reorder, greedy_multicolouring) {

  // Test search vertex (should be old vertex 0)
  // New Index:  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14
  // Old Index:  0,  1,  2,  3,  4,  5,  6,  9, 11,  7, 12,  8, 10, 11, 14
  std::vector<std::size_t> reorder = greedy_multicolouring(graph_saad);
  EXPECT_EQ(reorder.size(), graph_saad.size_vertex());
  EXPECT_EQ(reorder[0], 0);
  EXPECT_EQ(reorder[1], 1);
  EXPECT_EQ(reorder[2], 2);
  EXPECT_EQ(reorder[3], 3);
  EXPECT_EQ(reorder[4], 4);
  EXPECT_EQ(reorder[5], 5);
  EXPECT_EQ(reorder[6], 6);
  EXPECT_EQ(reorder[7], 9);
  EXPECT_EQ(reorder[8], 12);
  EXPECT_EQ(reorder[9], 7);
  EXPECT_EQ(reorder[10], 13);
  EXPECT_EQ(reorder[11], 8);
  EXPECT_EQ(reorder[12], 10);
  EXPECT_EQ(reorder[13], 11);
  EXPECT_EQ(reorder[14], 14);

  EXPECT_TRUE(greedy_multicolouring(AdjacencyGraph()).empty()); // ensure empty graphs returns empty reorder.
}





















