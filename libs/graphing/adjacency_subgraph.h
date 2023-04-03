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
// File Name: adjacency_subgraph.h
// Description: Declarations of the adjacency subgraph class.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_ADJACENCY_SUB_GRAPH_H
#define DISA_ADJACENCY_SUB_GRAPH_H

#include "adjacency_graph.h"

#include "graph_utilities.h"

#include <functional>
#include <memory>
#include <vector>

namespace Disa {

/**
 * @class AdjacencySubgraph
 * @brief Represents a subgraph G' that is a subset of a parent adjacency graph G.
 *
 * @details
 * The Adjacency Subgraph class is used to represent a subgraph that is a subset of a parent graph. It functions
 * similarly to a standard graph, but also stores additional data that describes its relationship with the parent graph.
 * This includes a hash of the parent graph to ensure its integrity, global vertex indices of local vertices, and the
 * ability to add additional levels to the graph.
 *
 * Levels in this context refer to 'level traversal', and form a halo around the primary partition. This allows the
 * subgraph to store overlapping (with other subgraphs) vertices. All vertices in the primary partition are have a level
 * value of 0, and then each successive level after this increases the level by 1.
 *
 * @warning
 * 1. The relationship between the parent graph and the subgraph is logical, and not enforced. Any update to the
 *    topology of the parent or subgraph will invalidate their correlation.
 * 2. Some edge and vertex operations may produce different results in the subgraph compared to the parent graph. For
 *    example, checking the degree of a peripheral vertex in the subgraph will most likely give a different value than
 *    checking the same vertex in the parent graph. Similarly, the "contains" method may give different results since
 *    it uses edge data.
 *
 * Future:
 * 1. Add iterators (e.g., begin(), end()).
 * 2. Create arithmetic operators (+, -, etc.) for unions and intersections.
 * 3. Consider creating a base graph class from which this class can inherit.
 */
class Adjacency_Subgraph {

public:

  /**
   * @brief Default constructor.
   * @return An empty Adjacency_Subgraph.
   */
  Adjacency_Subgraph() = default;

  /**
   * @brief Default destructor.
   */
  ~Adjacency_Subgraph() = default;

  /**
   * @breif Constructor for creating a subgraph from a parent graph.
   * @param[in] parent_graph The parent graph upon which this subgraph will be constructed.
   * @param[in] i_partition_local_global For this partition of the parent graph, the local to global index mapping.
   * @param[in] extra_levels Additional levels to add to the primary partition.
   * @return Constructed Adjacency_Subgraph
   */
  Adjacency_Subgraph(Adjacency_Graph& parent_graph, const std::vector<std::size_t>& i_sub_graph_vertex,
                     std::size_t extra_levels = 0);

  //--------------------------------------------------------------------------------------------------------------------
  // Element Access
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Subscript operator for access to a specified graph vertex's adjacency, with range checking.
   * @param[in] i_vertex The vertex index to the adjacency graph being sought.
   * @return A span of a single vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<const size_t> at(const std::size_t& i_vertex) const { return graph[i_vertex]; }

  /**
   * @brief Subscript operator for access to a specified graph vertex's adjacency.
   * @param[in] i_vertex The vertex index to the adjacency graph being sought.
   * @return A span of a single vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<const std::size_t> operator[](const std::size_t& i_vertex) const {
    return graph[i_vertex];
  }

  /**
   * @brief Subscript operator for access to a specified graph vertex's adjacency.
   * @param[in] i_vertex The vertex index to the adjacency graph being sought.
   * @return A span of a single vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<std::size_t> operator[](const std::size_t& i_vertex) { return graph[i_vertex]; }

  /**
   * @brief Returns the first vertex's adjacency.
   * @return A const span of a the first vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<const std::size_t> front() const { return graph.front(); }

  /**
   * @brief Returns the first vertex's adjacency.
   * @return A span of a the first vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<std::size_t> front() { return graph.front(); }

  /**
   * @brief Returns the last vertex's adjacency.
   * @return A span of a the last vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<const std::size_t> back() const { return graph.back(); }

  /**
   * @brief Returns the last vertex's adjacency.
   * @return A span of a the last vertex adjacency in the graph.
   */
  [[nodiscard]] inline std::span<std::size_t> back() { return graph.back(); }

