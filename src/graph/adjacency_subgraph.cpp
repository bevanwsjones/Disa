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

// ---------------------------------------------------------------------------------------------------------------------
// Adjacency_Subgraph
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details Constructs a new subgraph using the given vertex partitioning of the parent graph. The resulting subgraph
 * will form the primary partition with these vertices. This presently is achieved by copying the parent graph to the
 * internal graph representation, and forming the global-local mappings and deleting vertices not in the subgraph.
 * Finally, should extra levels be requested they are added to the subgraph.
 */
Adjacency_Subgraph::Adjacency_Subgraph(const Adjacency_Graph<false>& parent_graph,
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

  hash_parent = std::hash<Adjacency_Graph<false> >{}(parent_graph);
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
 * @details Creates a new subgraph instance that is a reordered version of the current instance. The new subgraph
 * contains the same vertices and edges as the original, but with their local indexes permuted based on the given
 * permutation vector. Finally the new instance and this instance are swapped, with the now old instance returned.
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
 * @details Updates the levels of the subgraph by either adding or removing levels from the parent graph depending on
 * the current size vs the requested new size of the levels, max_level. Where adding occurs when max level is greater
 * than the current levels, and removing when less than current. The new global to local mapping is returned through the
 * i_global_local shared pointer.
 */
void Adjacency_Subgraph::update_levels(const Adjacency_Graph<false>& parent_graph, const std::size_t max_level,
                                       std::shared_ptr<std::vector<std::size_t> > i_global_local) {

  ASSERT(hash_parent == std::hash<Adjacency_Graph<false> >{}(parent_graph), "Parsed graph is not the parent to this subgraph.");

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
 * @details Adds new levels to the subgraph based on the given max_level. To add additional levels a level traversal of
 * the parent graph is performed. The current subgraph vertices are 'loaded' into a vector of visited vertices and a
 * queue formed from the current maximum level vertex set. This data is then used to start the level traversal upto the
 * maximum requested level. The added vertices are then added into the subgraph, with the local-to-global mappings and
 * level stored.
 */
void Adjacency_Subgraph::add_levels(const Adjacency_Graph<false>& parent_graph, const std::size_t max_level,
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
 * @details Removes vertices in the subgraph's graph whose level is greater than that of the new requested maximum. The
 * local-to-global map and levels are updated. If the global-to-local map is not a nullptr it is (re)populated.
 */
void Adjacency_Subgraph::remove_levels(const Adjacency_Graph<false>& parent_graph, const std::size_t max_level,
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
 * @details Outputs the subgraph data to 'screen', each vertex is printed, followed by its adjacent vertices. If the
 * vertex adjacency is empty then a period is placed in the row. Following this the local-to-global map is printed with
 * the vertex level value placed in parenthesis.
 *
 * The output format looks like:
 * 1, 3
 * 0
 * .
 * 0, 1
 * 0 -> 165 (0)
 * 1 -> 154 (0)
 * 2 -> 161 (0)
 * 2 -> 150 (1)
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
