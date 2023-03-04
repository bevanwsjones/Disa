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

#ifndef DISA_TEST_GRAPH_H
#define DISA_TEST_GRAPH_H

#include "adjacency_graph.h"

/**
 *
 * @param number_vertices
 * @return
 */
Disa::Adjacency_Graph create_graph_saad() {

  return Disa::Adjacency_Graph({{0, 6}, {0, 8},
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

/**
 *
 * @param number_vertices
 */
Disa::Adjacency_Graph  create_graph_structured(std::size_t number_vertices) {
  Disa::Adjacency_Graph structured;
  for(std::size_t i_y = 0; i_y < number_vertices; ++i_y) {
    for(std::size_t i_x = 0; i_x < number_vertices; ++i_x) {
      const std::size_t i_vertex = i_y*(number_vertices) + i_x;
      if(i_x != 0) structured.insert({i_vertex - 1, i_vertex});
      if(i_x != number_vertices - 1) structured.insert({i_vertex, i_vertex + 1});
      if(i_y != 0) structured.insert({i_vertex - number_vertices, i_vertex});
      if(i_y < number_vertices - 1) structured.insert({i_vertex, i_vertex + number_vertices});
    }
  }
  return structured;
}




#endif //DISA_TEST_GRAPH_H
