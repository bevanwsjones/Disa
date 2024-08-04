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
// File Name: graph.h
// Description: Contains the declaration and some definitions for an adjacency graph for Disa.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_ADJACENCY_GRAPH_H
#define DISA_ADJACENCY_GRAPH_H

#include "edge.hpp"
#include "macros.hpp"

#include <algorithm>
#include <functional>
#include <limits>
#include <numeric>
#include <span>
#include <vector>

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Adjacency_Graph
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @class Adjacency_Graph
 * @brief An graph G(V, E), where V is a set of vertex indices and E is a set of undirected edges connecting the
 *        vertices in V.
 *
 * @details
 * An adjacency graph G stores a pair of integer sets, V and E. V represents a list of vertex/point/node labels and E
 * contains a set (list) of pairs, each containing two unsigned integers which are elements of V. If V is an ordered
 * continuous set of unsigned integers it can be represented implicitly. An edge E is said to be directed if the order 
 * of the pair matters, i.e. (i, j) != (j, i), else the edge is undirected. Note: in the undirected case the resulting 
 * graph is symmetric. This class thus represents G by storing the connectivity, of each vertex, to its connected 
 * neighbours (rather than the pairs of E directly). This allows for faster search times, similar to other 'sparse' 
 * containers.
 *
 * From a design point of view this class is similar to that of a sparse vector, and thus follows most of the
 * std::vector paradigms, except that each 'element' is dynamically sized and an ordered set of unsigned adjacency
 * values.
 *
 * @note:
 * 1. Where _directed == false edges are undirected, and thus the edge (i, j) == (j, i).
 * 2. Where _directed == true edges are undirected, and thus the edge (i, j) != (j, i). 
 * 3. Currently the graph does not support self connected edges i.e. edges for the form (i, i).
 * 4. Once populated, no information about the edge indexing is stored.
 * 5. The class assumes there is no difference between vertex types (red or blue colourings for example).
 * 6. The class will assume that all vertices, with an index <size_vertex() exist, independet of their connectivity.
 *
 * Future:
 * 1. Look into optimising memory for directed graphs (in terms of max number of nodes, vs elements in adjacency list).
 * 2. Add iterators, begin(), end() etc.
 * 3. Create arithmetic operators +,-, etc for unions and intersections.
 * 4. Possible base graph class from which this class can inherit.
 */
template<bool _directed>
class Adjacency_Graph {

 public:
  // -------------------------------------------------------------------------------------------------------------------
  // Public Constructors and Destructors
  // -------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Default constructor.
   */
  Adjacency_Graph() = default;

  /**
   * @brief Construction via a list of edges connecting two vertices i and j, where i != j.
   * @param[in] edge_graph The list of edge.
   *
   * @note At preset this is not writen efficiently, and can be improved, mainly used for testing.
   */
  Adjacency_Graph(std::initializer_list<Edge> edge_graph);

  /**
   * @brief Default destructor.
   */
  ~Adjacency_Graph() = default;

  // ------------------------------------------------------------------------------------------------------------------
  // Element Access
  // -------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Subscript operator for access to a specified graph vertex's adjacency, with range checking.
   * @param[in] i_vertex The vertex index to the adjacency graph being sought.
   * @return A span of a single vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<const size_t> at(const std::size_t& i_vertex) const {
    ASSERT(i_vertex < size_vertex(),
           "Vertex index " + std::to_string(i_vertex) + " not in range [0, " + std::to_string(i_vertex) + ").");
    return (*this)[i_vertex];
  };

