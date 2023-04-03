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
 * @param[in] i_start The vertex from which level traversal is to start.
 * @param[in] end_level The maximum level to be considered during traversal (default is maximum value of std::size_t).
 * @return A vector that stores the level of each vertex after level traversal.
 */
template<class _graph>
std::vector<std::size_t> level_traversal(const _graph& graph, const std::size_t i_start,
                                         const std::size_t end_level = std::numeric_limits<std::size_t>::max()) {

  ASSERT_DEBUG(!graph.empty(), "Graph is empty.");
  ASSERT_DEBUG(i_start < graph.size_vertex(),
               "Starting vertex not in range (0, "+ std::to_string(graph.size_vertex()) + "].");

  std::vector<std::size_t> vertex_level(graph.size_vertex(), std::numeric_limits<std::size_t>::max());
  std::queue<std::size_t> vertex_queue({i_start});
  vertex_level[i_start] = 0;
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
  FOR_EACH_REF(level, vertex_level) ++level;   // roll the vector over, before checking max.
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
               "A seed index is not in graph range [0, " + std::to_string(graph.size_vertex()) + ").");

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
 * @param[in] i_start Because the algorithm is path dependent a starting vertex can be specified, defaults to 0.
 * @return The index of te pseudo peripheral vertex.
 */
template<class _graph>
std::size_t pseudo_peripheral_vertex(const _graph& graph, std::size_t i_start = 0) {

  ASSERT_DEBUG(!graph.empty(), "The parsed graph is empty.");
  ASSERT_DEBUG(i_start < graph.size_vertex(), "The parsed start vertex is not in the graph.");
  if(graph.degree(i_start) == 0)
  {
    WARNING("The parsed start vertex has a 0 degree.");
    return i_start;
  }

  // Calculate the distance of each vertex from the starting vertex
  bool found = false;
  std::size_t max_distance = 0;
  std::size_t& pseudo_peripheral_node = i_start;
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
 * @brief Computes the eccentricity of all vertices in the graph using the breadth-first search algorithm.
 * @tparam _graph Type of the input graph.
 * @param[in] graph A graph containing the vertices for which the eccentricity is to be computed.
 * @param[out] eccentricity Output vector of vectors containing the eccentricity of each vertex in the graph.
 */
template<class _graph>
void eccentricity_graph(const _graph& graph, std::vector<std::vector<std::size_t> >& eccentricity) {
  eccentricity.resize(graph.size_vertex());
    FOR(i_vertex, graph.size_vertex()){
    const std::size_t start_vertex = graph.size_vertex() - i_vertex - 1;
    eccentricity_vertex_breadth_first(graph, start_vertex, eccentricity[start_vertex], start_vertex + 1);
  }
}

/**
 * @brief Perform breadth-first search to compute the eccentricity of each vertex in a graph from a given vertex.
 * @tparam _graph  The type of the graph.
 * @param[in] graph A non-empty graph to search for a peripheral vertex.
 * @param[in]  i_start The index of the starting vertex, i.e. computed distance will be from this vertex to all others.
 * @param[out] distance Vector to store the distances between the starting vertex and each vertex in the graph.
 * @param[in] i_stop The index of a stopping vertex, defaults to max. See details.
 *
 * @details The function uses a queue to store the vertices to be visited and updates the distance vector as it performs
 * a breath first search. If the index of the vertex at the front of the queue is greater than or equal to i_stop it is
 * not visited, this can improve performance when building full eccentricities which are symmetric.
 */
template<class _graph>
void eccentricity_vertex_breadth_first(const _graph& graph, const std::size_t i_start,
                                       std::vector<std::size_t>& distance,
                                       const std::size_t i_stop = std::numeric_limits<std::size_t>::max()) {
  ASSERT_DEBUG(!graph.empty(), "The parsed graph is empty.");
  ASSERT_DEBUG(i_start < graph.size_vertex(), "The parsed start vertex is not in the graph.");
  ASSERT_DEBUG(i_start <= i_stop, "The parsed start vertex is greater than the parsed stop vertex.");
  ASSERT_DEBUG(i_stop == std::numeric_limits<std::size_t>::max() || i_stop <= graph.size_vertex(),
               "The stopping vertex is not in the graph size range [0, " + std::to_string(graph.size_vertex())
               + "] and not set to a default.");

  // Initialise the distance vector.
  std::fill(distance.begin(), distance.end(), std::numeric_limits<std::size_t>::max());
  distance.resize(graph.size_vertex() <= i_stop ? graph.size_vertex() : i_stop,
                  std::numeric_limits<std::size_t>::max());

  // Create a priority queue to store the vertices to be visited
  std::queue<std::size_t> queue({i_start});
  distance[i_start] = 0;

  // Visit each vertex in the graph
  while(!queue.empty()) {
    const std::size_t front = queue.front();
    queue.pop();
    if(front >= i_stop) continue; // we don't search.
    FOR_EACH(vertex, graph[front]) {
      if(distance[vertex] == std::numeric_limits<std::size_t>::max()) {
        distance[vertex] = distance[front] + 1;
        queue.push(vertex);
        }
    }
  }
}

}

#endif //DISA_GRAPH_UTILITIES_H
