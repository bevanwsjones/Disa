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

#ifndef DISA_GRAPH_H
#define DISA_GRAPH_H

#include "macros.h"

#include <algorithm>
#include <span>
#include <utility>
#include <vector>

namespace Disa {

using Edge = std::pair<std::size_t, std::size_t>; // shorthand for an edge

//----------------------------------------------------------------------------------------------------------------------
// AdjacencyGraph
//----------------------------------------------------------------------------------------------------------------------

/**
 * @class AdjacencyGraph
 * @brief
 *
 * convention
 * vertices -
 * edges -
 *
 * i - j connection == j - i connection (i.e. the graph is undirected).
 */
class AdjacencyGraph {

public:

  //--------------------------------------------------------------------------------------------------------------------
  // Public Member Functions
  //--------------------------------------------------------------------------------------------------------------------

  AdjacencyGraph() = default;

  /**
   * @brief Construction via a list of edges connecting two vertices i and j, where i != j.
   * @param[in] edge_graph The list of edge.
   *
   * @note At preset this is not writen efficiently, and can be improved, mainly used for testing.
   */
  AdjacencyGraph(std::initializer_list<Edge> edge_graph) {
    const auto iter = std::max_element(edge_graph.begin(), edge_graph.end(), [](const Edge& edge_0, const Edge& edge_1){
      return std::max(edge_0.first, edge_0.second) < std::max(edge_1.first, edge_1.second);
    });
    offset.reserve(std::max(iter->first, iter->second) + 1);
    vertex_adjacent_list.reserve(edge_graph.size()*2);
    FOR_EACH(edge, edge_graph) insert(edge);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Element Access
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * todo
   */
//  [[nodiscard]] std::span<const std::size_t> & at(const std::size_t& i_row);

  /**
   * todo
   */
//  [[nodiscard]] const std::span<const std::size_t> & at(const std::size_t& i_row) const;

  /**
   * tod
   */
//  std::span<const std::size_t> operator[](const std::size_t& i_row);

  /**
   * todo
   */
  std::span<const std::size_t> operator[](const std::size_t& i_row) const {
    return {vertex_adjacent_list.begin() + offset[i_row], offset[i_row + 1] - offset[i_row]};
  }

  /**
   * todo
   */
  std::pair<std::size_t*, std::size_t*> inline data() noexcept {
    if(!empty() && !vertex_adjacent_list.empty()) return std::make_pair(vertex_adjacent_list.data(), offset.data());
    else if(vertex_adjacent_list.empty()) return std::make_pair(nullptr, offset.data());
    else return std::make_pair(nullptr, nullptr);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Iterators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * todo.
   */
//  iterator begin() noexcept;

  /**
   * todo.
   */
//  [[nodiscard]] const_iterator begin() const noexcept;

  /**
   * todo.
   */
//  [[nodiscard]] const_iterator cbegin() const noexcept;

  /**
   * todo.
   */
//  iterator end() noexcept;

  /**
   * todo.
   */
//  [[nodiscard]] const_iterator end() const noexcept;

  /**
   * todo.
   */
//  [[nodiscard]] const_iterator cend() const noexcept;

  //--------------------------------------------------------------------------------------------------------------------
  // Capacity
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Checks whether the graph is empty. An empty graph is considered where there are no vertices.
   * @return True if the graph is empty, else false.
   */
  [[nodiscard]] inline bool empty() const noexcept { return offset.empty() || offset.size() < 2; }

  /**
   * @brief Returns the number of vertices in the graph.
   * @return The number of vertices.
   */
  [[nodiscard]] std::size_t size_vertices() const {return !offset.empty() ? offset.size() - 1 : 0;}

  /**
   * @brief Returns the number of edge in the graph.
   * @return The number of edge.
   */
  [[nodiscard]] std::size_t size_edge() const {return vertex_adjacent_list.size()/2;}

  /**
   * @brief Returns the number of vertices and edges in the graph.
   * @return Pair containing [vertices, edges].
   */
  [[nodiscard]] std::pair<std::size_t, std::size_t> size() const { 
    return std::make_pair(size_vertices(), size_edge());
  }

  /**
   * @brief Reserves enough memory such that the data structure can be filled with the parsed vertices and/or edges.
   * @param[in] size_vertices The total number of vertices to reserve for.
   * @param[in] size_edge The total number of edge to reserve for (defaults to zero).
   */
  void reserve(std::size_t size_vertices, std::size_t size_edge = 0) {
    offset.reserve(size_vertices + 1);
    vertex_adjacent_list.reserve(size_edge*2);
  }

  /**
   * @brief Returns the current number of vertices and edges the graph has capacity (memory for).
   * @return Pair containing [vertices, edges].
   */
  [[nodiscard]] std::pair<std::size_t, std::size_t> capacity() const {
    return std::make_pair(!offset.capacity() ? 0 : offset.capacity() - 1, vertex_adjacent_list.capacity()/2);
  }

  void shrink_to_fit() {
    vertex_adjacent_list.shrink_to_fit();
    offset.shrink_to_fit();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Modifiers
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * todo
   */
  void clear() {
    vertex_adjacent_list.clear();
    offset.clear();
  }

  /**
   * todo
   */
  bool insert(const Edge& edge);

  /**
   * todo
   */
  void resize(const std::size_t size_vertices, const std::size_t size_edges = 0) {

    // todo if smaller
    offset.resize(size_vertices + 1, offset.empty() ? 0 : offset.back());
  }

  /**
   * todo
   */
  void swap(AdjacencyGraph& graph_other) {
    vertex_adjacent_list.swap(graph_other.vertex_adjacent_list);
    offset.swap(graph_other.offset);
  }


  //--------------------------------------------------------------------------------------------------------------------
  // Lookup
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * todo
   */
  [[nodiscard]] bool contains(const Edge& edge) const {
    // todo i < j, i != j
    if(edge.second >= size_vertices() || edge.first >= size_vertices() || empty()) return false;
    std::span<const std::size_t> adjacency = (*this)[edge.first];
    return std::ranges::any_of(adjacency.begin(), adjacency.end(),
                               [&](const std::size_t& i_vertex){return edge.second == i_vertex;});
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Graph Operators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * todo
   */
  void re_order(const std::vector<std::size_t>& new_order) {
    AdjacencyGraph new_graph;

    // determine new offsets.
    new_graph.offset[0] = 0;
    FOR(i_vertex, new_graph.offset.size()) {
      new_graph.offset[i_vertex + 1] += offset[new_order[i_vertex] + 1] - offset[new_order[i_vertex]];
      new_graph[i_vertex] = (*this)[i_vertex];
      std::ranges::copy_n(&vertex_adjacent_list[new_order[i_vertex + 1]],
                          offset[new_order[i_vertex] + 1] - offset[new_order[i_vertex]],
                          &new_graph.vertex_adjacent_list[i_vertex]);
    }

    this->swap(new_graph);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Private Members
  //--------------------------------------------------------------------------------------------------------------------

private:
  std::vector<std::size_t> vertex_adjacent_list;    //!< todo
  std::vector<std::size_t> offset;                  //!< todo

};

// arithmetic operators

// todo - produces a union
inline AdjacencyGraph operator+(const AdjacencyGraph& graph_0, const AdjacencyGraph& graph_1) {
  return AdjacencyGraph();
};

// todo -  produces the intersection between the graphs
inline AdjacencyGraph operator-(const AdjacencyGraph& graph_0, const AdjacencyGraph& graph_1) {
  return AdjacencyGraph();
};

// todo -  output operateres - test
inline std::ostream& operator<<(std::ostream& ostream, const AdjacencyGraph& graph) {
  FOR(i_vertex, graph.size_vertices()) {
    ostream<<"\n";
    FOR_EACH(adjacent_vertex, graph[i_vertex])
      ostream<<adjacent_vertex<<(adjacent_vertex != graph[i_vertex].back() ? ", ": "");
  }
  return ostream;
}

}

#endif //DISA_GRAPH_H
