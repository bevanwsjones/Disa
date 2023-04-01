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
// File Name: adjacency_subgraph.cpp
// Description: Declarations of the adjacency subgraph class.
// ---------------------------------------------------------------------------------------------------------------------

#include "adjacency_subgraph.h"
#include "graph_utilities.h"

#include <numeric>
#include <queue>

namespace Disa {

/**
 * @details
 */
Adjacency_Subgraph::Adjacency_Subgraph(Adjacency_Graph& parent_graph,
                                       const std::vector<std::size_t>& i_sub_graph_vertex,
                                       std::size_t extra_levels) {

  // TODO: there is a pointer error here, probably the deque?
//  if(extra_levels != 0) {
//    std::vector<std::size_t> vertex_level(graph.size_vertex(), std::numeric_limits<std::size_t>::max());
//    std::queue<std::size_t> vertex_queue(std::deque(i_sub_graph_vertex.begin(), i_sub_graph_vertex.end()));
//    FOR_EACH(global_vertex, vertex_level) vertex_level[global_vertex] = 0;
//    level_traversal(graph, vertex_queue, vertex_level, extra_levels);
//  }
#ifdef DISA_DEBUG
  bool is_in_range = std::any_of(i_sub_graph_vertex.begin(), i_sub_graph_vertex.end(),
                [&](const auto& iter){return iter < parent_graph.size_vertex();});
  ASSERT_DEBUG(is_in_range, "Global vertex is not in parent graph range [0, "
                            + std::to_string(parent_graph.size_vertex()) + ").");
#endif

  hash_parent = std::hash<Adjacency_Graph>{}(parent_graph);
  graph = parent_graph;
  reserve(i_sub_graph_vertex.size());

  // Copy the contents of the graph into the sub-graph and set up axillary containers.
  std::size_t i_local_vertex = 0;
  std::vector<std::size_t> i_global_local(parent_graph.size_vertex(), std::numeric_limits<std::size_t>::max());
  FOR_EACH(i_global_vertex, i_sub_graph_vertex) {
    i_local_global.push_back(i_global_vertex);
    i_global_local[i_global_vertex] = i_local_vertex;
    ++i_local_vertex;
  }

  // Remove vertices not in this sub grap h.
  auto not_in_subgraph =
    [&](const std::size_t& vertex){return i_global_local[vertex] == std::numeric_limits<std::size_t>::max();};
  graph.erase_if(not_in_subgraph);

  // Add back additional levels as needed.
  if(extra_levels != 0)
    update_levels(parent_graph, extra_levels, std::make_shared<std::vector<std::size_t> >(i_global_local));
};

//--------------------------------------------------------------------------------------------------------------------
// Graph Operators
//--------------------------------------------------------------------------------------------------------------------

/**
 * @details
 */
Adjacency_Subgraph Adjacency_Subgraph::reorder(const std::vector<std::size_t>& permutation) {
  Adjacency_Subgraph new_graph;
  new_graph.graph = graph.reorder(permutation);
  new_graph.i_local_global.resize(permutation.size());
  new_graph.level_set_value.resize(level_set_value.empty() ? 0 : permutation.size());

  FOR(i_old, size_vertex()) {
  new_graph.i_local_global[permutation[i_old]] = i_local_global[i_old];
  if(!level_set_value.empty()) new_graph.level_set_value[permutation[i_old]] = level_set_value[i_old];
  }

  new_graph.i_local_global.swap(i_local_global);
  new_graph.level_set_value.swap(level_set_value);
  return new_graph;
}

/**
 * @details
 */
void Adjacency_Subgraph::update_levels(const Adjacency_Graph& parent_graph, std::size_t& max_level,
                                       std::shared_ptr<std::vector<std::size_t> > i_global_local) {

  ASSERT(hash_parent == std::hash<Adjacency_Graph>{}(parent_graph), "");

  // Check the global-local vector
  if(i_global_local == nullptr) i_global_local =
    std::make_shared<std::vector<std::size_t> > (size_vertex(), std::numeric_limits<std::size_t>::max());

  // Determine if we are adding or removing levels
  std::size_t current_max = 0;
  if(level_set_value.empty()) level_set_value.resize(size_vertex(), 0);
  else current_max = *std::max_element(level_set_value.begin(), level_set_value.end());

  if(current_max < max_level) add_levels(parent_graph, max_level, current_max, i_global_local);
  else remove_levels(max_level);
}

/**
 * @details
 */
void Adjacency_Subgraph::add_levels(const Adjacency_Graph& parent_graph, std::size_t& max_level,
                                    std::size_t& current_max,
                                    std::shared_ptr<std::vector<std::size_t> > i_global_local) {
  // Create data for level traversal.
  std::queue<std::size_t> start_vertices;
  std::vector<std::size_t> levels(parent_graph.size_vertex(), std::numeric_limits<std::size_t>::max());
  FOR(i_vertex, size_vertex()) {
    if(level_set_value[i_vertex] == current_max) start_vertices.push(i_vertex);
    levels[i_local_global[i_vertex]] = level_set_value[i_vertex];
    (*i_global_local)[i_local_global[i_vertex]] = i_vertex;
  }

  // Perform level traversal to add new vertices.
  level_traversal(parent_graph, start_vertices, levels, max_level);

  // Add new vertices to local-global mapping.
  FOR(i_vertex, levels.size())
    if(current_max < level_set_value[i_vertex] <= max_level) {
      (*i_global_local)[i_local_global[i_vertex]] = i_local_global.size();
      i_local_global.push_back(i_vertex);
    }

  // Inert
  FOR(i_vertex, levels.size()) {
    if(current_max < level_set_value[i_vertex] <= max_level) {
      FOR_EACH(i_adjacency, parent_graph[i_vertex]) {
        if(i_vertex < i_adjacency && level_set_value[i_adjacency] != std::numeric_limits<std::size_t>::max()) {
          ASSERT_DEBUG((*i_global_local)[i_local_global[i_vertex]] != std::numeric_limits<std::size_t>::max(),
                       "Failed sanity check, vertex has a level number but has no global-local mapping.");
          graph.insert({i_vertex, i_adjacency});
        }
      }
    }
  }
}

/**
 *
 */
void Adjacency_Subgraph::remove_levels(std::size_t& max_level) {
  auto not_in_subgraph = [&](const std::size_t& vertex){return level_set_value[vertex] > max_level;};
  graph.erase_if(not_in_subgraph);

  std::size_t i_vertex = 0;
  i_local_global.erase(std::remove_if(i_local_global.begin(), i_local_global.end(),
                                      [&](const auto){return not_in_subgraph(i_vertex++);}), i_local_global.end());

  level_set_value.erase(std::remove_if(level_set_value.begin(), level_set_value.end(),
                                       [&](const std::size_t level){return level > max_level;}), level_set_value.end());


}

}