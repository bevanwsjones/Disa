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
//  File Name: reorder.h
//  Description: Definitions for the various reordering algorithms available in Disa.
// ----------------------------------------------------------------------------------------------------------------------
//

#include "macros.h"
#include "reorder.h"

#include <algorithm>
#include <numeric>
#include <map>
#include <queue>

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Level-Set Orderings
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details The implementation of this breadth first search (BFS) follows pretty standard queue based approach which is
 * readily searchable online (two links below). The algorithm reordered the graph through an 'advancing front' or
 * 'level-set' of unvisited vertices which are adjacent to those already visited. Each iteration the front of the
 * visited vertex queue is added to the reordered vertex mapping vector before it popped off the queue. While vertex's
 * unvisited adjacent neighbours are added to the back queue: A 'first-in first-out' queue principle.
 *
 * Perhaps some minor differences to note:
 * 1. We are not doing a search here, all vertices are visited. Hence the removal of the 'search' term in the name.
 * 2. Currently this implementation only supports fully joined graphs.
 *
 * References:
 * https://en.wikipedia.org/wiki/Breadth-first_search
 * https://www.geeksforgeeks.org/breadth-first-search-or-bfs-for-a-graph/
 */
[[nodiscard]] std::vector<std::size_t> breadth_first(const AdjacencyGraph& graph, std::size_t root_vertex) {
  ASSERT_DEBUG(root_vertex < graph.size_vertex(), "New root, " + std::to_string(root_vertex) + " no in graph range [0, "
                                                  + std::to_string(graph.size_vertex()) + ").");

  // Setup
  std::size_t new_index = 0;
  std::queue<std::size_t> queue;
  std::vector<bool> visited(graph.size_vertex(), false);
  std::vector<std::size_t> reorder(graph.size_vertex(), std::numeric_limits<std::size_t>::max());

  // record first vertex
  queue.push(root_vertex);
  visited[root_vertex] = true;

  while(!queue.empty()) {
    // Get the next vertex from the queue
    reorder[queue.front()] = new_index++;
    std::span<const std::size_t> vertex_adjacency = graph[queue.front()];
    queue.pop();

    // Loop over neighbours and push unvisited vertices onto the queue.
    FOR_EACH(vertex, vertex_adjacency) {
      if(!visited[vertex]) {
        visited[vertex] = true;
        queue.push(vertex);
      }
    }
  }
  ASSERT(new_index == graph.size_vertex(), "Graph disjointed, queue emptied before all vertices had been visited.");
  return reorder;
}

/**
 * @details Similar to the BFS algorithm this function implements a queue based Cuthill McKee (CMK) method. As with
 * other level-set orderings an 'advancing front' of unvisited vertices are created and updated around the already
 * visited vertices. The CMK method, however, starts by searching for a suitable root vertex (if one is not provided),
 * by looking for a/the vertex with the lowest degree in the graph. Technically, this should be a periphery vertex, but
 * this implementation does not check for this. Similarly, no errors will be thrown should the user parse a vertex index
 * that is neither the lowest degree nor that is non-periphery. Finally the queue itself is added to in a manner that
 * 'new vertices' are sorted from lowest to highest degree before being placed in the queue.
 *
 * References:
 * https://en.wikipedia.org/wiki/Cuthill%E2%80%93McKee_algorithm
 */
std::vector<std::size_t> cuthill_mckee(const AdjacencyGraph& graph, std::size_t root_vertex) {
  ASSERT_DEBUG(root_vertex < graph.size_vertex() or root_vertex == std::numeric_limits<std::size_t>::max(),
               "New root, " + std::to_string(root_vertex) + " no in graph range [0, "
               + std::to_string(graph.size_vertex()) + ").");

  // Setup
  std::size_t new_index = 0;
  std::queue<std::size_t> queue;
  std::vector<bool> visited(graph.size_vertex(), false);
  std::vector<std::size_t> reorder(graph.size_vertex(), std::numeric_limits<std::size_t>::max());

  // Find first vertex if required.
  if(root_vertex == std::numeric_limits<std::size_t>::max()) {
    std::size_t minimum_degree = std::numeric_limits<std::size_t>::max();
    FOR(i_vertex, graph.size_vertex())
      if(minimum_degree > graph.degree(i_vertex)) {
        root_vertex = i_vertex;
        minimum_degree = graph.degree(i_vertex);
        ASSERT_DEBUG(static_cast<bool>(minimum_degree), "Graph is disjoint, vertex with zero degree found.");
      }
    ASSERT(root_vertex != std::numeric_limits<std::size_t>::max(), "Could not find a vertex with a minimum degree.");
  }
  queue.push(root_vertex);
  visited[root_vertex] = true;

  while(!queue.empty()) {
    // Get the next vertex from the queue
    reorder[queue.front()] = new_index++;
    std::span<const std::size_t> vertex_adjacency = graph[queue.front()];
    queue.pop();

    // Loop over neighbours and push unvisited vertices onto the queue.
    std::vector<std::pair<std::size_t, std::size_t>> vertex_adjacent_degree;
    FOR_EACH(vertex, vertex_adjacency) {
      if(!visited[vertex]) {
        visited[vertex] = true;
        vertex_adjacent_degree.emplace_back(vertex, graph.degree(vertex));
      }
    }

    // Sort and add to the queue.
    if(!vertex_adjacent_degree.empty()) {
      std::sort(vertex_adjacent_degree.begin(), vertex_adjacent_degree.end(),
                [](const auto& pair_0, const auto& pair_1) { return pair_0.second < pair_1.second;});
      FOR_EACH(mappp, vertex_adjacent_degree) queue.push(mappp.first);
    }
  }
  ASSERT(new_index == graph.size_vertex(), "Graph disjointed, queue emptied before all vertices had been visited.");
  return reorder;
}

}
