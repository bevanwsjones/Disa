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

#include "adjacency_graph.h"

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Dynamically Sized Graphs
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Creates a line Adjacency Graph consisting of n vertices.
 * @param[in] number_vertices The number of vertices n  to create.
 * @return The created Adjacency Graph.
 */
Adjacency_Graph<false> create_graph_line(std::size_t number_vertices);

/**
 * @brief Creates a structured grid Adjacency Graph consisting of n^2 vertices.
 * @param[in] number_vertices The number of vertices n along one of the 'axis'.
 * @return The created Adjacency Graph.
 */
Adjacency_Graph<false> create_graph_structured(std::size_t number_vertices);

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
  return Adjacency_Graph<false> ({{3, 0}, {0, 1}, {7, 4}, {6, 7}, {2, 5}, {3, 4}, {6, 3}, {4, 5}, {4, 6}, {7, 4}, 
                                  {5, 7}, {2, 1}, {1, 4}});

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
  return Adjacency_Graph<false> ({{0, 6}, {0, 8},
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

}


#endif //DISA_GENERATOR_H
