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
#include "partition.h"
#include "test_graph.h"

using namespace Disa;

// Unit test for LevelTraversal using Google Test
TEST(test_partition, multinode_level_set_expansion) {

  Adjacency_Graph graph = create_graph_structured(10);

  std::cout<<graph;
  auto sub_graphs = multinode_level_set_expansion(graph, 3, 4);

  FOR_EACH(sgraph, sub_graphs){
    std::cout<<"\n------------------";
    std::cout<<sgraph;
  }

  //  auto sub_graphs = recursive_graph_bisection(create_graph_saad(), 3);
}

// Unit test for LevelTraversal using Google Test
TEST(LevelTraversalTest, SimpleTest) {

//  auto sub_graphs = recursive_graph_bisection(create_graph_structured(4), 3);
  std::cout<<"\n------------------";
//  FOR_EACH(graph, sub_graphs){
//    std::cout<<"\n"<<graph;
    std::cout<<"\n------------------";
//  }

//  auto sub_graphs = recursive_graph_bisection(create_graph_saad(), 3);
}
