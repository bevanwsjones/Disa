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
// File Name: test_csr_data.hpp
// Description: Testing the CSR_Data class, and all free functions.
// ---------------------------------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "matrix_sparse_data.hpp"

using namespace Disa;

TEST(test_csr_data, size) {
  CSR_Data<Scalar> data{};
  EXPECT_EQ(size_row(data), 0);
  EXPECT_EQ(size_column(data), 0);
  EXPECT_EQ(size_non_zero(data), 0);

  resize(data, 7, 6);
  data.i_column.push_back(3);
  data.value.push_back(1.0);
  data.row_offset[8] = 1;  // add additional entry - show that non-zero increased.
  EXPECT_EQ(size_row(data), 7);
  EXPECT_EQ(size_column(data), 6);
  EXPECT_EQ(size_non_zero(data), 1);
}

TEST(test_csr_data, resize) {

  CSR_Data<Scalar> data{};

  // Death test
  EXPECT_DEATH(resize(data, -1, 1), ".*");
  EXPECT_DEATH(resize(data, 1, -1), ".*");

  // check row and column expansion
  resize(data, 5, 9);
  EXPECT_EQ(size_row(data), 5);
  EXPECT_EQ(size_column(data), 9);
  EXPECT_EQ(size_non_zero(data), 0);

  // check row reduction
  data = CSR_Data<Scalar>({.row_offset = {0, 2, 5, 5, 7},
                           .i_column = {1, 3, 0, 2, 3, 3, 4},
                           .value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                           .columns = 5});
  resize(data, 3, 5);
  EXPECT_EQ(size_row(data), 3);
  EXPECT_EQ(size_column(data), 5);
  EXPECT_EQ(size_non_zero(data), 5);

  // check column reduction
  data = CSR_Data<Scalar>({.row_offset = {0, 2, 5, 5, 7},
                           .i_column = {1, 3, 0, 2, 3, 3, 4},
                           .value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                           .columns = 5});
  resize(data, 4, 2);
  EXPECT_EQ(size_row(data), 4);
  EXPECT_EQ(size_column(data), 2);
  EXPECT_EQ(size_non_zero(data), 2);
  // row 0
  EXPECT_EQ(data.row_offset[0], 0);
  EXPECT_EQ(data.i_column[0], 1);
  EXPECT_EQ(data.value[0], 1.0);
  // row 1
  EXPECT_EQ(data.row_offset[1], 1);
  EXPECT_EQ(data.i_column[1], 0);
  EXPECT_EQ(data.value[1], 3.0);
  // end
  EXPECT_EQ(data.row_offset[2], 2);

  // check row reduction with column expansion
  data = CSR_Data<Scalar>({.row_offset = {0, 2, 5, 5, 7},
                           .i_column = {1, 3, 0, 2, 3, 3, 4},
                           .value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                           .columns = 5});
  resize(data, 2, 8);
  EXPECT_EQ(size_row(data), 2);
  EXPECT_EQ(size_column(data), 8);
  EXPECT_EQ(size_non_zero(data), 5);
  // row 0
  EXPECT_EQ(data.row_offset[0], 0);
  EXPECT_EQ(data.i_column[0], 1);
  EXPECT_EQ(data.value[0], 1.0);
  EXPECT_EQ(data.i_column[1], 3);
  EXPECT_EQ(data.value[1], 2.0);
  // row 1
  EXPECT_EQ(data.row_offset[1], 2);
  EXPECT_EQ(data.i_column[2], 0);
  EXPECT_EQ(data.value[2], 3.0);
  EXPECT_EQ(data.i_column[3], 2);
  EXPECT_EQ(data.value[3], 4.0);
  EXPECT_EQ(data.i_column[4], 3);
  EXPECT_EQ(data.value[4], 5.0);
  // end
  EXPECT_EQ(data.row_offset[2], 5);

  // check row expansion with column reduction
  data = CSR_Data<Scalar>({.row_offset = {0, 2, 5, 5, 7},
                           .i_column = {1, 3, 0, 2, 3, 3, 4},
                           .value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                           .columns = 5});
  resize(data, 10, 2);
  EXPECT_EQ(size_row(data), 10);
  EXPECT_EQ(size_column(data), 2);
  EXPECT_EQ(size_non_zero(data), 2);
  // row 0
  EXPECT_EQ(data.row_offset[0], 0);
  EXPECT_EQ(data.i_column[0], 1);
  EXPECT_EQ(data.value[0], 1.0);
  // row 1
  EXPECT_EQ(data.row_offset[1], 1);
  EXPECT_EQ(data.i_column[1], 0);
  EXPECT_EQ(data.value[1], 3.0);
  // end
  EXPECT_EQ(data.row_offset[2], 2);

  // check row and column reduction
  data = CSR_Data<Scalar>({.row_offset = {0, 2, 5, 5, 7},
                           .i_column = {1, 3, 0, 2, 3, 3, 4},
                           .value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                           .columns = 5});
  resize(data, 1, 2);
  EXPECT_EQ(size_row(data), 1);
  EXPECT_EQ(size_column(data), 2);
  EXPECT_EQ(size_non_zero(data), 1);
  // row 0
  EXPECT_EQ(data.row_offset[0], 0);
  EXPECT_EQ(data.i_column[0], 1);
  EXPECT_EQ(data.value[0], 1.0);
  EXPECT_EQ(data.row_offset[1], 1);

  // check zero sizing.
  data = CSR_Data<Scalar>({.row_offset = {0, 2, 5, 5, 7},
                           .i_column = {1, 3, 0, 2, 3, 3, 4},
                           .value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                           .columns = 5});
  resize(data, 0, 0);
  EXPECT_EQ(size_row(data), 0);
  EXPECT_EQ(size_column(data), 0);
  EXPECT_EQ(size_non_zero(data), 0);
}