  /**
   * @brief Subscript operator for access to a specified graph vertex's adjacency.
   * @param[in] i_vertex The vertex index to the adjacency graph being sought.
   * @return A span of a single vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<const std::size_t> operator[](const std::size_t& i_vertex) const {
    ASSERT_DEBUG(i_vertex < size_vertex(),
                 "Vertex index " + std::to_string(i_vertex) + " not in range [0, " + std::to_string(i_vertex) + ").");
    return {vertex_adjacent_list.begin() + static_cast<s_size_t>(offset[i_vertex]),
            offset[i_vertex + 1] - offset[i_vertex]};
  }

  /**
   * @brief Subscript operator for access to a specified graph vertex's adjacency.
   * @param[in] i_vertex The vertex index to the adjacency graph being sought.
   * @return A span of a single vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<std::size_t> operator[](const std::size_t& i_vertex) {
    ASSERT_DEBUG(i_vertex < size_vertex(),
                 "Vertex index " + std::to_string(i_vertex) + " not in range [0, " + std::to_string(i_vertex) + ").");
    return {vertex_adjacent_list.begin() + static_cast<s_size_t>(offset[i_vertex]),
            offset[i_vertex + 1] - offset[i_vertex]};
  }

  /**
   * @brief Returns the first vertex's adjacency.
   * @return A const span of a the first vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<const std::size_t> front() const {
    ASSERT_DEBUG(!empty(), "Graph is empty, cannot get the front.");
    return (*this)[0];
  };

  /**
   * @brief Returns the first vertex's adjacency.
   * @return A span of a the first vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<std::size_t> front() {
    ASSERT_DEBUG(!empty(), "Graph is empty, cannot get the front.");
    return (*this)[0];
  };

  /**
   * @brief Returns the last vertex's adjacency.
   * @return A span of a the last vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<const std::size_t> back() const {
    ASSERT_DEBUG(!empty(), "Graph is empty, cannot get the back.");
    return (*this)[size_vertex() - 1];
  };

  /**
   * @brief Returns the last vertex's adjacency.
   * @return A span of a the last vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<std::size_t> back() {
    ASSERT_DEBUG(!empty(), "Graph is empty, cannot get the back.");
    return (*this)[size_vertex() - 1];
  };

  /**
    * @brief Direct access to the underlying vectors of the graph.
    * @return pair [pointer to vertex start, pointer to offset start].
    *
    * @note If empty all pointers returned pointers will be nullptrs. if the only the vertex list is empty a nullptr is
    *       returned for, while the pointer to the offset vector is returned. Finally populated pointers to both vectors
    *       are returned.
    */
  [[nodiscard]] inline std::pair<std::size_t*, std::size_t*> data() noexcept {
    if(empty()) return std::make_pair(nullptr, nullptr);
    else if(vertex_adjacent_list.empty()) return std::make_pair(nullptr, offset.data());
    else return std::make_pair(vertex_adjacent_list.data(), offset.data());
  }

  // -------------------------------------------------------------------------------------------------------------------
  // Capacity
  // -------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Checks whether the graph is empty. An empty graph is considered where there are no vertices.
   * @return True if the graph is empty, else false.
   */
  [[nodiscard]] inline bool empty() const noexcept { return offset.empty() || offset.size() < 2; }

  /**
   * @brief Returns the number of vertices in the graph.
   * @return The number of vertices.
   */
  [[nodiscard]] inline std::size_t size_vertex() const noexcept { return !offset.empty() ? offset.size() - 1 : 0; }

  /**
   * @brief Returns the number of edge in the graph.
   * @return The number of edge.
   */
  [[nodiscard]] inline std::size_t size_edge() const noexcept {
    return !_directed ? vertex_adjacent_list.size() / 2 : vertex_adjacent_list.size();
  }

  /**
   * @brief Returns the number of vertices and edges in the graph.
   * @return Pair containing [vertices, edges].
   */
  [[nodiscard]] inline std::pair<std::size_t, std::size_t> size() const noexcept {
    return std::make_pair(size_vertex(), size_edge());
  }

  /**
   * @brief Reserves enough memory such that the data structure can be filled with the parsed vertices and/or edges.
   * @param[in] size_vertex The total number of vertices to reserve for.
   * @param[in] size_edge The total number of edge to reserve for (defaults to zero).
   */
  inline void reserve(std::size_t size_vertex, std::size_t size_edge = 0) noexcept {
    offset.reserve(size_vertex + 1);
    vertex_adjacent_list.reserve(size_edge * (!_directed ? 2 : 1));
  }

