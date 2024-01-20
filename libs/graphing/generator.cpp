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
// File Name: generator.cpp
// Description: Definitions of functions which generate graphs.
// ---------------------------------------------------------------------------------------------------------------------

#include "generator.h"

namespace Disa{

/**
 * @details The topology of the Adjacency Graph is depicted below, with the ordering of the vertex indices.
 *
 *  0  - 1 -  ... - n-2 - n-1
 */
Adjacency_Graph<false> create_graph_line(const std::size_t number_vertices) {
  Adjacency_Graph<false> line;
  for(std::size_t i_x = 0; i_x < number_vertices - 1; ++i_x)
    line.insert({i_x, i_x + 1});
  return line;
}

/**
 * @details The topology of the Adjacency Graph is depicted below, with the ordering of the vertex indices.
 *
 *  n^2-n    - n^2-(n+1) -   ...     -  n^2-2    -  n^2-1
 *    |           |           |           |           |
 *   ...     -           -   ...     -   ...     -   ...
 *    |           |           |           |           |
 *    n      -   n+1     -   ...     -   2n-2    -   2n-1
 *    |           |           |            |          |
 *    0      -    1      -   ...     -    n-2    -   n-1
 */
Adjacency_Graph<false> create_graph_structured(const std::size_t number_vertices) {
  Adjacency_Graph<false> structured;
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

}
