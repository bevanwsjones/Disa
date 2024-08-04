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
// File Name: partition.cpp
// Description: Contains declaration of partitioning methods.
// ---------------------------------------------------------------------------------------------------------------------

#include "partition.hpp"
#include "adjacency_graph.hpp"
#include "adjacency_subgraph.hpp"
#include "graph_utilities.hpp"

#include <cmath>
#include <numeric>
#include <vector>

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Level-Set Partitioning
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details This function performs multinode level set expansion on a set of input subgraphs to 'improve' partition
 * topology. The number of output subgraphs is thus the same as the number of input subgraphs. The method works by first
 * computing the global eccentricity of the input graph. Nucleation seed vertices are then selected for each subgraph by
 * finding the vertex with the minimum maximal eccentricity in each subgraph. From the nucleation sites a level set
 * expansion is performed to create the next iteration of partitions. The process is repeated for the given number of
 * iterations or until the seed vertices do not change between iterations. Once the function returns the update
 * partition will be contained within the subgraph_list.
 */
void multinode_level_set_expansion(const Adjacency_Graph<false>& graph, const std::size_t max_iter,
                                   std::vector<Adjacency_Subgraph>& subgraph_list) {
  ASSERT(!subgraph_list.empty(), "Parsed Subgraph list is empty.");
  ASSERT(
  std::all_of(subgraph_list.begin(), subgraph_list.end(), [&](auto& subgraph) { return subgraph.is_parent(graph); }),
  "The parsed graph is not a parent of all subgraphs.");

  if(subgraph_list.size() == 1) return;  // If its only one partition return.

  // Allocate memory.
  std::vector<std::size_t> vertex_colors;
  std::vector<std::size_t> seed_previous(subgraph_list.size());
  std::vector<std::size_t> seed(subgraph_list.size());
  std::vector<std::vector<std::size_t>> vertex_subgraph(subgraph_list.size());

  // compute global eccentricity.
  std::vector<std::vector<std::size_t>> vertex_eccentricity;
  eccentricity_graph(graph, vertex_eccentricity);

  FOR(iter, max_iter) {
    // find 'nucleation' seed sites.
    FOR(i_subgraph, subgraph_list.size()) {
      // Using the graph eccentricity we search in each graph for central vertices.
      const auto& subgraph = subgraph_list[i_subgraph];
      seed[i_subgraph] = std::numeric_limits<std::size_t>::max();
      std::size_t min_eccentricity = std::numeric_limits<std::size_t>::max();
      FOR(i_vertex_0, subgraph.size_vertex()) {
        std::size_t i_max_eccentricity = 0;

        FOR(i_vertex_1, subgraph.size_vertex()) {
          if(i_vertex_0 == i_vertex_1) continue;
          const bool is_0_low = subgraph.local_global(i_vertex_0) < subgraph.local_global(i_vertex_1);
          const std::size_t& i_global_lower =
          is_0_low ? subgraph.local_global(i_vertex_0) : subgraph.local_global(i_vertex_1);
          const std::size_t& i_global_upper =
          is_0_low ? subgraph.local_global(i_vertex_1) : subgraph.local_global(i_vertex_0);
          i_max_eccentricity = std::max(vertex_eccentricity[i_global_upper][i_global_lower], i_max_eccentricity);
        }

        if(min_eccentricity > i_max_eccentricity) {
          min_eccentricity = i_max_eccentricity;
          seed[i_subgraph] = subgraph.local_global(i_vertex_0);
        }
      }
    }

    // Check if the seeds have moved, if not return, we have 'converged'.
    if(std::ranges::equal(seed_previous, seed)) return;
    else seed_previous = seed;

    // perform level expansion from the 'seeding vertexes'
    level_expansion(graph, seed, vertex_colors);

    FOR_EACH_REF(vertex_sub, vertex_subgraph) vertex_sub.clear();
    FOR(i_vertex, vertex_colors.size()) vertex_subgraph[vertex_colors[i_vertex]].push_back(i_vertex);

    FOR(i_subgraph, subgraph_list.size()) {
      subgraph_list[i_subgraph] = Adjacency_Subgraph(graph, vertex_subgraph[i_subgraph]);
    }
  }
};

/**
 * @details This function performs a recursive bisection of a graph into a specified number of partitions. It uses a
 * level traversal to determine the level at which to split the subgraph with the largest number of vertices, which then
 * forms two new subgraphs. This process is repeated, until the desired number of partitions is achieved.
 */
std::vector<Adjacency_Subgraph> recursive_graph_bisection(const Adjacency_Graph<false>& graph,
                                                          std::size_t number_partitions) {
  ASSERT(number_partitions > 0, "Cannot split a graph into zero domains.");

  // Memory allocation.
  std::vector<std::size_t> left_partition;
  std::vector<std::size_t> right_partition;
  std::vector<std::size_t> levels(graph.size_vertex());

  // Tricky: hi-jacking this vector of sub-graph initialisation.
  std::iota(levels.begin(), levels.end(), 0);
  std::vector<Adjacency_Subgraph> subgraph(1, Adjacency_Subgraph(graph, levels));

  --number_partitions;  // n - 1 partitions
  while(number_partitions != 0) {
    // Determine domain to split
    const auto& split_graph = std::max_element(subgraph.begin(), subgraph.end(), [](const auto& sg0, const auto& sg1) {
      return sg0.size_vertex() < sg1.size_vertex();
    });

    // Perform a level traversal and determine level to split at.
    levels = level_traversal(*split_graph, pseudo_peripheral_vertex(*split_graph));
    const std::size_t middle_level = std::ceil(*std::max_element(levels.begin(), levels.end()) / 2);

    // Split the graph, and form new subgraphs.
    FOR(i_vertex, levels.size())
    if(levels[i_vertex] <= middle_level) left_partition.push_back(split_graph->local_global(i_vertex));
    else right_partition.push_back(split_graph->local_global(i_vertex));
    *split_graph = Adjacency_Subgraph(graph, left_partition, 0);
    subgraph.emplace_back(graph, right_partition, 0);

    // Set for next split.
    left_partition.clear();
    right_partition.clear();
    --number_partitions;
  }
  return subgraph;
};

}  // namespace Disa
