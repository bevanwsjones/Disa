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
#include "adjacency_subgraph.h"

using namespace Disa;

TEST(Adjacency_Subgraph, constructor) {

  Adjacency_Graph graph_saad = Adjacency_Graph({{0, 6}, {0, 8}, {0, 13}, {0, 14},
                                                {1, 7}, {1, 8}, {1, 10}, {1, 12},
                                                {2, 6}, {2, 7}, {2, 9},
                                                {3, 11}, {3, 12}, {3, 14},
                                                {4, 9}, {4, 10}, {4, 11}, {4, 13},
                                                {5, 13}, {5, 14},
                                                {6, 7}, {6, 8},
                                                {7, 8}, {7, 9}, {7, 10},
                                                {8, 13}, {8, 14},
                                                {9, 10},
                                                {10, 11}, {10, 12},
                                                {11, 12}, {11, 13}, {11, 14},
                                                {13, 14}});

  std::vector<std::size_t> subgraph_indexes({0, 8, 6, 1, 2});

  Adjacency_Subgraph test(graph_saad, subgraph_indexes, 1);


  std::cout<<"\nstart ";
  std::cout<<"\n"<<test;
  std::cout<<"\nend\n\n ";


}























