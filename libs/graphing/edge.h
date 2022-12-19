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
// File Name: edge.h
// Description: Contains the 'alias' for an edge, as well as some utility functions.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_EDGE_H
#define DISA_EDGE_H

#include "macros.h"

#include <utility>

namespace Disa {

//----------------------------------------------------------------------------------------------------------------------
// Edge
//----------------------------------------------------------------------------------------------------------------------

using Edge = std::pair<std::size_t, std::size_t>; //! Definition of an edge, alias for a pair of unsigned ints.

/**
 * @brief Returns an ordered vertex list for an edge (without duplicated memory).
 * @param[in] edge The edge to re_order.
 * @return A new 'edge', [reference to lower indexed vertex, reference to upper indexed vertex].
 */
inline constexpr std::pair<const std::size_t&, const std::size_t&> order_edge_vertex(const Edge* edge) {
  ASSERT_DEBUG(edge != nullptr, "Parsed edge point is null.");
  return edge->first < edge->second ? std::pair<const std::size_t&, const std::size_t&>({edge->first, edge->second})
                                    : std::pair<const std::size_t&, const std::size_t&>({edge->second, edge->first});
}

}

#endif //DISA_EDGE_H
