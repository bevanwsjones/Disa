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

#include "adjacency_subgraph.h"
#include "graph_utilities.h"

#include <deque>
#include <numeric>
#include <queue>

namespace Disa {

Adjacency_Subgraph::Adjacency_Subgraph(const Adjacency_Graph& graph, const std::vector<std::size_t>& i_sub_graph_vertex,
                                       std::size_t extra_levels) {

  if(extra_levels != 0) {
    std::vector<std::size_t> vertex_level(graph.size_vertex(), std::numeric_limits<std::size_t>::max());
    std::queue<std::size_t> vertex_queue(std::deque(i_sub_graph_vertex.begin(), i_sub_graph_vertex.end()));
    FOR_EACH(global_vertex, vertex_level) vertex_level[global_vertex] = 0;
    level_traversal(graph, vertex_queue, vertex_level, extra_levels);
  }

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


}