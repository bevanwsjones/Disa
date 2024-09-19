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

TEST(test_matrix_sparse, size) {
  CSR_Data<Scalar> data{};
  EXPECT_EQ(size_row(data), 0);
  EXPECT_EQ(size_column(data), 0);
  EXPECT_EQ(size_non_zero(data), 0);

  resize(data, 7, 6);
  data.column_index.push_back(3);
  data.value.push_back(1.0);
  data.row_offset[8] = 1;  // add additional entry - show that non-zero increased.
  EXPECT_EQ(size_row(data), 7);
  EXPECT_EQ(size_column(data), 6);
  EXPECT_EQ(size_non_zero(data), 1);
}

TEST(test_matrix_sparse, resize) {

  CSR_Data<Scalar> data{};

  // check row and column expansion
  resize(data, 5, 9);
  EXPECT_EQ(size_row(data), 5);
  EXPECT_EQ(size_column(data), 9);
  EXPECT_EQ(size_non_zero(data), 0);

  // check row reduction
  data = CSR_Data<Scalar>({.row_offset = {0, 2, 5, 5, 7},
                           .column_index = {1, 3, 0, 2, 3, 3, 4},
                           .value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                           .columns = 5});
  resize(data, 3, 5);
  EXPECT_EQ(size_row(data), 3);
  EXPECT_EQ(size_column(data), 5);
  EXPECT_EQ(size_non_zero(data), 5);

  // check column reduction
  data = CSR_Data<Scalar>({.row_offset = {0, 2, 5, 5, 7},
                           .column_index = {1, 3, 0, 2, 3, 3, 4},
                           .value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                           .columns = 5});
  resize(data, 4, 2);
  EXPECT_EQ(size_row(data), 4);
  EXPECT_EQ(size_column(data), 2);
  EXPECT_EQ(size_non_zero(data), 2);
  // row 0
  EXPECT_EQ(data.row_offset[0], 0);
  EXPECT_EQ(data.column_index[0], 1);
  EXPECT_EQ(data.value[0], 1.0);
  // row 1
  EXPECT_EQ(data.row_offset[1], 1);
  EXPECT_EQ(data.column_index[1], 0);
  EXPECT_EQ(data.value[1], 3.0);
  // end
  EXPECT_EQ(data.row_offset[2], 2);

  // check row reduction with column expansion
  data = CSR_Data<Scalar>({.row_offset = {0, 2, 5, 5, 7},
                           .column_index = {1, 3, 0, 2, 3, 3, 4},
                           .value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                           .columns = 5});
  resize(data, 2, 8);
  EXPECT_EQ(size_row(data), 2);
  EXPECT_EQ(size_column(data), 8);
  EXPECT_EQ(size_non_zero(data), 5);
  // row 0
  EXPECT_EQ(data.row_offset[0], 0);
  EXPECT_EQ(data.column_index[0], 1);
  EXPECT_EQ(data.value[0], 1.0);
  EXPECT_EQ(data.column_index[1], 3);
  EXPECT_EQ(data.value[1], 2.0);
  // row 1
  EXPECT_EQ(data.row_offset[1], 2);
  EXPECT_EQ(data.column_index[2], 0);
  EXPECT_EQ(data.value[2], 3.0);
  EXPECT_EQ(data.column_index[3], 2);
  EXPECT_EQ(data.value[3], 4.0);
  EXPECT_EQ(data.column_index[4], 3);
  EXPECT_EQ(data.value[4], 5.0);
  // end
  EXPECT_EQ(data.row_offset[2], 5);

  // check row expansion with column reduction
  data = CSR_Data<Scalar>({.row_offset = {0, 2, 5, 5, 7},
                           .column_index = {1, 3, 0, 2, 3, 3, 4},
                           .value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                           .columns = 5});
  resize(data, 10, 2);
  EXPECT_EQ(size_row(data), 10);
  EXPECT_EQ(size_column(data), 2);
  EXPECT_EQ(size_non_zero(data), 2);
  // row 0
  EXPECT_EQ(data.row_offset[0], 0);
  EXPECT_EQ(data.column_index[0], 1);
  EXPECT_EQ(data.value[0], 1.0);
  // row 1
  EXPECT_EQ(data.row_offset[1], 1);
  EXPECT_EQ(data.column_index[1], 0);
  EXPECT_EQ(data.value[1], 3.0);
  // end
  EXPECT_EQ(data.row_offset[2], 2);

  // check row and column reduction
  data = CSR_Data<Scalar>({.row_offset = {0, 2, 5, 5, 7},
                           .column_index = {1, 3, 0, 2, 3, 3, 4},
                           .value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                           .columns = 5});
  resize(data, 1, 2);
  EXPECT_EQ(size_row(data), 1);
  EXPECT_EQ(size_column(data), 2);
  EXPECT_EQ(size_non_zero(data), 1);
  // row 0
  EXPECT_EQ(data.row_offset[0], 0);
  EXPECT_EQ(data.column_index[0], 1);
  EXPECT_EQ(data.value[0], 1.0);
  EXPECT_EQ(data.row_offset[1], 1);

  // check zero sizing.
  data = CSR_Data<Scalar>({.row_offset = {0, 2, 5, 5, 7},
                           .column_index = {1, 3, 0, 2, 3, 3, 4},
                           .value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                           .columns = 5});
  resize(data, 0, 0);
  EXPECT_EQ(size_row(data), 0);
  EXPECT_EQ(size_column(data), 0);
  EXPECT_EQ(size_non_zero(data), 0);
}

