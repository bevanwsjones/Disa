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

#include "adjacency_graph.h"

#include <numeric>

namespace Disa {

//----------------------------------------------------------------------------------------------------------------------
// AdjacencyGraph
//----------------------------------------------------------------------------------------------------------------------
// Modifiers
//----------------------------------------------------------------------------------------------------------------------

/**
 * \details If the edge is already in the graph the function returns immediately. If the edge contains a vertex index
 * greater than the current graph size, the size of the graph is increased to accommodate the insertion. Lower bound is
 * then used to insert the upper index of the edge into the lower index adjacency list. The offset values are then
 * increased by one until the upper index is reached. The lower index is then inserted into the upper index adjacency
 * list. Finally the offset values are increase by two till the end of the offset vector.
 */
bool Adjacency_Graph::insert(const Edge& edge) {
  ASSERT_DEBUG(edge.first != edge.second, "Edge vertices identical, " + std::to_string(edge.first) + " and "
                                          + std::to_string(edge.second) + ".");

  // Preliminaries check if the edge exists, then determine if we need to resize.
  if(contains(edge)) return false;
  const auto& [i_lower_vertex, i_upper_vertex] = order_edge_vertex(&edge);
  if(i_upper_vertex >= size_vertex()) resize(i_upper_vertex + 1);

  // Insert lower vertex.
  insert_vertex_adjacent_list(i_lower_vertex, i_upper_vertex);
  std::for_each(std::next(offset.begin(), static_cast<s_size_t>(i_lower_vertex + 1)),
                std::next(offset.begin(), static_cast<s_size_t>(i_upper_vertex + 2)), [](std::size_t& off){off++;});

  // Insert upper vertex (tricky: last for loop is safe -> The highest vertex + 1 (for size) + 1 (for offset) <= end()).
  insert_vertex_adjacent_list(i_upper_vertex, i_lower_vertex);
  ++(*std::next(offset.begin(), static_cast<s_size_t>(i_upper_vertex + 1)));
  std::for_each(std::next(offset.begin(), static_cast<s_size_t>(i_upper_vertex + 2)), offset.end(),
                [](std::size_t& off){off += 2;});

  return true;
}

/**
 * @details Resizes the graph. If the graph is smaller than the new size the offset list is grown out, essentially
 * representing the addition of a disconnected point cloud. If the size is, however, smaller than the current size of
 * the graph the vertices with an index greater or equal to the new size are removed. The 'edges' connecting the removed
 * vertices from the remaining vertices are sought and removed from the graph.
 */
void Adjacency_Graph::resize(const std::size_t& size) {

  // Branch based on expansion or contraction of the graph.
  if(size_vertex() < size) {
    offset.resize(size + 1, offset.empty() ? 0 : offset.back());
  }
  else {
    // Resize the containers.
    offset.resize(size + 1);
    vertex_adjacent_list.resize(offset.back());

    // Loop over remaining vertices, looking for edges which no longer exist.
    std::size_t offset_carry = 0;
    FOR(i_vertex, size_vertex()) {
      auto [adjacency_begin, adjacency_end] = vertex_adjacency_iter(i_vertex);
      const auto erase_iter = std::lower_bound(adjacency_begin, adjacency_end, size);
      if(erase_iter != adjacency_end) {
        const auto offset_current = std::distance(erase_iter, adjacency_end);
        vertex_adjacent_list.erase(erase_iter, adjacency_end);
        offset[i_vertex + 1] -= offset_current;
        offset_carry += offset_current;
      }
      offset[i_vertex + 2] -= offset_carry;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------
// Lookup
//--------------------------------------------------------------------------------------------------------------------

/**
 * @details First ensures that the vertex exists in the graph, then using std::ranges looks for the second vertex of the
 * edge in the adjacency graph of the first.
 */
[[nodiscard]] bool Adjacency_Graph::contains(const Edge& edge) const {
  ASSERT_DEBUG(edge.first != edge.second, "Edge vertices identical, " + std::to_string(edge.first) + " and "
                                          + std::to_string(edge.second) + ".");
  const Edge ordered_edge = order_edge_vertex(&edge);
  if(ordered_edge.second >= size_vertex() || empty()) return false;
  auto adjacency = (*this)[ordered_edge.first];
  return std::ranges::any_of(adjacency.begin(), adjacency.end(),
                             [&](const std::size_t& i_vertex) { return ordered_edge.second == i_vertex; });
}

//----------------------------------------------------------------------------------------------------------------------
// Graph Operators
//----------------------------------------------------------------------------------------------------------------------

/**
 * @details To begin new memory is allocated for a new graph. First the offsets for the new graph of determined by
 * computing and storing the offset for each old vertex and storing it in the new vertex position, and then by summing
 * the preceding offsets to obtain the new offset for each vertex. The new vertex lists are populated by coping and
 * renumbering the old vertex list's data to the new positions, using both the permutation vector and the newly computed
 * offsets. Each must also be resorted in an ascending fashion. Finally, the new graph is swapped with the current and
 * returned.
 */
Adjacency_Graph Adjacency_Graph::reorder(const std::vector<std::size_t>& permutation) {

  ASSERT_DEBUG(permutation.size() == size_vertex(), "Incorrect sizes, " + std::to_string(permutation.size()) + " vs. "
                                                    + std::to_string(size_vertex()) + ".");
  ASSERT_DEBUG(std::accumulate(permutation.begin(), permutation.end(), 0.0) == (size_vertex() - 1)*size_vertex()/2.0,
               "Checksum for parsed re_ordering failed, are the elements unique?");

  // Allocate memory.
  Adjacency_Graph graph;
  graph.offset.resize(offset.size());
  graph.vertex_adjacent_list.resize(vertex_adjacent_list.size());

  // Set up the offsets for the new graph.
  if(!empty()) {
    graph.offset[0] = 0;
    FOR(i_old, size_vertex()) graph.offset[permutation[i_old] + 1] = offset[i_old + 1] - offset[i_old];
    FOR(i_new, size_vertex()) graph.offset[i_new + 1] += graph.offset[i_new];
  }

  // Populate the new graph using the reorder mapping.
  FOR(i_old, size_vertex()) {
    const std::size_t& i_new = permutation[i_old];
    auto adjacency_iter = vertex_adjacency_iter(i_old);
    std::transform(adjacency_iter.first, adjacency_iter.second,
                   std::next(graph.vertex_adjacent_list.begin(), static_cast<s_size_t>(graph.offset[i_new])),
                   [permutation](const std::size_t& i_old){return permutation[i_old];});
    adjacency_iter = graph.vertex_adjacency_iter(i_new);
    std::sort(adjacency_iter.first, adjacency_iter.second);
  }

  // swap class data and return.
  this->swap(graph);
  return graph;
}

}