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

// ---------------------------------------------------------------------------------------------------------------------
// AdjacencyGraph
// ---------------------------------------------------------------------------------------------------------------------
// Public Constructors and Destructors
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details Initializes an Adjacency_Graph object with the edges in the given initializer list. The constructor finds
 * the vertex with the highest index to determine a suitable reserve for the graph in the provided edge list. Each edge
 * is then inserted into the edge and any excess reserve memory removed.
 */
Adjacency_Graph::Adjacency_Graph(std::initializer_list<Edge> edge_graph) {
  const auto iter = std::max_element(edge_graph.begin(), edge_graph.end(),
                                     [](const Edge& edge_0, const Edge& edge_1) {
                                       return order_edge_vertex(&edge_0).second < order_edge_vertex(&edge_1).second;
                                     });
  reserve(std::max(iter->first, iter->second), edge_graph.size());
  FOR_EACH(edge, edge_graph) insert(edge);
  shrink_to_fit();
}

// ---------------------------------------------------------------------------------------------------------------------
// Modifiers
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details If the edge is already in the graph the function returns immediately. If the edge contains a vertex index
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

// ---------------------------------------------------------------------------------------------------------------------
// Lookup
// ---------------------------------------------------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------------------------------------------------
// Graph Operators
// ---------------------------------------------------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------------------------------------------------
// Helper Functions
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details This function inserts a new vertex into the adjacency list of the specified vertex in the graph. If the
 * inserted vertex is already present in the adjacency list, this function does nothing. It uses a binary search to
 * find the correct position in the adjacency list to insert the new vertex, and then inserts the new vertex into the
 * list using the `std::vector::insert()` function.
 */
void Adjacency_Graph::insert_vertex_adjacent_list(std::size_t vertex, std::size_t insert_vertex) {
  const auto& adjacency = vertex_adjacency_iter(vertex);
  const auto& insert_iter = std::lower_bound(adjacency.first, adjacency.second, insert_vertex);
  const auto& distance = std::distance(vertex_adjacent_list.begin(), insert_iter);
  vertex_adjacent_list.insert(vertex_adjacent_list.begin() + distance, insert_vertex);
}

//----------------------------------------------------------------------------------------------------------------------
// Operator Overloading
//----------------------------------------------------------------------------------------------------------------------
// Stream Operators
//----------------------------------------------------------------------------------------------------------------------

/**
 * @details This function overloads the `<<` operator to enable printing an adjacency graph to an output stream.
 * It iterates over each vertex in the graph and prints its adjacent vertices, separated by commas. If a vertex has
 * no adjacent vertices, a period is printed. Each vertex and its adjacent vertices are printed on a separate line,
 * with no trailing newline character.
 */

std::ostream& operator<<(std::ostream& ostream, const Adjacency_Graph& graph) {
  FOR(i_vertex, graph.size_vertex()) {
    if(i_vertex != 0) ostream<<"\n";
    FOR_EACH(adjacent_vertex, graph[i_vertex])
      ostream<<adjacent_vertex<<(adjacent_vertex != graph[i_vertex].back() ? ", " : "");
    if(graph[i_vertex].empty()) ostream<<".";
  }
  return ostream;
}

}

//----------------------------------------------------------------------------------------------------------------------
// STL specialisation
//----------------------------------------------------------------------------------------------------------------------

namespace std {

/**
 * @details This is a specialization of the `std::hash` template for the `Disa::Adjacency_Graph` class. It calculates
 * a hash value for the adjacency graph by XORing the hash values of its size, number of edges, and sizes of its first
 * and last vertices. If the graph is empty, the hash value is set to zero. The function is marked `noexcept` because
 * it doesn't throw any exceptions.
 */
std::size_t hash<Disa::Adjacency_Graph>::operator()(const Disa::Adjacency_Graph& graph) const noexcept {
  if(graph.empty()) return 0; // All empty graphs are considered identical.
  std::size_t hashValue = 0;
  hashValue ^= std::hash<std::size_t> {}(graph.size_vertex());
  hashValue ^= std::hash<std::size_t> {}(graph.size_edge());
  hashValue ^= std::hash<std::size_t> {}(graph.front().size());
  hashValue ^= std::hash<std::size_t> {}(graph.back().size());
  return hashValue;
}

}