TEST(test_matrix_sparse, lower_bound) {
  CSR_Data<Scalar> data = {
  .row_offset = {0, 1, 3, 3}, .column_index = {1, 0, 2}, .value = {3.0, 4.0, 5.0}, .columns = 3};

  // row 0
  EXPECT_EQ(*std::get<0>(lower_bound(data, 0, 0)), 0);
  EXPECT_EQ(*std::get<1>(lower_bound(data, 0, 0)), 1);
  EXPECT_EQ(*std::get<2>(lower_bound(data, 0, 0)), 3.0);

  EXPECT_EQ(*std::get<0>(lower_bound(data, 0, 1)), 0);
  EXPECT_EQ(*std::get<1>(lower_bound(data, 0, 1)), 1);
  EXPECT_EQ(*std::get<2>(lower_bound(data, 0, 1)), 3.0);

  EXPECT_EQ(*std::get<0>(lower_bound(data, 0, 2)), 1);    // equivalent to end (next row offset: 1)
  EXPECT_EQ(*std::get<1>(lower_bound(data, 0, 2)), 0);    // equivalent to end (next col: 0)
  EXPECT_EQ(*std::get<2>(lower_bound(data, 0, 2)), 4.0);  // equivalent to end (next value: 4)

  // row 1
  EXPECT_EQ(*std::get<0>(lower_bound(data, 1, 0)), 1);
  EXPECT_EQ(*std::get<1>(lower_bound(data, 1, 0)), 0);
  EXPECT_EQ(*std::get<2>(lower_bound(data, 1, 0)), 4.0);

  EXPECT_EQ(*std::get<0>(lower_bound(data, 1, 1)), 1);
  EXPECT_EQ(*std::get<1>(lower_bound(data, 1, 1)), 2);
  EXPECT_EQ(*std::get<2>(lower_bound(data, 1, 1)), 5.0);

  EXPECT_EQ(*std::get<0>(lower_bound(data, 1, 2)), 1);
  EXPECT_EQ(*std::get<1>(lower_bound(data, 1, 2)), 2);
  EXPECT_EQ(*std::get<2>(lower_bound(data, 1, 2)), 5.0);

  // row 2
  EXPECT_EQ(std::get<0>(lower_bound(data, 2, 0)), data.row_offset.end() - 1);  // equivalent to end (i.e. size row)
  EXPECT_EQ(std::get<1>(lower_bound(data, 2, 0)), data.column_index.end());    // equivalent to end of columns
  EXPECT_EQ(std::get<2>(lower_bound(data, 2, 0)), data.value.end());           // equivalent to end of entries/values.

  EXPECT_EQ(std::get<0>(lower_bound(data, 2, 1)), data.row_offset.end() - 1);  // equivalent to end (i.e. size row)
  EXPECT_EQ(std::get<1>(lower_bound(data, 2, 1)), data.column_index.end());    // equivalent to end of columns
  EXPECT_EQ(std::get<2>(lower_bound(data, 2, 1)), data.value.end());           // equivalent to end of entries/values.

  EXPECT_EQ(std::get<0>(lower_bound(data, 2, 2)), data.row_offset.end() - 1);  // equivalent to end (i.e. size row)
  EXPECT_EQ(std::get<1>(lower_bound(data, 2, 2)), data.column_index.end());    // equivalent to end of columns
  EXPECT_EQ(std::get<2>(lower_bound(data, 2, 2)), data.value.end());           // equivalent to end of entries/values.

  // catch when request row is greater than
  EXPECT_EQ(std::get<0>(lower_bound(data, 3, 0)), data.row_offset.end());    // real row end.
  EXPECT_EQ(std::get<1>(lower_bound(data, 3, 0)), data.column_index.end());  // equivalent to end of columns
  EXPECT_EQ(std::get<2>(lower_bound(data, 3, 0)), data.value.end());         // equivalent to end of entries/values.

  // catch when request column is greater than column (result should be same as row 0 last test.)
  EXPECT_EQ(*std::get<0>(lower_bound(data, 0, 3)), 1);
  EXPECT_EQ(*std::get<1>(lower_bound(data, 0, 3)), 0);
  EXPECT_EQ(*std::get<2>(lower_bound(data, 0, 3)), 4.0);
}