TEST(test_csr_data, insert) {
  CSR_Data<Scalar> data;
  resize(data, 5, 5);

  // death tests
  EXPECT_DEATH(insert(data, -1, 1, 1.0), ".*");
  EXPECT_DEATH(insert(data, 1, -1, 1.0), ".*");

  // add A[3][2] = 1.0
  auto [iter, is_insert] = insert(data, 3, 2, 1.0);
  auto [iter_row, iter_column, iter_value] = iter;
  EXPECT_TRUE(is_insert);
  EXPECT_EQ(size_row(data), 5);
  EXPECT_EQ(size_column(data), 5);
  EXPECT_EQ(size_non_zero(data), 1);
  EXPECT_EQ(*iter_row, 0);  // together with size_non_zero implicitly checks correct update of offsets.
  EXPECT_EQ(i_row(data, iter_row), 3);
  EXPECT_EQ(*iter_column, 2);
  EXPECT_EQ(*iter_value, 1.0);

  // add A[3][1] = 3.0 - we insert out of order - will check order at the end.
  std::tie(iter, is_insert) = insert(data, 3, 1, 3.0);
  std::tie(iter_row, iter_column, iter_value) = iter;
  EXPECT_TRUE(is_insert);
  EXPECT_EQ(size_row(data), 5);
  EXPECT_EQ(size_column(data), 5);
  EXPECT_EQ(size_non_zero(data), 2);
  EXPECT_EQ(*iter_row, 0);
  EXPECT_EQ(i_row(data, iter_row), 3);
  EXPECT_EQ(*iter_column, 1);
  EXPECT_EQ(*iter_value, 3.0);

  // add A[2][1] = 4.0 - Previous row test
  std::tie(iter, is_insert) = insert(data, 2, 1, 4.0);
  std::tie(iter_row, iter_column, iter_value) = iter;
  EXPECT_TRUE(is_insert);
  EXPECT_EQ(size_row(data), 5);
  EXPECT_EQ(size_column(data), 5);
  EXPECT_EQ(size_non_zero(data), 3);
  EXPECT_EQ(*iter_row, 0);
  EXPECT_EQ(i_row(data, iter_row), 2);
  EXPECT_EQ(*iter_column, 1);
  EXPECT_EQ(*iter_value, 4.0);

  // add A[2][4] = 4.0 - last of columns test
  std::tie(iter, is_insert) = insert(data, 2, 4, 5.0);
  std::tie(iter_row, iter_column, iter_value) = iter;
  EXPECT_TRUE(is_insert);
  EXPECT_EQ(size_row(data), 5);
  EXPECT_EQ(size_column(data), 5);
  EXPECT_EQ(size_non_zero(data), 4);
  EXPECT_EQ(*iter_row, 0);
  EXPECT_EQ(i_row(data, iter_row), 2);
  EXPECT_EQ(*iter_column, 4);
  EXPECT_EQ(*iter_value, 5.0);

  // add A[4][0] = 8.0 - last of rows test
  std::tie(iter, is_insert) = insert(data, 4, 0, 8.0);
  std::tie(iter_row, iter_column, iter_value) = iter;
  EXPECT_TRUE(is_insert);
  EXPECT_EQ(size_row(data), 5);
  EXPECT_EQ(size_column(data), 5);
  EXPECT_EQ(size_non_zero(data), 5);
  EXPECT_EQ(*iter_row, 4);
  EXPECT_EQ(i_row(data, iter_row), 4);
  EXPECT_EQ(*iter_column, 0);
  EXPECT_EQ(*iter_value, 8.0);

  // add A[4][4] = -5.0 - last row and column test
  std::tie(iter, is_insert) = insert(data, 4, 4, -5.0);
  std::tie(iter_row, iter_column, iter_value) = iter;
  EXPECT_TRUE(is_insert);
  EXPECT_EQ(size_row(data), 5);
  EXPECT_EQ(size_column(data), 5);
  EXPECT_EQ(size_non_zero(data), 6);
  EXPECT_EQ(*iter_row, 4);
  EXPECT_EQ(i_row(data, iter_row), 4);
  EXPECT_EQ(*iter_column, 4);
  EXPECT_EQ(*iter_value, -5.0);

  // try add A[3][2] = -5.0 (blocked) - no insert test
  std::tie(iter, is_insert) = insert(data, 3, 2, -5.0);
  std::tie(iter_row, iter_column, iter_value) = iter;
  EXPECT_FALSE(is_insert);
  EXPECT_EQ(size_row(data), 5);
  EXPECT_EQ(size_column(data), 5);
  EXPECT_EQ(size_non_zero(data), 6);
  EXPECT_EQ(*iter_row, 2);
  EXPECT_EQ(i_row(data, iter_row), 3);
  EXPECT_EQ(*iter_column, 2);
  EXPECT_EQ(*iter_value, 1.0);  // previous insert

  // add A[6][2] = 10.0 - new 'key' insert by adding an extra row.
  std::tie(iter, is_insert) = insert(data, 6, 2, 10.0);
  std::tie(iter_row, iter_column, iter_value) = iter;
  EXPECT_TRUE(is_insert);
  EXPECT_EQ(size_row(data), 7);
  EXPECT_EQ(size_column(data), 5);
  EXPECT_EQ(size_non_zero(data), 7);
  EXPECT_EQ(*iter_row, 6);
  EXPECT_EQ(i_row(data, iter_row), 6);
  EXPECT_EQ(*iter_column, 2);
  EXPECT_EQ(*iter_value, 10.0);

  // add A[6][2] = 10.0 - new 'key' insert by adding an extra column.
  std::tie(iter, is_insert) = insert(data, 2, 6, 50.0);
  std::tie(iter_row, iter_column, iter_value) = iter;
  EXPECT_TRUE(is_insert);
  EXPECT_EQ(size_row(data), 7);
  EXPECT_EQ(size_column(data), 7);
  EXPECT_EQ(size_non_zero(data), 8);
  EXPECT_EQ(*iter_row, 0);
  EXPECT_EQ(i_row(data, iter_row), 2);
  EXPECT_EQ(*iter_column, 6);
  EXPECT_EQ(*iter_value, 50.0);

  // Check data structure integrity before.
  iter_row = data.row_offset.begin();
  iter_column = data.i_column.begin();
  iter_value = data.value.begin();
  for(auto check_value : std::vector<CSR_Data<Scalar>::index_type>({0, 0, 0, 3, 5, 7, 7, 8}))
    EXPECT_EQ(*iter_row++, check_value);
  for(auto check_value : std::vector<CSR_Data<Scalar>::index_type>({1, 4, 6, 1, 2, 0, 4, 2}))
    EXPECT_EQ(*iter_column++, check_value);
  for(auto check_value : std::vector<CSR_Data<Scalar>::value_type>({4.0, 5.0, 50.0, 3.0, 1.0, 8.0, -5.0, 10.0}))
    EXPECT_EQ(*iter_value++, check_value);
  EXPECT_EQ(data.columns, 7);
}

