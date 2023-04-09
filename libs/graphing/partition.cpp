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
//  File Name:
//  Description:
// ----------------------------------------------------------------------------------------------------------------------

#include "adjacency_graph.h"
#include "adjacency_subgraph.h"
#include "graph_utilities.h"
#include "partition.h"

#include <cmath>
#include <numeric>
#include <vector>
#include <queue>

namespace Disa {

/**
 *
 *
 */
std::vector<Adjacency_Subgraph> multinode_level_set_expansion(const Adjacency_Graph& graph,
                                                              std::size_t number_partitions,
                                                              std::size_t iterations){

  std::vector<Adjacency_Subgraph> subgraph_list = recursive_graph_bisection(graph, number_partitions);

  std::vector<std::size_t> subgraph_seed(number_partitions);
  std::vector<std::size_t> vertex_colors;
  std::vector<std::vector<std::size_t> > vertex_eccentricity;
  eccentricity_graph(graph, vertex_eccentricity);
  std::vector<std::vector<std::size_t> > vertex_subgraph(number_partitions);

  FOR_EACH(row, vertex_eccentricity)
  {
    FOR_EACH(col, row) {std::cout<<col<<", ";}
    std::cout<<"\n";
  }

  FOR(iter, iterations){
    std::cout<<"\nIter: "<<iter;
    std::cout<<"\nSubgraph sizes: ";
    FOR_EACH(sg, subgraph_list) std::cout<<sg.size_vertex()<<", ";

    // find 'nucleation' seed sites.
    FOR(i_subgraph, subgraph_list.size()) {

      // Using the graph eccentricity we search in each graph for 'central vertices.
//    FOR_EACH(col, row) {std::cout<<col<<", ";}
      const auto& subgraph = subgraph_list[i_subgraph];
      std::size_t minimum_excentricity = std::numeric_limits<std::size_t>::max();
      subgraph_seed[i_subgraph] = std::numeric_limits<std::size_t>::max();
      FOR(i_vertex_0, subgraph_list[i_subgraph].size_vertex()){
        std::size_t i_max_eccentricity = 0;
        FOR(i_vertex_1, subgraph_list[i_subgraph].size_vertex()) {
          const bool is_verted_lower = subgraph.local_global(i_vertex_0) < subgraph.local_global(i_vertex_1);
          const std::size_t i_global_lower = is_verted_lower ? subgraph.local_global(i_vertex_0) : subgraph.local_global(i_vertex_1);
          const std::size_t i_global_upper = is_verted_lower ? subgraph.local_global(i_vertex_1) : subgraph.local_global(i_vertex_0);
          i_max_eccentricity = std::max(vertex_eccentricity[i_global_upper][i_global_lower], i_max_eccentricity);
        }

        if(minimum_excentricity > i_max_eccentricity){
          minimum_excentricity = i_max_eccentricity;
          subgraph_seed[i_subgraph] = subgraph.local_global(i_vertex_0);
        }
      }

    }

    std::cout<<"\nsubgraph seeds: ";
    FOR_EACH(seed, subgraph_seed) std::cout<<seed<<", ";

    // perform level expention from the 'seeding vertexes'
    level_expansion(graph, subgraph_seed, vertex_colors);

    FOR_EACH_REF(vertex_sub, vertex_subgraph) vertex_sub.clear();
    FOR(i_vertex, vertex_colors.size()) vertex_subgraph[vertex_colors[i_vertex]].push_back(i_vertex);


    FOR(i_subgraph, subgraph_list.size()){
      subgraph_list[i_subgraph] = Adjacency_Subgraph(graph, vertex_subgraph[i_subgraph]);
    }
  }
  return subgraph_list;
};

/**
 * This function recursively bisects the input graph into a specified number of partitions. The input graph must be an
 * instance of the Adjacency_Graph class. The function returns a vector of Adjacency_Subgraph objects, each representing
 * one of the partitions of the input graph. The algorithm works by iteratively finding the largest partition in the
 * current set of subgraphs, and bisecting it into two partitions. This process continues until the desired number of
 * partitions is reached. The bisection is performed by finding the vertex with the highest eccentricity (distance to
 * the vertex furthest away from it), and splitting the graph into two partitions using the vertices at a distance less
 * than or equal to the midpoint of the eccentricity range as one partition, and the rest as the other partition. The
 * levels vector passed as an argument is hijacked and used to store the vertices of the partition. The left_partition
 * and right_partition vectors store the indices of the vertices that belong to the two new partitions. Note that the
 * function asserts that the number of partitions is greater than zero, and that the input graph must have at least one
 * vertex.
 */
std::vector<Adjacency_Subgraph> recursive_graph_bisection(const Adjacency_Graph& graph, std::size_t number_partitions){
  ASSERT_DEBUG(number_partitions > 0, "Cannot split a graph into zero domains.");

  std::vector<std::size_t> levels(graph.size_vertex()); // Tricky: hi-jacking this vector of sub-graph initialisation.
  std::iota(levels.begin(), levels.end(), 0);
  std::vector<Adjacency_Subgraph> subgraph(1, Adjacency_Subgraph(graph, levels));

  std::vector<std::size_t> i_sub_graph;

  --number_partitions; // n - 1 partitions
  while(number_partitions != 0){

    // find domain to split
    const auto& split_graph = std::max_element(subgraph.begin(), subgraph.end(),
                                               [](const auto& sg0, const auto& sg1)
                                               {return sg0.size_vertex() < sg1.size_vertex();});

    levels = level_traversal(*split_graph, pseudo_peripheral_vertex(*split_graph));
    const std::size_t& max_level = *std::max_element(levels.begin(), levels.end());
    const std::size_t middle_level = std::ceil(max_level/2);

    std::vector<std::size_t> left_partition, right_partition;
    FOR(i_vertex, levels.size()) { // and here ------------------
      if(levels[i_vertex] <= middle_level) left_partition.push_back(split_graph->local_global(i_vertex));
      else right_partition.push_back(split_graph->local_global(i_vertex));
    }

    *split_graph = Adjacency_Subgraph(graph, left_partition, 0);
    subgraph.emplace_back(graph, right_partition, 0);
    --number_partitions;
  }

  return subgraph;
};

}
