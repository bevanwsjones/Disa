// ----------------------------------------------------------------------------------------------------------------------
//  MIT License
//  Copyright (c) 2022 Bevan W.S. Jones
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
//  documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
//  permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
//  Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
//  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----------------------------------------------------------------------------------------------------------------------
//  File Name:
//  Description:
// ----------------------------------------------------------------------------------------------------------------------

#ifndef DISA_ADJACENCY_SUB_GRAPH_H
#define DISA_ADJACENCY_SUB_GRAPH_H

#include "adjacency_graph.h"

#include <vector>
#include <unordered_set>

namespace Disa {

class Adjacency_Subgraph : public Adjacency_Graph {

public:

  /**
   * @brief Constructor
   * @param graph
   * @param i_sub_graph_vertex
   * @param extra_levels
   *
   * // todo - i_global_local should make sparse vector
   */
  Adjacency_Subgraph(const Adjacency_Graph& graph, const std::vector<std::size_t>& i_sub_graph_vertex,
                     std::size_t extra_levels = 0) {

    std::vector<std::size_t> i_global_local(graph.size_vertex(), std::numeric_limits<std::size_t>::max());
    reserve(i_sub_graph_vertex.size());
    i_local_global.reserve(i_sub_graph_vertex.size());
    level_set_value.reserve(i_sub_graph_vertex.size());

    // Copy the contents of the graph into the sub-graph and set up axillary containers.
    offset.push_back(0);
    std::size_t i_local_vertex = 0;
    FOR_EACH(i_global_vertex, i_sub_graph_vertex) {
      std::copy(graph[i_global_vertex].begin(), graph[i_global_vertex].end(), std::back_inserter(vertex_adjacent_list));
      offset.push_back(offset[i_local_vertex] + graph[i_global_vertex].size());
      i_local_global.push_back(i_global_vertex);
      level_set_value.push_back(0);
      i_global_local[i_global_vertex] = i_local_vertex;
      ++i_local_vertex;
    }

    // Relabel sub-graph.
    FOR_EACH_REF(vertex, vertex_adjacent_list)
      if(i_global_local[vertex] != std::numeric_limits<std::size_t>::max())
        vertex = i_global_local[vertex];

    // Remove vertices not in this sub graph.
    std::size_t removed = 0;
    auto is_vertex_in_subgraph = [&](const std::size_t& vertex){return vertex >= size_vertex();};
    FOR(i_vertex, size_vertex()) {
      const auto& adjacency = vertex_adjacency_iter(i_vertex);
      offset[i_vertex] -= removed;
      removed += std::count_if(adjacency.first, adjacency.second, is_vertex_in_subgraph);
    }
    offset.back() -= removed;
    vertex_adjacent_list.erase(std::remove_if(vertex_adjacent_list.begin(), vertex_adjacent_list.end(),
                                              is_vertex_in_subgraph), vertex_adjacent_list.end());
    ASSERT(offset.back() == vertex_adjacent_list.size(),
           "Total offsets no longer match vertex size while reducing to subgraph," + std::to_string(offset.back())
           + " vs. " + std::to_string(vertex_adjacent_list.size()) + ".");
  };


  //--------------------------------------------------------------------------------------------------------------------
  // Modifiers
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Clears the contents of the graph, removing all vertices and edges.
   */
  void clear()  {
    vertex_adjacent_list.clear();
    offset.clear();
    i_local_global.clear();
    level_set_value.clear();
  }

//  /**
//   * @brief Inserts an edge into the graph, if the edges exists nothing occurs.
//   * @param[in] edge The edge to insert.
//   * @return true if an edge was inserted, else false.
//   */
//  bool insert(const Edge& edge);
//
//  /**
//   * @brief Resizes the number of vertices in the graph.
//   * @size[in] The number of vertices to resize the graph to.
//   *
//   * @note Edges are removed if the graph size is reduced.
//   */
//  void resize(const std::size_t& size);
//
//  /**
//   * @brief Swaps the contents of the graph with the parsed graph
//   * @param[in,out] other The other graph, this graph will obtain the other's vertices and edges, and visa versa.
//   */
//  inline void swap(Adjacency_Graph& graph_other) {
//    vertex_adjacent_list.swap(graph_other.vertex_adjacent_list);
//    offset.swap(graph_other.offset);
//  }

  //--------------------------------------------------------------------------------------------------------------------
  // Graph Operators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief todo
   * @param[in] i_vertex
   * @return
   */
  [[nodiscard]] bool is_local(const std::size_t& i_vertex) const {
    ASSERT_DEBUG(i_vertex < size_vertex(), "Local vertex index " + std::to_string(i_vertex) + " not in range [0, "
                                           + std::to_string(i_vertex) + ").");
    return level_set_value[i_vertex] == 0;
  }

  /**
   * @brief todo
   * @param[in] i_vertex
   * @return
   */
  [[nodiscard]] std::size_t local_global(const std::size_t& i_vertex) const {
    ASSERT_DEBUG(i_vertex < size_vertex(), "Local vertex index " + std::to_string(i_vertex) + " not in range [0, "
                                           + std::to_string(i_vertex) + ").");
    return i_local_global[i_vertex];
  }

  /**
   *
   * @param i_vertex
   * @return
   */
  [[nodiscard]] std::size_t vertex_level(const std::size_t& i_vertex) const {
    ASSERT_DEBUG(i_vertex < size_vertex(), "Local vertex index " + std::to_string(i_vertex) + " not in range [0, "
                                           + std::to_string(i_vertex) + ").");
    return level_set_value[i_vertex];
  }

private:
  std::vector<std::size_t> i_local_global;
  std::vector<std::size_t> level_set_value;

  //--------------------------------------------------------------------------------------------------------------------
  // Overrides
  //--------------------------------------------------------------------------------------------------------------------

  using Adjacency_Graph::clear; // hide clear -> we are overriding here.
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
    FOR_EACH(adjacent_vertex, graph[i_vertex])ostream<<adjacent_vertex
                                                     <<(adjacent_vertex != graph[i_vertex].back() ? ", " : "");

    if(graph[i_vertex].empty()) ostream<<".";
  }

  FOR(i_vertex, graph.size_vertex()) {
    ostream<<"\n"<<i_vertex<<" -> "<<graph.local_global(i_vertex)<<" ("<<graph.vertex_level(i_vertex)<<")";
  }

  return ostream;
}

}

#endif //DISA_ADJACENCY_SUB_GRAPH_H