  /**
   * @brief Returns the current number of vertices and edges the graph has capacity (memory for).
   * @return Pair containing [vertices, edges].
   */
  [[nodiscard]] inline std::pair<std::size_t, std::size_t> capacity() const noexcept {
    return std::make_pair(!offset.capacity() ? 0 : offset.capacity() - 1,
                          !_directed ? vertex_adjacent_list.capacity() / 2 : vertex_adjacent_list.capacity());
  }

  /**
   * @brief reduces memory usage of the graph by freeing unused memory for both vertices and edges.
   */
  inline void shrink_to_fit() noexcept {
    vertex_adjacent_list.shrink_to_fit();
    offset.shrink_to_fit();
  }

  // -------------------------------------------------------------------------------------------------------------------
  // Modifiers
  // -------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Clears the contents of the graph, removing all vertices and edges.
   */
  inline void clear() noexcept {
    vertex_adjacent_list.clear();
    offset.clear();
  }

  /**
   * @brief Inserts an edge into the graph, if the edges exists nothing occurs.
   * @param[in] edge The edge to insert.
   * @return true if an edge was inserted, else false.
   */
  bool insert(const Edge& edge);

  /**
   * @brief Erase function, for deleting/erase multiple vertices at the same time.
   * @tparam _unary_predicate Should returns true if the element should be erased.
   * @param[in] delete_vertex The instance of the _unary_predicate.
   *
   * @note Can cause the graph to become disjoint.
   */
  template<class _unary_predicate>
  void erase_if(_unary_predicate delete_vertex);

  /**
   * @brief Resizes the number of vertices in the graph.
   * @size[in] The number of vertices to resize the graph to.
   *
   * @note Edges are removed if the graph size is reduced.
   */
  void resize(const std::size_t& size);

  /**
   * @brief Swaps the contents of the graph with the parsed graph
   * @param[in,out] graph_other The other graph, this graph will obtain the other's vertices and edges, and visa versa.
   */
  inline void swap(Adjacency_Graph& graph_other) {
    vertex_adjacent_list.swap(graph_other.vertex_adjacent_list);
    offset.swap(graph_other.offset);
  }

  // -------------------------------------------------------------------------------------------------------------------
  // Lookup
  // -------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Checks if the edge exists in the graph.
   * @param[in] edge The edge to check for.
   * @return True if the graph contains the edge, else false.
   */
  [[nodiscard]] bool contains(const Edge& edge) const;

  // -------------------------------------------------------------------------------------------------------------------
  // Graph Operators
  // -------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Computes the degree of a vertex: defined as the number of vertices it adjacent (connected) to.
   * @param[in] i_vertex The vertex index for which to compute the degree.
   * @return The degree of the vertex.
   */
  [[nodiscard]] inline std::size_t degree(const std::size_t& i_vertex) const {
    ASSERT_DEBUG(i_vertex < size_vertex(),
                 "Vertex index " + std::to_string(i_vertex) + " not in range [0, " + std::to_string(i_vertex) + ").");
    return offset[i_vertex + 1] - offset[i_vertex];
  }

  /**
   * @brief Reorders this graph given a new index numbering.
   * @param[in] permutation The permutation vector, mapping new_index = permutation[old_index].
   * @return The old, un-permuted, adjacency graph.
   */
  Adjacency_Graph reorder(const std::vector<std::size_t>& permutation);

  // -------------------------------------------------------------------------------------------------------------------
  // Private Members
  // -------------------------------------------------------------------------------------------------------------------

 protected:
  std::vector<std::size_t> vertex_adjacent_list;  //!< Single contiguous list of all vertex adjacency for the graph.
  std::vector<std::size_t> offset;                //!< List pointing to the start of each vertex's adjacency graph.

