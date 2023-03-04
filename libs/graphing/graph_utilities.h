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
#include "adjacency_subgraph.h"

#include <numeric>
#include <vector>
#include <queue>
#include <cmath>

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
               "Start vertex not in range (0, "+ std::to_string(graph.size_vertex()) + "].");

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

  // roll the vector over, before checking max.
  FOR_EACH_REF(level, vertex_level) ++level;
  ASSERT_DEBUG(*std::max_element(vertex_level.begin(), vertex_level.end()) < graph.size_vertex(),
               "A vertex in vertex level not in graph range (0, "+ std::to_string(graph.size_vertex()) + "].");

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
 * @brief Applies a level-set expansion to color a parsed graph from a set of seed vertices, where the color of each
 *        vertex is associated with the 'closest' seed vertex.
 * @tparam _graph The type of the graph.
 * @param[in] graph  The graph on which level expansion is to be performed.
 * @param[in] i_seeds Indices of seed vertices from which expansion will occur (No. colors = size of this vector).
 * @param[out] vertex_color Of graph vertex size, and containing the color of each vertex after the level expansion.
 */
template<class _graph>
void level_expansion(const _graph& graph, const std::vector<std::size_t>& seeds,
                     std::vector<std::size_t>& vertex_color) {
  ASSERT_DEBUG(!graph.empty(), "Graph is empty.");
  ASSERT_DEBUG(!seeds.empty(), "There are no seeds to begin expansion.");
  ASSERT_DEBUG(*std::max_element(seeds.begin(), seeds.end()) < graph.size_vertex(),
               "A seed index is no in graph range [0, " + std::to_string(graph.size_vertex()) + ").");

  // Setup memory, and seed the queues and colors.
  vertex_color.resize(graph.size_vertex());
  std::vector<std::queue<std::size_t> > vertex_queues(seeds.size());
  FOR_EACH_REF(color, vertex_color) color = std::numeric_limits<std::size_t>::max();
  FOR(i_seed, seeds.size()) {
    vertex_queues[i_seed].push({seeds[i_seed]});
    vertex_color[seeds[i_seed]] = i_seed;
  }

  // Color vertices.
  std::size_t iteration = 0;
  while(std::any_of(vertex_queues.begin(), vertex_queues.end(), [](const auto& queue){return !queue.empty();})) {
    FOR(i_queue, vertex_queues.size()) {
      // ensures we do a forward's and backwards sweep to try and keep expansion 'unbiased'.
      auto& vertex_queue = iteration%2 == 0 ? vertex_queues[i_queue]
                                            : vertex_queues[vertex_queues.size() - i_queue - 1];

      // Perform a level expansion for this color.
      std::queue<std::size_t> new_queue;
      while(!vertex_queue.empty()) {
        const std::size_t front = vertex_queue.front();
        vertex_queue.pop();
        FOR_EACH(vertex, graph[front]) {
          if(vertex_color[vertex] == std::numeric_limits<std::size_t>::max()) {
            new_queue.push(vertex);
            vertex_color[vertex] = vertex_color[front];
          }
        }
      }
      std::swap(new_queue, vertex_queue);
    }
    ASSERT(iteration++ < graph.size_vertex(), "Number of iterations have exceeded, "
                                               + std::to_string(iteration) + ". Is the graph disjoint?");
  }
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

  ASSERT_DEBUG(!graph.empty(), "The parsed graph is empty.");
  ASSERT_DEBUG(start_vertex < graph.size_vertex(), "The parsed start vertex is not in the graph.");
  if(graph.degree(start_vertex) == 0)
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

/**
 *
 * @tparam _graph
 * @return
 */
template<class _graph>
std::size_t eigen_vector_centrality(_graph graph){
  std::vector<std::size_t> centrality_0(graph.size_vertex(), 0);
  std::vector<std::size_t> centrality_1(graph.size_vertex(), 0);

  std::size_t max_iter = std::ceil(level_traversal(graph, pseudo_peripheral_vertex(graph)).back()/2);

  FOR(i_vertex, graph.size_vertex()) centrality_0[i_vertex] = graph[i_vertex].size();

  FOR(iter, max_iter) {
    FOR(i_vertex, graph.size_vertex()){
      centrality_1[i_vertex] = centrality_0[i_vertex];
      FOR_EACH(i_adjacent, graph[i_vertex]){
          centrality_1[i_vertex] += centrality_0[i_adjacent];
        }
    }
    std::swap(centrality_0, centrality_1);
  }

  return *std::max_element(centrality_0.begin(), centrality_0.end()); //todo - what happens where there is more than one max?
}


}

#endif //DISA_GRAPH_UTILITIES_H
