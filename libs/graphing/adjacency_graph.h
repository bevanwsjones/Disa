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

#include "edge.h"
#include "macros.h"

#include <algorithm>
#include <functional>
#include <limits>
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
 * continuous set of unsigned integers it can be represented implicitly. This class thus represents G by storing the
 * connectivity, of each vertex, to its connected neighbours rather than V and E directly. This allowing for single
 * faster search times, similar to other 'sparse' containers.
 *
 * From a design point of view this class is similar to that of a sparse vector, and thus follows most of the
 * std::vector paradigms, except that each 'element' is dynamically sized and an ordered set of unsigned adjacency
 * values.
 *
 * @notes:
 * 1. Edges are undirected, and thus the edge connecting vertex i to j is the same as the edge connecting j to i.
 * 2. Currently the graph does not support self connected edges i.e. edges i to i.
 * 3. Once populated, no information about the edge indexing is stored.
 * 4. The graph cannot contain information for connectivity between two types of vertices (i.e. a red set and a blue
 *    set where each set begins indexing at 0).
 *
 * Future:
 * 1. Add iterators, begin(), end() etc.
 * 2. Create arithmetic operators +,-, etc for unions and intersections.
 * 3. Possible base graph class from which this class can inherit.
 */
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
  [[nodiscard]] inline std::size_t size_edge() const noexcept { return vertex_adjacent_list.size()/2; }

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
    vertex_adjacent_list.reserve(size_edge*2);
  }

  /**
   * @brief Returns the current number of vertices and edges the graph has capacity (memory for).
   * @return Pair containing [vertices, edges].
   */
  [[nodiscard]] inline std::pair<std::size_t, std::size_t> capacity() const noexcept {
    return std::make_pair(!offset.capacity() ? 0 : offset.capacity() - 1, vertex_adjacent_list.capacity()/2);
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
   * @param[in,out] other The other graph, this graph will obtain the other's vertices and edges, and visa versa.
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
    ASSERT_DEBUG(i_vertex < size_vertex(), "Vertex index " + std::to_string(i_vertex) + " not in range [0, "
                                           + std::to_string(i_vertex) + ").");
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
  std::vector<std::size_t> vertex_adjacent_list;    //!< Single contiguous list of all vertex adjacency for the graph.
  std::vector<std::size_t> offset;                  //!< List pointing to the start of each vertex's adjacency graph.

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
std::ostream& operator<<(std::ostream& ostream, const Adjacency_Graph& graph);

}

// Add template definitions
#include "adjacency_graph.hpp"

// ---------------------------------------------------------------------------------------------------------------------
// STL specialisation
// ---------------------------------------------------------------------------------------------------------------------

namespace std {

/**
 * @struct hash<Disa::Adjacency_Graph>
 * @brief Template specialization to provides hash function for the Adjacency_Graph.
 */
template<>
struct hash<Disa::Adjacency_Graph> {

  /**
   * @brief Computes the hash value of a given Disa::Adjacency_Graph object.
   * @param graph The graph to be hashed.
   * @returns A std::size_t value representing the hash value of the input graph.
   */
  std::size_t operator()(const Disa::Adjacency_Graph& graph) const noexcept;
};

}

#endif //DISA_ADJACENCY_GRAPH_H
