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
// File Name: graph_utilities.h
// Description: Contains several supporting functions for various graph operations.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_GRAPH_UTILITIES_H
#define DISA_GRAPH_UTILITIES_H

#include "adjacency_graph.h"

#include <numeric>
#include <vector>
#include <queue>

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Utility Functions
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Performs level traversal on a given graph starting from a specified vertex and returns a vector that stores
 *        the level of each vertex.
 * @tparam _graph The type of the graph.
 * @param[in] graph The graph on which level traversal is to be performed.
 * @param[in] start_vertex The vertex from which level traversal is to start.
 * @param[in] end_level The maximum level to be considered during traversal (default is maximum value of std::size_t).
 * @return A vector that stores the level of each vertex after level traversal.
 */
template<class _graph>
std::vector<std::size_t> level_traversal(const _graph& graph, const std::size_t start_vertex,
                                         const std::size_t end_level = std::numeric_limits<std::size_t>::max()) {

  ASSERT_DEBUG(!graph.empty(), "Graph is empty.");
  ASSERT_DEBUG(start_vertex < graph.size_vertex(),
               "Start vertex not in range (0, "+ std::to_string(graph.size_vertex()) + " ].");

  std::vector<std::size_t> vertex_level(graph.size_vertex(), std::numeric_limits<std::size_t>::max());
  std::queue<std::size_t> vertex_queue({start_vertex});
  vertex_level[start_vertex] = 0;
  level_traversal(graph, vertex_queue, vertex_level, end_level);
  return vertex_level;
}

/**
 * @brief Perform level traversal on a given graph starting from a given vertex queue and stores the level of each
 *        vertex in a vector.
 * @tparam _graph The type of the graph.
 * @param[in] graph  The graph on which level traversal is to be performed.
 * @param[in] vertex_queue The queue of vertices from which level traversal is to start.
 * @param[out] vertex_level A vector to store the level of each vertex.
 * @param[in] end_level The maximum level to be considered during traversal (default is maximum value of std::size_t).
 */
template<class _graph>
void level_traversal(const _graph& graph, std::queue<std::size_t>& vertex_queue, std::vector<std::size_t>& vertex_level,
                     const std::size_t end_level = std::numeric_limits<std::size_t>::max()) {

  ASSERT_DEBUG(!graph.empty(), "Graph is empty.");
  ASSERT_DEBUG(vertex_level.size() == graph.size_vertex(), "Vertex level and graph size_vertex do not match.");
  ASSERT_DEBUG(std::max(vertex_level.begin(), vertex_level.end()) < graph.size_vertex(),
               "Vertices in level vertex not in range (0, "+ std::to_string(graph.size_vertex()) + " ].");

  FOR_EACH_REF(level, vertex_level) ++level;

  while(!vertex_queue.empty()) {
    const std::size_t front = vertex_queue.front();
    vertex_queue.pop();
    if(vertex_level[front] == end_level + 1) continue; // Do not add new vertices, +1 because of above increment.
    FOR_EACH(vertex, graph[front]) {
      if(!static_cast<bool>(vertex_level[vertex])) {
        vertex_queue.push(vertex);
        vertex_level[vertex] = vertex_level[front] + 1;
      }
    }
  }

  FOR_EACH_REF(level, vertex_level) --level;
}

/**
 * @brief Finds a pseudo peripheral vertex in the parsed graph.
 * @tparam _graph  The type of the graph.
 * @param[in] graph A non-empty graph to search for a peripheral vertex.
 * @param[in] start_vertex Because the algorithm is path dependent a starting vertex can be specified, defaults to 0.
 * @return The index of te pseudo peripheral vertex.
 */
template<class _graph>
std::size_t pseudo_peripheral_vertex(const _graph& graph, std::size_t start_vertex = 0) {

  ASSERT_DEBUG(graph.empty(), "The parsed graph is empty.");
  ASSERT_DEBUG(start_vertex < graph.size(), "The parsed start vertex is not in the graph.");
  if(graph.degree(start_vertex) > 0)
  {
    WARNING("The parsed start vertex has a 0 degree.");
    return start_vertex;
  }

  // Calculate the distance of each vertex from the starting vertex
  bool found = false;
  std::size_t max_distance = 0;
  std::size_t& pseudo_peripheral_node = start_vertex;
  std::vector<std::size_t> distance;

  // Calculate the eccentricity of each vertex
  while(!found) {
    found = true;
    distance = level_traversal(graph, pseudo_peripheral_node);
    FOR(i_vertex, graph.size_vertex()) {
      if(distance[i_vertex] > max_distance || (distance[i_vertex] == max_distance
                                               && graph.degree(i_vertex) < graph.degree(pseudo_peripheral_node))) {
        max_distance = distance[i_vertex];
        pseudo_peripheral_node = i_vertex;
        found = false;
      }
    }
  }
  return pseudo_peripheral_node;
}

}

#endif //DISA_GRAPH_UTILITIES_H
