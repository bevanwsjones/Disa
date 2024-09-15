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
// File Name: test_matrix_sparse_data.hpp
// Description: Testing file from the matrix sparse data.
// ---------------------------------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "matrix_sparse_data.hpp"

using namespace Disa;

TEST(test_matrix_sparse, lower_bound) {
  CSR_Data<Scalar, uint> data = {
  .row_offset = {0, 1, 3, 3}, .column_index = {1, 0, 2}, .value = {3.0, 4.0, 5.0}, .columns = 3};

  EXPECT_EQ(std::get<0>(lower_bound(data, 0u, 0u)), 0);
  EXPECT_EQ(std::get<1>(lower_bound(data, 0u, 0u))[0], 1);
  EXPECT_EQ(std::get<2>(lower_bound(data, 0u, 0u))[0], 3.0);
  //EXPECT_EQ(lower_bound(data, 0, 1).i_row(), 0);
  //EXPECT_EQ(lower_bound(data, 0, 1).i_column(), 1);
  //EXPECT_EQ(lower_bound(data, 0, 2), matrix[0].end());

  // EXPECT_EQ(lower_bound(1, 0).i_row(), 1);
  // EXPECT_EQ(lower_bound(1, 0).i_column(), 0);
  // EXPECT_EQ(lower_bound(1, 1).i_row(), 1);
  // EXPECT_EQ(lower_bound(1, 1).i_column(), 2);
  // EXPECT_EQ(lower_bound(1, 2).i_row(), 1);
  // EXPECT_EQ(lower_bound(1, 2).i_column(), 2);

  // EXPECT_EQ(matrix.lower_bound(2, 0), matrix[2].end());
  // EXPECT_EQ(matrix.lower_bound(3, 3), matrix.end()->end());

  // const Matrix_Sparse matrix_const({0, 1, 3, 3}, {1, 0, 2}, {3.0, 4.0, 5.0}, 3);
  // EXPECT_EQ(matrix_const.lower_bound(0, 0).i_row(), 0);
  // EXPECT_EQ(matrix_const.lower_bound(0, 0).i_column(), 1);
  // EXPECT_EQ(matrix_const.lower_bound(0, 1).i_row(), 0);
  // EXPECT_EQ(matrix_const.lower_bound(0, 1).i_column(), 1);
  // EXPECT_EQ(matrix_const.lower_bound(0, 2), matrix_const[0].end());

  // EXPECT_EQ(matrix_const.lower_bound(1, 0).i_row(), 1);
  // EXPECT_EQ(matrix_const.lower_bound(1, 0).i_column(), 0);
  // EXPECT_EQ(matrix_const.lower_bound(1, 1).i_row(), 1);
  // EXPECT_EQ(matrix_const.lower_bound(1, 1).i_column(), 2);
  // EXPECT_EQ(matrix_const.lower_bound(1, 2).i_row(), 1);
  // EXPECT_EQ(matrix_const.lower_bound(1, 2).i_column(), 2);

  // EXPECT_EQ(matrix_const.lower_bound(2, 0), matrix_const[2].end());
  // EXPECT_EQ(matrix_const.lower_bound(3, 3), (*matrix_const.end()).end());
}