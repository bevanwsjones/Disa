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
// File Name: reorder.h
// Description: Declarations for the various reordering/permutation algorithms available in Disa.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_REORDER_H
#define DISA_REORDER_H

#include "adjacency_graph.h"

#include <numeric>

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Level-Set Orderings
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Constructs a permutation vector for a given non-disjoint graph using the bread first (search) algorithm.
 * @param[in] graph The graph to reorder.
 * @param[in] start_vertex The new graph root vertex, or starting vertex for the reordering. Defaults to 0.
 * @return The permutation vector mapping the old to new graph, i.e. new_index = permutation[old_index].
 */
[[nodiscard]] std::vector<std::size_t> breadth_first(const Adjacency_Graph& graph, std::size_t start_vertex = 0);

/**
 * @brief Constructs a permutation vector given non-disjoint graph using the Cuthill-Mckee algorithm.
 * @param[in] graph The graph to reorder.
 * @param[in] start_vertex The new graph root vertex, if default a periphery node will be search for. Defaults to max().
 * @return The permutation vector mapping the old to new graph, i.e. new_index = re_order[old_index].
 */
std::vector<std::size_t> cuthill_mckee(const Adjacency_Graph& graph,
                                       std::size_t start_vertex = std::numeric_limits<std::size_t>::max());

/**
 * @brief Constructs a permutation vector given non-disjoint graph using the Reverse Cuthill-Mckee algorithm.
 * @param[in] graph The graph to reorder.
 * @param[in] start_vertex The new graph root vertex, if default a periphery node will be search for. Defaults to max().
 * @return The permutation vector mapping the old to new graph, i.e. new_index = permutation[old_index].
 */
inline std::vector<std::size_t> cuthill_mckee_reverse(const Adjacency_Graph& graph,
                                                      std::size_t root_vertex = std::numeric_limits<std::size_t>::max())
                                                      {
  std::vector permutation = cuthill_mckee(graph, root_vertex);
  FOR_EACH_REF(t, permutation) { t = permutation.size() - t - 1;}
  return permutation;
};

// ---------------------------------------------------------------------------------------------------------------------
// Multicolor Ordering
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Constructs a permutation vector given non-disjoint graph using a multicolouring algorithm.
 * @param[in] graph The graph to reorder.
 * @return The permutation vector mapping the old to new graph, i.e. new_index = permutation[old_index].
 */
std::vector<std::size_t> greedy_multicolouring(const Adjacency_Graph& graph);

}
#endif //DISA_REORDER_H
