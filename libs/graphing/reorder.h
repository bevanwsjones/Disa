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
// File Name: reorder.h
// Description: Declarations for the various reordering algorithms available in Disa.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_REORDER_H
#define DISA_REORDER_H

#include "adjacency_graph.h"

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Level-Set Orderings
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Using the bread first (search) algorithm and a parsed new vertex root the non-disjoint graph is reordered.
 * @param[in] graph The graph to reorder.
 * @param[in] root_vertex The new root, or starting vertex for the reordering. Defaults to 0.
 * @return A mapping for the graph where new_index = re_order[old_index].
 */
[[nodiscard]] std::vector<std::size_t> breadth_first(const AdjacencyGraph& graph, std::size_t root_vertex = 0);

}
#endif //DISA_REORDER_H