  // -------------------------------------------------------------------------------------------------------------------
  // Helper Functions
  // -------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Returns iterators to the beginning and end of a vertex adjacency graph.
   * @param[in] i_vertex The vertex index to the adjacency graph being sought.
   * @return pair of iterators, [beginning of the vertex adjacency graph, end of the vertex adjacency graph].
   */
  [[nodiscard]] inline std::pair<std::vector<std::size_t>::iterator, std::vector<std::size_t>::iterator>
  vertex_adjacency_iter(const std::size_t& i_vertex) {
    return std::make_pair(std::next(vertex_adjacent_list.begin(), static_cast<s_size_t>(offset[i_vertex])),
                          std::next(vertex_adjacent_list.begin(), static_cast<s_size_t>(offset[i_vertex + 1])));
  }

  /**
   * @brief Inserts into the vertex adjacent list a new vertex in a sorted manner.
   * @param[in] vertex The vertex to have its adjacency updated.
   * @param[in] insert_vertex The new vertex index to insert.
   *
   * @warning This operation breaks the graph if not used correctly, as the offsets are not updated. Further, if the
   *          offset vector is not upto date this operation produces undefined behaviour.
   */
  void insert_vertex_adjacent_list(std::size_t vertex, std::size_t insert_vertex);
};

// --------------------------------------------------------------------------------------------------------------------
// Operator Overloading
// ---------------------------------------------------------------------------------------------------------------------
// Stream Operators
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Overloaded stream operator for the out stream, writes out a formatted graph.
 * @param[in] ostream The out stream to write to.
 * @param[in] graph The graph to write.
 * @return Returns the ostream, with the graph writen out.
 */
template<bool _directed>
std::ostream& operator<<(std::ostream& ostream, const Adjacency_Graph<_directed>& graph);

// ---------------------------------------------------------------------------------------------------------------------
// Adjacency_Graph Template Definitions
// ---------------------------------------------------------------------------------------------------------------------
// Public Constructors and Destructors
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details Initializes an Adjacency_Graph object with the edges in the given initializer list. The constructor finds
 * the vertex with the highest index to determine a suitable reserve for the graph in the provided edge list. Each edge
 * is then inserted into the edge and any excess reserve memory removed.
 */
