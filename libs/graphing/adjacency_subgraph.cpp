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
 * @details Constructs a new subgraph by partitioning the vertices of the given parent graph, using the indices
 * provided in `i_partition_local_global`. The resulting subgraph includes only the vertices in the partition, and
 * is indexed starting from 0, with no gaps in the indices. The parameter `extra_levels` indicates the number of levels
 * to be added to the graph after the partition. The function also initializes auxiliary data structures used in the
 * implementation of the partition, including `i_local_global` and `i_global_local`, and removes vertices that are not
 * included in the subgraph. Finally, if extra levels are added, the function calls `update_levels` to update the
 * levels of the subgraph. If DISA_DEBUG is defined, the function performs several assertions to ensure that the input
 * data is valid.
 */
Adjacency_Subgraph::Adjacency_Subgraph(Adjacency_Graph& parent_graph,
                                       const std::vector<std::size_t>& i_partition_local_global,
                                       const std::size_t extra_levels) {
#ifdef DISA_DEBUG
  std::vector<std::size_t> unique_check;
  std::unique_copy(i_partition_local_global.begin(), i_partition_local_global.end(), std::back_inserter(unique_check));
  ASSERT(unique_check.size() == i_partition_local_global.size(), "Partition vertices are not unique.");
  ASSERT(i_partition_local_global.size() <= parent_graph.size_vertex(),
         "Partition size is bigger than graph vertex size.");
  ASSERT(!std::any_of(i_partition_local_global.begin(), i_partition_local_global.end(),
                            [&](const std::size_t i_global){return i_global >= parent_graph.size_vertex();}),
               "A global vertex is not in the parsed parent graph range [0, "
               + std::to_string(parent_graph.size_vertex()) + ").");
#endif

  hash_parent = std::hash<Adjacency_Graph>{}(parent_graph);
  graph = parent_graph;
  reserve(i_partition_local_global.size());

  // Copy the contents of the graph into the sub-graph and set up axillary containers.
  std::size_t i_local_vertex = 0;
  std::vector<std::size_t> i_global_local(parent_graph.size_vertex(), std::numeric_limits<std::size_t>::max());
  FOR_EACH(i_global_vertex, i_partition_local_global) {
    i_local_global.push_back(i_global_vertex);
    i_global_local[i_global_vertex] = i_local_vertex;
    ++i_local_vertex;
  }

  // Remove vertices not in this subgraph.
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
 * @details This function creates a new `Adjacency_Subgraph` instance that is a reordered version of the current
 * instance. The new `Adjacency_Subgraph` contains the same vertices and edges as the original, but with their order
 * permuted based on the given permutation vector. The `i_local_global` and `level_set_value` vectors of the new
 * `Adjacency_Subgraph` instance are also updated to reflect the permutation. The old `i_local_global` and
 * `level_set_value` vectors are swapped with the corresponding vectors of the new instance, which makes the old
 * instance empty and the new instance contain all the data.
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
 * @details This function updates the `level_set_value` vector of the `Adjacency_Subgraph` instance by adding or
 * removing levels as necessary to achieve a maximum level of `max_level`. If the current maximum level is less than
 * `max_level`, this function adds levels to the subgraph by updating `level_set_value` for all vertices that are
 * connected to vertices in the subgraph at the previous maximum level. If the current maximum level is greater than
 * `max_level`, this function removes levels from the subgraph by updating `level_set_value` for all vertices in the
 * subgraph at levels greater than `max_level`. The `i_global_local` vector is an optional shared pointer to a vector
 * that maps global vertex indices to local vertex indices in the subgraph.
 */
void Adjacency_Subgraph::update_levels(const Adjacency_Graph& parent_graph, const std::size_t max_level,
                                       std::shared_ptr<std::vector<std::size_t> > i_global_local) {

  ASSERT(hash_parent == std::hash<Adjacency_Graph>{}(parent_graph), "Parsed graph is not the parent to this subgraph.");

  // Determine if we are adding or removing levels
  std::size_t current_max = 0;
  if(level_set_value.empty()) level_set_value.resize(size_vertex(), 0);
  else current_max = *std::max_element(level_set_value.begin(), level_set_value.end());

  if(current_max < max_level) add_levels(parent_graph, max_level, current_max, i_global_local);
  else remove_levels(parent_graph, max_level, i_global_local);
}

// ---------------------------------------------------------------------------------------------------------------------
// Helper Functions
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details This function adds new levels to the `Adjacency_Subgraph` instance based on the given `max_level`.
 * It uses a level traversal algorithm to traverse the parent graph from the vertices of the current instance
 * that belong to the previous maximum level, and adds all newly visited vertices to the subgraph. The `i_global_local`
 * vector is updated to reflect the new subgraph, and edge connectivity between the newly added vertices is inserted
 * into the adjacency graph. This function assumes that the `hash_parent` value of the current subgraph matches the
 * hash value of the given `parent_graph`.
 */
void Adjacency_Subgraph::add_levels(const Adjacency_Graph& parent_graph, const std::size_t max_level,
                                    const std::size_t current_max,
                                    std::shared_ptr<std::vector<std::size_t> > i_global_local) {

  // Check the global-local vector
  if(i_global_local == nullptr) i_global_local =
    std::make_shared<std::vector<std::size_t> > (parent_graph.size_vertex(), std::numeric_limits<std::size_t>::max());
  if(i_global_local->size() != parent_graph.size_vertex()) i_global_local->resize(parent_graph.size_vertex());
  std::fill(i_global_local->begin(), i_global_local->end(), std::numeric_limits<std::size_t>::max());

  // Create data for level traversal.
  std::queue<std::size_t> start_vertices;
  std::vector<std::size_t> levels(parent_graph.size_vertex(), std::numeric_limits<std::size_t>::max());
  FOR(i_vertex, size_vertex()) {
    if(level_set_value[i_vertex] == current_max) start_vertices.push(i_local_global[i_vertex]);
    levels[i_local_global[i_vertex]] = level_set_value[i_vertex];
    (*i_global_local)[i_local_global[i_vertex]] = i_vertex;
  }

  // Perform level traversal to add new vertices.
  level_traversal(parent_graph, start_vertices, levels, max_level);

  // Add new vertices to local-global mapping.
  FOR(i_vertex, levels.size())
    if(current_max < levels[i_vertex] && levels[i_vertex] <= max_level) {
      (*i_global_local)[i_vertex] = i_local_global.size();
      i_local_global.push_back(i_vertex);
      level_set_value.push_back(levels[i_vertex]);
    }

  // Insert edge connectivity between newly added vertices.
  FOR(i_global, levels.size()) {
    if(current_max < levels[i_global] && levels[i_global] <= max_level) {
      const std::size_t& i_local = (*i_global_local)[i_global];
      FOR_EACH(i_global_adjacency, parent_graph[i_global]) {
        const std::size_t& i_local_adjacency = (*i_global_local)[i_global_adjacency];
        if(i_local_adjacency != std::numeric_limits<std::size_t>::max()) {
          graph.insert({i_local, i_local_adjacency});
        }
      }
    }
  }
}

/**
 * @details The graph's adjacency list is updated by removing any edges that are not within the subgraph after the
 * levels have been updated. The local-global index mapping is also updated by removing any vertices that are no longer
 * present in the subgraph. If a shared pointer to a global-local index mapping is provided, it is updated to reflect
 * the changes in the local-global mapping caused by the removal of vertices from the subgraph.
 */
void Adjacency_Subgraph::remove_levels(const Adjacency_Graph& parent_graph, const std::size_t max_level,
                                       std::shared_ptr<std::vector<std::size_t> > i_global_local) {
  auto not_in_subgraph = [&](const std::size_t& vertex){return level_set_value[vertex] > max_level;};
  graph.erase_if(not_in_subgraph);

  std::size_t i_vertex = 0;
  i_local_global.erase(std::remove_if(i_local_global.begin(), i_local_global.end(),
                                      [&](const auto){return not_in_subgraph(i_vertex++);}), i_local_global.end());

  if(max_level != 0) {
    level_set_value.erase(std::remove_if(level_set_value.begin(), level_set_value.end(),
                                         [&](const std::size_t level) { return level > max_level; }),
                          level_set_value.end());
  }
  else level_set_value.clear();

  // If we were parsed a global to local pointer, populate with new data.
  if(i_global_local != nullptr) {
    i_global_local->resize(parent_graph.size_vertex());
    std::fill(i_global_local->begin(), i_global_local->end(), std::numeric_limits<std::size_t>::max());
    FOR(i_local, size_vertex()) (*i_global_local)[local_global(i_local)] = i_local;
  }
}

// ---------------------------------------------------------------------------------------------------------------------
// Operator Overloading
// ---------------------------------------------------------------------------------------------------------------------
// Stream Operators
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details The adjacency list of the graph is written to the output stream as follows:
 * For each vertex in the graph, its adjacent vertices are listed separated by commas. If a vertex has no adjacent
 * vertices, it is represented by a period ('.'). After the adjacent vertices of each vertex are listed,
 * the vertex's local index, its corresponding global index, and its level are written in the format
 * "local_index -> global_index (level)".
 *
 * The output format looks like:
 * 0: 1, 2
 * 1: 0
 * 2: 0, 1
 * 0 -> 3 (0)
 * 1 -> 1 (1)
 * 2 -> 2 (1)
 */
std::ostream& operator<<(std::ostream& ostream, const Adjacency_Subgraph& graph) {

  FOR(i_vertex, graph.size_vertex()) {
    if(i_vertex != 0) ostream<<"\n";
    FOR_EACH(adjacent_vertex, graph[i_vertex])
      ostream<<adjacent_vertex<<(adjacent_vertex != graph[i_vertex].back() ? ", " : "");
    if(graph[i_vertex].empty()) ostream<<".";
  }

  FOR(i_vertex, graph.size_vertex())
    ostream<<"\n"<<i_vertex<<" -> "<<graph.local_global(i_vertex)<<" ("<<graph.vertex_level(i_vertex)<<")";

  return ostream;
}

}
