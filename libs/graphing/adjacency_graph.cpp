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
bool AdjacencyGraph::insert(const Edge& edge) {
  ASSERT_DEBUG(edge.first != edge.second, "Edge vertices identical, " + std::to_string(edge.first) + " and "
                                          + std::to_string(edge.second) + ".");

  // Preliminaries check if the edge exists, then determine if we need to resize.
  if(contains(edge)) return false;
  const auto& [i_lower_vertex, i_upper_vertex] = order_edge_vertex(&edge);
  if(i_upper_vertex >= size_vertex()) resize(i_upper_vertex + 1);
  auto adjacency = vertex_adjacency_iter(i_lower_vertex);

  // Insert lower vertex.
  auto insert_iter = std::lower_bound(adjacency.first, adjacency.second, i_upper_vertex);
  auto distance = std::distance(vertex_adjacent_list.begin(), insert_iter);
  vertex_adjacent_list.insert(vertex_adjacent_list.begin() + distance, i_upper_vertex);
  for(auto offs = std::next(offset.begin(), static_cast<s_size_t>(i_lower_vertex + 1));
      offs != std::next(offset.begin(), static_cast<s_size_t>(i_upper_vertex + 1)); ++(*offs++));

  // Insert upper vertex
  adjacency = vertex_adjacency_iter(i_upper_vertex);
  insert_iter = std::lower_bound(adjacency.first, adjacency.second, i_lower_vertex);
  distance = std::distance(vertex_adjacent_list.begin(), insert_iter);
  vertex_adjacent_list.insert(vertex_adjacent_list.begin() + distance, i_lower_vertex);
  for(auto offs = std::next(offset.begin(), static_cast<s_size_t>(i_upper_vertex + 1));
      offs != offset.end(); ++offs)
    *offs += 2;

  return true;
}

/**
 * @details Resizes the graph. If the graph is smaller than the new size the offset list is grown out, essentially
 * representing the addition of a disconnected point cloud. If the size is, however, smaller than the current size of
 * the graph the vertices with an index greater or equal to the new size are removed. The 'edges' connecting the removed
 * vertices from the remaining vertices are sought and removed from the graph.
 */
void AdjacencyGraph::resize(const std::size_t& size) {

  // Branch based on expansion or contraction of the graph.
  if(size_vertex() < size) {
    offset.resize(size + 1, offset.empty() ? 0 : offset.back());
  } else {
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
[[nodiscard]] bool AdjacencyGraph::contains(const Edge& edge) const {
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
 * @details To begin new memory is allocated for a new graph. Using the parsed re-ordering/numbering map (re_order), the
 * new vertices are iterated over. The offset vector is populated by pushing back the back() offset value plus the
 * difference between the corresponding old vertex and the 'next-old' vertex offset difference. The vertex adjacency
 * list is populated by copying the old vertex adjacency range to the back of the new adjacency range and performing a
 * renumbering in the process. Finally the new graph is swapped with the current and returned.
 */
AdjacencyGraph AdjacencyGraph::reorder(const std::vector<std::size_t>& re_order) {

  ASSERT_DEBUG(re_order.size() == size_vertex(), "Incorrect sizes, " + std::to_string(re_order.size()) + " vs. "
                                                 + std::to_string(size_vertex()) + ".");
  ASSERT_DEBUG(std::accumulate(re_order.begin(), re_order.end(), 0.0) == (size_vertex() - 1)*size_vertex()/2.0,
               "Checksum for parsed re_ordering, are the elements unique?");

  // Allocate memory.
  AdjacencyGraph graph;
  graph.offset.reserve(offset.size());
  graph.vertex_adjacent_list.reserve(vertex_adjacent_list.size());

  // populate the new graph using the reorder mapping.
  graph.offset.push_back(0);
  FOR(i_vertex, size_vertex()) {
    graph.offset.push_back(graph.offset.back() + offset[re_order[i_vertex] + 1] - offset[re_order[i_vertex]]);
    auto adjacency_iter = vertex_adjacency_iter(re_order[i_vertex]);
    std::transform(adjacency_iter.first, adjacency_iter.second, std::back_inserter(graph.vertex_adjacent_list),
                   [re_order](const std::size_t& new_vertex) { return re_order[new_vertex]; });
    adjacency_iter = graph.vertex_adjacency_iter(i_vertex);
    std::sort(adjacency_iter.first, adjacency_iter.second);
  }

  // swap class data and return.
  this->swap(graph);
  return graph;
}

}