template<bool _directed>
Adjacency_Graph<_directed>::Adjacency_Graph(std::initializer_list<Edge> edge_graph) {
  const auto iter = std::max_element(edge_graph.begin(), edge_graph.end(),
                                     !_directed ?
                                      [](const Edge& edge_0, const Edge& edge_1) {
                                          return order_edge_vertex(&edge_0).second < order_edge_vertex(&edge_1).second;
                                        } :
                                      [](const Edge& edge_0, const Edge& edge_1) {
                                          return edge_0.second < edge_1.second;
                                        });
  reserve(!_directed ? std::max(iter->first, iter->second) : iter->first, edge_graph.size());
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
template<bool _directed>
bool Adjacency_Graph<_directed>::insert(const Edge& edge) {
  ASSERT_DEBUG(edge.first != edge.second,
               "Edge vertices identical, " + std::to_string(edge.first) + " and " + std::to_string(edge.second) + ".");

  // Preliminaries check if the edge exists, then determine if we need to resize.
  if(contains(edge)) return false;
  const auto& [i_first_vertex, i_second_vertex] =
  !_directed ? order_edge_vertex(&edge) : std::pair<const std::size_t&, const std::size_t&>({edge.first, edge.second});
  if(std::max(i_first_vertex, i_second_vertex) >= size_vertex()) resize(std::max(i_first_vertex, i_second_vertex) + 1);

  // Insert lower vertex.
  insert_vertex_adjacent_list(i_first_vertex, i_second_vertex);
  std::for_each(std::next(offset.begin(), static_cast<s_size_t>(i_first_vertex + 1)),
                !_directed ? std::next(offset.begin(), static_cast<s_size_t>(i_second_vertex + 2)) : offset.end(),
                [](std::size_t& off) { off++; });
  if(_directed) return true;

  // Insert upper vertex (tricky: last for loop is safe -> The highest vertex + 1 (for size) + 1 (for offset) <= end()).
  insert_vertex_adjacent_list(i_second_vertex, i_first_vertex);
  ++(*std::next(offset.begin(), static_cast<s_size_t>(i_second_vertex + 1)));
  std::for_each(std::next(offset.begin(), static_cast<s_size_t>(i_second_vertex + 2)), offset.end(),
                [](std::size_t& off) { off += 2; });

  return true;
}

/**
 * @details Loops over the data and determines the new offsets and which entries in vertex adjacent list need to be
 * removed, updating the offsets and recording the entries to be deleted. After which both the offset vector and the
 * vertex adjacency list are 'looped' over and all entries flagged for removal are erased. Finally all vertices in the
 * vertex adjacency list are relabeled.
 */
template<bool _directed>
template<class _unary_predicate>
void Adjacency_Graph<_directed>::erase_if(_unary_predicate delete_vertex) {

  std::size_t removed = 0;
  std::size_t i_vertex = 0;
  std::vector<bool> adjacency_delete(vertex_adjacent_list.size(), false);
  std::vector<std::size_t> new_indexes(vertex_adjacent_list.size(), std::numeric_limits<std::size_t>::max());

  // Loop over the data, and determine the new offsets and which entries in vertex_adjacent_list need to be removed.
  FOR(i_vertex_old, size_vertex()) {
    const auto& adjacency = vertex_adjacency_iter(i_vertex_old);
    const auto& begin_delete = adjacency_delete.begin() + std::distance(vertex_adjacent_list.begin(), adjacency.first);
    const auto& end_delete = adjacency_delete.begin() + std::distance(vertex_adjacent_list.begin(), adjacency.second);

    offset[i_vertex_old] -= removed;

    // Only remove vertices which will be taken out, and record new indexes.
    if(!delete_vertex(i_vertex_old)) {
      new_indexes[i_vertex_old] = i_vertex++;
      removed += std::count_if(adjacency.first, adjacency.second, delete_vertex);
      std::transform(adjacency.first, adjacency.second, begin_delete, delete_vertex);
    } else {  // All adjacency entries need to be removed, since the vertex is to be removed.
      removed += std::distance(adjacency.first, adjacency.second);
      std::transform(begin_delete, end_delete, begin_delete, [](const auto) { return true; });
    }
  }
  offset.back() -= removed;

  // Erase offset entries for the removed vertices.
  i_vertex = 0;
  offset.erase(std::remove_if(offset.begin(), offset.end(),
                              [&](const auto) { return i_vertex < size_vertex() ? delete_vertex(i_vertex++) : false; }),
               offset.end());

  // Erase removed vertices from the adjacency lists, and then relabel.
  i_vertex = 0;
  vertex_adjacent_list.erase(std::remove_if(vertex_adjacent_list.begin(), vertex_adjacent_list.end(),
                                            [&](const auto) { return adjacency_delete[i_vertex++]; }),
                             vertex_adjacent_list.end());

  std::transform(vertex_adjacent_list.begin(), vertex_adjacent_list.end(), vertex_adjacent_list.begin(),
                 [&](const auto& i_old_vertex) { return new_indexes[i_old_vertex]; });

  ASSERT(offset.back() == vertex_adjacent_list.size(),
         "Total offsets no longer match vertex size while reducing to subgraph, " + std::to_string(offset.back()) +
         " vs. " + std::to_string(vertex_adjacent_list.size()) + ".");
};

/**
 * @details Resizes the graph. If the graph is smaller than the new size the offset list is grown out, essentially
 * representing the addition of a disconnected point cloud. If the size is, however, smaller than the current size of
 * the graph the vertices with an index greater or equal to the new size are removed. The 'edges' connecting the removed
 * vertices from the remaining vertices are sought and removed from the graph.
 */
template<bool _directed>
void Adjacency_Graph<_directed>::resize(const std::size_t& size) {

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

// -------------------------------------------------------------------------------------------------------------------
// Lookup
// -------------------------------------------------------------------------------------------------------------------

/**
 * @details First ensures that the vertex exists in the graph, then using std::ranges looks for the second vertex of the
 * edge in the adjacency graph of the first.
 */
template<bool _directed>
[[nodiscard]] bool Adjacency_Graph<_directed>::contains(const Edge& edge) const {
  ASSERT_DEBUG(edge.first != edge.second,
               "Edge vertices identical, " + std::to_string(edge.first) + " and " + std::to_string(edge.second) + ".");

  if(empty()) return false;
  if(std::max(edge.first, edge.second) >= size_vertex()) return false;
  auto iter = std::lower_bound((*this)[edge.first].begin(), (*this)[edge.first].end(), edge.second);
  return iter != (*this)[edge.first].end() && *iter == edge.second;
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
template<bool _directed>
Adjacency_Graph<_directed> Adjacency_Graph<_directed>::reorder(const std::vector<std::size_t>& permutation) {

  ASSERT_DEBUG(permutation.size() == size_vertex(), "Incorrect sizes, " + std::to_string(permutation.size()) + " vs. " +
                                                    std::to_string(size_vertex()) + ".");
  ASSERT_DEBUG(
  std::accumulate(permutation.begin(), permutation.end(), 0.0) == (size_vertex() - 1) * size_vertex() / 2.0,
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
                   [permutation](const std::size_t& i_old) { return permutation[i_old]; });
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
 * inserted vertex is already present in the adjacency list, this function does nothing.
 */
template<bool _directed>
void Adjacency_Graph<_directed>::insert_vertex_adjacent_list(std::size_t vertex, std::size_t insert_vertex) {
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
 * @details Outputs the subgraph data to 'screen', each vertex is printed, followed by its adjacent vertices. If the
 * vertex adjacency is empty then a period is placed in the row.
 *
 * The output format looks like:
 * 1, 3
 * 0
 * .
 * 0, 1
 */
template<bool _directed>
std::ostream& operator<<(std::ostream& ostream, const Adjacency_Graph<_directed>& graph) {
  FOR(i_vertex, graph.size_vertex()) {
    if(i_vertex != 0) ostream << "\n";
    FOR_EACH(adjacent_vertex, graph[i_vertex])
    ostream << adjacent_vertex << (adjacent_vertex != graph[i_vertex].back() ? ", " : "");
    if(graph[i_vertex].empty()) ostream << ".";
  }
  return ostream;
}

}  // namespace Disa

//----------------------------------------------------------------------------------------------------------------------
// STL specialisation
//----------------------------------------------------------------------------------------------------------------------

namespace std {

/**
 * @struct hash<Disa::Adjacency_Graph>
 * @brief Template specialization to provides hash function for the Adjacency_Graph.
 */
template<bool _directed>
struct hash<Disa::Adjacency_Graph<_directed>> {

  /**
   * @brief Computes the hash value of a given Disa::Adjacency_Graph object.
   * @param graph The graph to be hashed.
   * @returns A std::size_t value representing the hash value of the input graph.
   */
  std::size_t operator()(const Disa::Adjacency_Graph<_directed>& graph) const noexcept;
};

/**
 * @details Specialization of the std::hash template for the Disa::Adjacency_Graph class. It calculates a hash value for
 * the adjacency graph by XORing the hash values of its size, number of edges, and sizes of its first and last vertices.
 * If the graph is empty, the hash value is set to zero.
 */
template<bool _directed>
std::size_t hash<Disa::Adjacency_Graph<_directed>>::operator()(
const Disa::Adjacency_Graph<_directed>& graph) const noexcept {
  if(graph.empty()) return 0;  // All empty graphs are considered identical.
  std::size_t hashValue = 0;
  hashValue ^= std::hash<std::size_t>{}(graph.size_vertex());
  hashValue ^= std::hash<std::size_t>{}(graph.size_edge());
  hashValue ^= std::hash<std::size_t>{}(graph.front().size());
  hashValue ^= std::hash<std::size_t>{}(graph.back().size());
  return hashValue;
}

}  // namespace std

#endif  //DISA_ADJACENCY_GRAPH_H
