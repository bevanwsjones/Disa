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
// File Name:
// Description:
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

template<class _graph>
std::vector<std::size_t> level_traversal(const _graph& graph, const std::size_t start_vertex,
                                         const std::size_t end_level = std::numeric_limits<std::size_t>::max()) {
  std::vector<std::size_t> vertex_level(graph.size_vertex(), std::numeric_limits<std::size_t>::max());
  std::queue<std::size_t> vertex_queue({start_vertex});
  vertex_level[start_vertex] = 0;
  level_traversal(graph, vertex_queue, vertex_level, end_level);
  return vertex_level;
}

template<class _graph>
void level_traversal(const _graph& graph, std::queue<std::size_t>& vertex_queue, std::vector<std::size_t>& vertex_level,
                     const std::size_t end_level = std::numeric_limits<std::size_t>::max()) {

  FOR_EACH_REF(level, vertex_level) ++level;

  while(!vertex_queue.empty()) {
    const std::size_t front = vertex_queue.front();
    vertex_queue.pop();
    FOR_EACH(vertex, graph[front]) {
      if(!static_cast<bool>(vertex_level[vertex])) {
        vertex_queue.push(vertex);
        vertex_level[vertex] = vertex_level[front] + 1;
      }
    }
  }

  FOR_EACH_REF(level, vertex_level) --level;
}

std::size_t pseudo_peripheral_vertex(const Adjacency_Graph& graph,
                                     const std::vector<std::vector<std::size_t> >& adjacency_list,
                                     std::size_t start_vertex) {


  // Calculate the distance of each vertex from the starting vertex
  std::vector<std::size_t> distance = level_traversal(graph, start_vertex);

  // Calculate the eccentricity of each vertex
  std::size_t max_distance = -1;
  std::size_t pseudo_peripheral_node = start_vertex;

  for(std::size_t i = 0; i < graph.size_vertex(); i++) {
    if (distance[i] > max_distance
        || (distance[i] == max_distance && graph.degree(i) > graph.degree(pseudo_peripheral_node))) {
      max_distance = distance[i];
      pseudo_peripheral_node = i;
    }
  }

  return pseudo_peripheral_node;
}


}

#endif //DISA_GRAPH_UTILITIES_H
