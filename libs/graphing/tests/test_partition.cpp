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
#include "partition.h"
#include "adjacency_graph.h"
#include "adjacency_subgraph.h"

using namespace Disa;

// Unit test for LevelTraversal using Google Test
TEST(LevelTraversalTest, SimpleTest) {
  Adjacency_Graph graph_saad({{0, 6}, {0, 8},
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

  auto sub_graphs = recursive_graph_bisection(graph_saad, 3);
  FOR_EACH(graph, sub_graphs){
    std::cout<<"\n"<<graph;
    std::cout<<"\n------------------";
  }
}