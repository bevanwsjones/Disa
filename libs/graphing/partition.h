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
// File Name: partition.h
// Description: Contains declaration of partitioning methods.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_PARTITION_H
#define DISA_PARTITION_H

#include <vector>

namespace Disa {

template<bool _directed>
class Adjacency_Graph;
class Adjacency_Subgraph;

// ---------------------------------------------------------------------------------------------------------------------
// Level-Set Partitioning
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Performs level set expansion on subgraphs to partition a graph, requires an initial partitioning.
 * @param[in] graph The input graph to partition.
 * @param[in] max_iter The number of iterations to perform.
 * @param[in, out] subgraph_list The list of initial subgraphs, and on completion, the final partitioning.
 */
void multinode_level_set_expansion(const Adjacency_Graph<false>& graph, std::size_t max_iter,
                                   std::vector<Adjacency_Subgraph>& subgraph_list);

/**
 * @brief Recursively bisects a given graph into a specified number of subgraphs.
 * @param[in] graph The original graph to be bisected.
 * @param[in] number_partitions The number of subgraphs to be generated.
 * @return A vector of subgraphs representing the bisected graph.
 */
std::vector<Adjacency_Subgraph> recursive_graph_bisection(const Adjacency_Graph<false>& graph, 
                                                          std::size_t number_partitions);

}

#endif //DISA_PARTITION_H