  /**
    * @brief Direct access to the underlying vectors of the graph.
    * @return pair [pointer to vertex start, pointer to offset start].
    *
    * @note If empty all pointers returned pointers will be nullptrs. if the only the vertex list is empty a nullptr is
    *       returned for, while the pointer to the offset vector is returned. Finally populated pointers to both vectors
    *       are returned.
    */
  [[nodiscard]] inline std::tuple<std::size_t*, std::size_t*, std::size_t*, std::size_t*> data() noexcept {
    const std::pair<std::size_t*, std::size_t*> graph_data = graph.data();
    return std::make_tuple(graph_data.first, graph_data.second,
                           i_local_global.empty() ? nullptr : i_local_global.data(),
                           level_set_value.empty() ? nullptr : level_set_value.data());
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Capacity
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Checks whether the graph is empty. An empty graph is considered where there are no vertices.
   * @return True if the graph is empty, else false.
   */
  [[nodiscard]] inline bool empty() const noexcept { return graph.empty(); }

  /**
   * @brief Returns the number of vertices in the graph.
   * @return The number of vertices.
   */
  [[nodiscard]] inline std::size_t size_vertex() const noexcept { return graph.size_vertex(); }

  /**
   * @brief Returns the number of edge in the graph.
   * @return The number of edge.
   */
  [[nodiscard]] inline std::size_t size_edge() const noexcept { return graph.size_edge(); }

  /**
   * @brief Returns the number of vertices and edges in the graph.
   * @return Pair containing [vertices, edges].
   */
  [[nodiscard]] inline std::pair<std::size_t, std::size_t> size() const noexcept { return graph.size(); }

  /**
   * @brief Reserves enough memory such that the data structure can be filled with the parsed vertices and/or edges.
   * @param[in] size_vertex The total number of vertices to reserve for.
   * @param[in] size_edge The total number of edge to reserve for (defaults to zero).
   */
  void inline reserve(std::size_t size_vertex, std::size_t size_edge = 0) noexcept {
    graph.reserve(size_vertex, size_edge);
    i_local_global.reserve(size_vertex);
    level_set_value.reserve(size_vertex);
  }

  /**
   * @brief Returns the current number of vertices and edges the graph has capacity (memory for).
   * @return tuple containing capacities for [vertices, edges].
   */
  [[nodiscard]] inline std::tuple<std::size_t, std::size_t, std::size_t, std::size_t> capacity() const noexcept {
    return {graph.capacity().first, graph.capacity().second, i_local_global.capacity(),
            level_set_value.capacity()};
  }

  /**
   * @brief reduces memory usage of the graph by freeing unused memory for both vertices and edges.
   */
  inline void shrink_to_fit() noexcept {
    graph.shrink_to_fit();
    i_local_global.shrink_to_fit();
    level_set_value.shrink_to_fit();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Modifiers
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Clears the contents of the graph, removing all vertices and edges.
   */
  inline void clear() {
    graph.clear();
    hash_parent = std::hash<Adjacency_Graph>{}(graph);
    i_local_global.clear();
    level_set_value.clear();
  }

  /**
   * @brief Resizes the number of vertices in the graph.
   * @size[in] The number of vertices to resize the graph to.
   *
   * @note Edges are removed if the graph size is reduced. And the parent graph is cleared.
   * @note Invalidates the parent hash if size is increased.
   */
  inline void resize(const std::size_t size) {
    if(size > size_vertex()) hash_parent = std::hash<Adjacency_Graph>{}(Adjacency_Graph());
    graph.resize(size);
    i_local_global.resize(size);
    if(!level_set_value.empty()) level_set_value.resize(size);
  };

  /**
   * @brief Swaps the contents of the graph with the parsed graph
   * @param[in,out] other The other graph, this graph will obtain the other's vertices and edges, and visa versa.
   */
  inline void swap(Adjacency_Subgraph& graph_other) {
    graph.swap(graph_other.graph);
    i_local_global.swap(graph_other.i_local_global);
    level_set_value.swap(graph_other.level_set_value);
    std::swap(hash_parent, graph_other.hash_parent);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Lookup
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Checks if the edge exists in the graph.
   * @param[in] edge The edge to check for.
   * @return True if the graph contains the edge, else false.
   */
  [[nodiscard]] bool contains(const Edge& edge) const {return graph.contains(edge); };

  //--------------------------------------------------------------------------------------------------------------------
  // Graph Operators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Computes the degree of a vertex: defined as the number of vertices it adjacent (connected) to.
   * @param[in] i_vertex The vertex index for which to compute the degree.
   * @return The degree of the vertex.
   */
  [[nodiscard]] inline std::size_t degree(const std::size_t& i_vertex) const { return graph.degree(i_vertex); }

  /**
   * @brief Is the vertex local to this subgraph.
   * @param[in] i_vertex The vertex to check.
   * @return True if the vertex is part of this subgraph, else false.
   */
  [[nodiscard]] bool is_local(const std::size_t& i_vertex) const {
    ASSERT_DEBUG(i_vertex < size_vertex(), "Local vertex index " + std::to_string(i_vertex) + " not in range [0, "
                                           + std::to_string(i_vertex) + ").");
    return level_set_value.empty() ? true : level_set_value[i_vertex] == 0;
  }

  /**
   * @brief Checks if the parsed graph is the parent of this sub-graph.
   * @param[in] graph_parent The parent graph to check.
   * @return True if the graph is the parent, else false.
   */
  [[nodiscard]] bool is_parent(const Adjacency_Graph& graph_parent) const {
    return std::hash<Adjacency_Graph>{}(graph_parent) == hash_parent;
  }

  /**
   * @brief Get the global index of a local vertex.
   * @param[in] i_vertex The local vertex index.
   * @return The global vertex index.
   */
  [[nodiscard]] inline std::size_t local_global(const std::size_t& i_vertex) const {
    ASSERT_DEBUG(i_vertex < size_vertex(), "Local vertex index " + std::to_string(i_vertex) + " not in range [0, "
                                           + std::to_string(i_vertex) + ").");
    return i_local_global[i_vertex];
  }

  /**
   * @brief Reorders this graph given a new index numbering.
   * @param[in] permutation The permutation vector, mapping new_index = permutation[old_index].
   * @return The old, un-permuted, adjacency graph.
   */
  Adjacency_Subgraph reorder(const std::vector<std::size_t>& permutation);

  /**
   * @brief Changes the number of 'halo' levels/vertices around the primary sub-graph.
   * @param[in] parent_graph The parent graph of this subgraph.
   * @param[in] max_level The new number of levels the subgraph must have.
   * @param[in,out] i_global_local A global to local mapping of all vertices in the parent graph.
   *                               Will be populated if empty.
   */
  void update_levels(const Adjacency_Graph& parent_graph, std::size_t max_level,
                     std::shared_ptr<std::vector<std::size_t> > i_global_local);

  /**
   * @brief Returns the level of a given vertex in the graph.
   * @param[in] i_vertex The local index of the vertex whose level is to be returned.
   * @return The level of the vertex at the specified local index.
   */
  [[nodiscard]] inline std::size_t vertex_level(const std::size_t& i_vertex) const {
    ASSERT_DEBUG(i_vertex < size_vertex(), "Local vertex index " + std::to_string(i_vertex) + " not in range [0, "
                                           + std::to_string(i_vertex) + ").");
    return level_set_value.empty() ? 0 : level_set_value[i_vertex];
  }

private:
  std::size_t hash_parent{std::hash<Adjacency_Graph>{}(Adjacency_Graph())};    /**< The hash of a the parent graph, defaults to empty parent. */
  Adjacency_Graph graph;                     /**< This graph's connectivity structure */
  std::vector<std::size_t> i_local_global;   /**< For each local vertex, its global index in the parent graph. */
  std::vector<std::size_t> level_set_value;  /**< For each local vertex, its level traversal value in this subgraph - 0 indicates the primary partition. */


  /**
   * @breief Adds further levels to the subgraph, using the parent graph.
   * @param[in] parent_graph The parent graph
   * @param[in] max_level The new number of levels the sub graph must contain.
   * @param[in] current_max The current number of levels the subgraph contains.
   * @param[in] i_global_local Global to local vertex index map.
   */
  void add_levels(const Adjacency_Graph& parent_graph, std::size_t max_level, std::size_t current_max,
                  std::shared_ptr<std::vector<std::size_t> > i_global_local);

  /**
   * @breif Removes levels from the subgraph.
   * @param[in] max_level The new number of levels in the subgraph.
   */
  void remove_levels(std::size_t max_level);
};

//----------------------------------------------------------------------------------------------------------------------
// Operator Overloading
//----------------------------------------------------------------------------------------------------------------------
// Stream Operators
//----------------------------------------------------------------------------------------------------------------------

/**
 * @brief Overloaded stream operator for the out stream, writes out a formatted graph.
 * @param[in] ostream The out stream to write to.
 * @param[in] graph The graph to write.
 * @return Returns the ostream, with the graph writen out.
 */
inline std::ostream& operator<<(std::ostream& ostream, const Adjacency_Subgraph& graph) {

  FOR(i_vertex, graph.size_vertex()) {
    if(i_vertex != 0) ostream<<"\n";
    FOR_EACH(adjacent_vertex, graph[i_vertex])
      ostream<<adjacent_vertex<<(adjacent_vertex != graph[i_vertex].back() ? ", " : "");
    if(graph[i_vertex].empty()) ostream<<".";
  }

  FOR(i_vertex, graph.size_vertex()) {
    ostream<<"\n"<<i_vertex<<" -> "<<graph.local_global(i_vertex)<<" ("<<graph.vertex_level(i_vertex)<<")";
  }

  return ostream;
}

}

#endif //DISA_ADJACENCY_SUB_GRAPH_H