TEST(test_csr_data, insert_or_assign) {
  CSR_Data<Scalar> data;
  resize(data, 3, 3);

  // Test insertion of new element
  auto [iter1, is_insert1] = insert_or_assign(data, 1, 1, 5.0);
  EXPECT_TRUE(is_insert1);
  EXPECT_EQ(*std::get<2>(iter1), 5.0);
  EXPECT_EQ(size_non_zero(data), 1);

  // Test assignment to existing element
  auto [iter2, is_insert2] = insert_or_assign(data, 1, 1, 7.0);
  EXPECT_FALSE(is_insert2);
  EXPECT_EQ(*std::get<2>(iter2), 7.0);
  EXPECT_EQ(size_non_zero(data), 1);

  // Test insertion that causes row expansion
  auto [iter3, is_insert3] = insert_or_assign(data, 4, 1, 3.0);
  EXPECT_TRUE(is_insert3);
  EXPECT_EQ(*std::get<2>(iter3), 3.0);
  EXPECT_EQ(size_row(data), 5);
  EXPECT_EQ(size_non_zero(data), 2);

  // Test insertion that causes column expansion
  auto [iter4, is_insert4] = insert_or_assign(data, 2, 4, 9.0);
  EXPECT_TRUE(is_insert4);
  EXPECT_EQ(*std::get<2>(iter4), 9.0);
  EXPECT_EQ(size_column(data), 5);
  EXPECT_EQ(size_non_zero(data), 3);

  // Verify final state
  EXPECT_EQ(size_row(data), 5);
  EXPECT_EQ(size_column(data), 5);
  EXPECT_EQ(size_non_zero(data), 3);

  // Check values
  EXPECT_EQ(*std::get<2>(lower_bound(data, 1, 1)), 7.0);
  EXPECT_EQ(*std::get<2>(lower_bound(data, 4, 1)), 3.0);
  EXPECT_EQ(*std::get<2>(lower_bound(data, 2, 4)), 9.0);
}

