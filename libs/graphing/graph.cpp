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
// File Name: graph.cpp
// Description: Contains the some definitions for an adjacency graph for Disa.
// ---------------------------------------------------------------------------------------------------------------------

#include "graph.h"

namespace Disa {

/**
 * @details
 */
bool AdjacencyGraph::insert(const Edge& edge) {

  if(contains(edge)) return false;

  ASSERT_DEBUG(edge.first != edge.second, "Edge vertices identical, " + std::to_string(edge.first) + " and "
               + std::to_string(edge.second) + ".");

  const std::size_t& i_lower_vertex = edge.first < edge.second ? edge.first : edge.second;
  const std::size_t& i_upper_vertex = edge.first < edge.second ? edge.second : edge.first;

  // Resize if we need to.
  if(i_upper_vertex >= size_vertices() || i_lower_vertex >= size_vertices()) resize(i_upper_vertex + 1);

  // Insert lower vertex.
  const auto insert_iter = vertex_adjacent_list.empty() ? vertex_adjacent_list.end() : std::lower_bound(
    std::next(vertex_adjacent_list.begin(), offset[i_lower_vertex]),
    std::next(vertex_adjacent_list.begin(), offset[i_lower_vertex] + 1), i_upper_vertex);
  const auto distance = std::distance(vertex_adjacent_list.begin(), insert_iter);
  vertex_adjacent_list.insert(vertex_adjacent_list.begin() + distance, i_upper_vertex);

  // Insert upper vertex
  for(auto offs = std::next(offset.begin(), i_lower_vertex + 1);
      offs != std::next(offset.begin(), i_upper_vertex + 1); ++(*offs++));
  const auto insert_iter0 = std::lower_bound(std::next(vertex_adjacent_list.begin(), offset[i_upper_vertex]),
                                             std::next(vertex_adjacent_list.begin(), offset[i_upper_vertex] + 1),
                                             i_lower_vertex);
  const auto distance0 = std::distance(vertex_adjacent_list.begin(), insert_iter0);
  vertex_adjacent_list.insert(vertex_adjacent_list.begin() + distance0, i_lower_vertex);

  for(auto offs = std::next(offset.begin(), i_upper_vertex + 1); offs != offset.end(); ++offs) *offs += 2;
  return true;
}

}