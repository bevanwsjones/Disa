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

#include "edge.hpp"
#include "gtest/gtest.h"

using namespace Disa;

// ---------------------------------------------------------------------------------------------------------------------
// Edge
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_edge, order_edge_vertex) {
  Edge edge({0, 1});
  EXPECT_EQ(order_edge_vertex(&edge).first, 0);
  EXPECT_EQ(order_edge_vertex(&edge).second, 1);

  edge = Edge({3, 2});
  EXPECT_EQ(order_edge_vertex(&edge).first, 2);
  EXPECT_EQ(order_edge_vertex(&edge).second, 3);

  EXPECT_DEATH(order_edge_vertex(nullptr), ".*");
}