TEST(test_csr_data, lower_bound) {
  CSR_Data<Scalar> data = {.row_offset = {0, 1, 3, 3}, .i_column = {1, 0, 2}, .value = {3.0, 4.0, 5.0}, .columns = 3};

  // death tests
  EXPECT_DEATH(lower_bound(data, -1, 1), ".*");
  EXPECT_DEATH(lower_bound(data, 1, -1), ".*");

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
  EXPECT_EQ(std::get<1>(lower_bound(data, 2, 0)), data.i_column.end());        // equivalent to end of columns
  EXPECT_EQ(std::get<2>(lower_bound(data, 2, 0)), data.value.end());           // equivalent to end of entries/values.

  EXPECT_EQ(std::get<0>(lower_bound(data, 2, 1)), data.row_offset.end() - 1);  // equivalent to end (i.e. size row)
  EXPECT_EQ(std::get<1>(lower_bound(data, 2, 1)), data.i_column.end());        // equivalent to end of columns
  EXPECT_EQ(std::get<2>(lower_bound(data, 2, 1)), data.value.end());           // equivalent to end of entries/values.

  EXPECT_EQ(std::get<0>(lower_bound(data, 2, 2)), data.row_offset.end() - 1);  // equivalent to end (i.e. size row)
  EXPECT_EQ(std::get<1>(lower_bound(data, 2, 2)), data.i_column.end());        // equivalent to end of columns
  EXPECT_EQ(std::get<2>(lower_bound(data, 2, 2)), data.value.end());           // equivalent to end of entries/values.

  // catch when request row is greater than
  EXPECT_EQ(std::get<0>(lower_bound(data, 3, 0)), data.row_offset.end());  // real row end.
  EXPECT_EQ(std::get<1>(lower_bound(data, 3, 0)), data.i_column.end());    // equivalent to end of columns
  EXPECT_EQ(std::get<2>(lower_bound(data, 3, 0)), data.value.end());       // equivalent to end of entries/values.

  // catch when request column is greater than column (result should be same as row 0 last test.)
  EXPECT_EQ(*std::get<0>(lower_bound(data, 0, 3)), 1);
  EXPECT_EQ(*std::get<1>(lower_bound(data, 0, 3)), 0);
  EXPECT_EQ(*std::get<2>(lower_bound(data, 0, 3)), 4.0);
}