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
// File Name: Generator.h
// Description: Declarations of functions which generate graphs.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_GENERATOR_H
#define DISA_GENERATOR_H

#include "adjacency_graph.hpp"

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Dynamically Sized Graphs
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Creates a line Adjacency Graph consisting of n vertices.
 * @tparam _directed Whether the graph is directed or not, directed graphs point in an ascending fashion.
 * @param[in] number_vertices The number of vertices n to create.
 * @return The created Adjacency Graph.
 * 
 * @details The topology of the Adjacency Graph is depicted below, with the ordering of the vertex indices.
 *
 *  directed:  
 *  0 > 1 > ... > n-2 > n-1
 *
 *  undirected:
 *  0 - 1 - ... - n-2 - n-1
 */
template<bool _directed>
Adjacency_Graph<_directed> create_graph_line(std::size_t number_vertices) {
  Adjacency_Graph<_directed> line;
  for(std::size_t i_x = 0; i_x < number_vertices - 1; ++i_x)
    line.insert({i_x, i_x + 1});
  return line;
};

/**
 * @brief Creates a structured grid Adjacency Graph consisting of n^2 vertices.
 * @tparam _directed Whether the graph is directed or not, directed graphs point in an ascending fashion.
 * @param[in] number_vertices The number of vertices n along one of the 'axis'.
 * @return The created Adjacency Graph.
 * 
 * @details The topology of the Adjacency Graph is depicted below, with the ordering of the vertex indices.
 *
 * directed:
 *   n^2-n   > n^2-(n+1) >   ...     >  n^2-2    >  n^2-1
 *    ^           ^           ^           ^           ^
 *   ...     >           >   ...     >   ...     >   ...
 *    ^           ^           ^           ^           ^
 *    n      >   n+1     >   ...     >   2n-2    >   2n-1
 *    ^           ^           ^           ^           ^
 *    0      >    1      >   ...     >    n-2    >   n-1
 * 
 * undirected:
 *  n^2-n    - n^2-(n+1) -   ...     -  n^2-2    -  n^2-1
 *    |           |           |           |           |
 *   ...     -           -   ...     -   ...     -   ...
 *    |           |           |           |           |
 *    n      -   n+1     -   ...     -   2n-2    -   2n-1
 *    |           |           |           |           |
 *    0      -    1      -   ...     -    n-2    -   n-1
 */
template<bool _directed>
Adjacency_Graph<_directed> create_graph_structured(std::size_t number_vertices) {
  Adjacency_Graph<_directed> structured;
  for(std::size_t i_y = 0; i_y < number_vertices; ++i_y) {
    for(std::size_t i_x = 0; i_x < number_vertices; ++i_x) {
      const std::size_t i_vertex = i_y * (number_vertices) + i_x;
      if(!_directed && i_x != 0) structured.insert({i_vertex - 1, i_vertex});
      if(i_x != number_vertices - 1) structured.insert({i_vertex, i_vertex + 1});
      if(!_directed && i_y != 0) structured.insert({i_vertex - number_vertices, i_vertex});
      if(i_y < number_vertices - 1) structured.insert({i_vertex, i_vertex + number_vertices});
    }
  }
  return structured;
}

// ---------------------------------------------------------------------------------------------------------------------
// Static Graphs
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Forms an Adjacency Graph with both 'triangular' and 'quadrilateral' type connectivity.
 * @return The created Adjacency Graph.
 *
 * @details Below is the vertex indexing and topology of the returned adjacency graph.
 *
 * 0 - 1 - 2
 * |   |   |
 * 3 - 4 - 5
 *  \ / \ /
 *   6 - 7
 */
inline Adjacency_Graph<false> create_graph_hybrid() {
  return Adjacency_Graph<false>(
  {{3, 0}, {0, 1}, {7, 4}, {6, 7}, {2, 5}, {3, 4}, {6, 3}, {4, 5}, {4, 6}, {7, 4}, {5, 7}, {2, 1}, {1, 4}});
}

/**
 * @brief Creates one of the example graphs from the text book Iterative Methods for Sparse Linear Systems.
 * @return The created Saad Adjacency Graph.
 *
 * 5
 * |  \
 * 14 - 13
 * |  \ |  \
 * 3  - 11 - 4
 * |  / |  / |
 * 12 - 10 - 9
 * |  / |  / |
 * 1  - 7  - 2
 * |  / |  /
 * 8  - 6
 * |  /
 * 0
 *
 * @note: Reference DOI: https://doi.org/10.1137/1.9780898718003
 */
inline Adjacency_Graph<false> create_graph_saad() {
  return Adjacency_Graph<false>({{0, 6},   {0, 8},   {1, 7},   {1, 8},   {1, 10},  {1, 12}, {2, 6},  {2, 7},
                                 {2, 9},   {3, 11},  {3, 12},  {3, 14},  {4, 9},   {4, 10}, {4, 11}, {4, 13},
                                 {5, 13},  {5, 14},  {6, 7},   {6, 8},   {7, 8},   {7, 9},  {7, 10}, {9, 10},
                                 {10, 11}, {10, 12}, {11, 12}, {11, 13}, {11, 14}, {13, 14}});
}

}  // namespace Disa

#endif  //DISA_GENERATOR_H
