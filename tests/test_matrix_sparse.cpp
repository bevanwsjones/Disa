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
//  File Name: test_matrix_sparse.cpp
//  Description: todo
// ----------------------------------------------------------------------------------------------------------------------
//

#include "gtest/gtest.h"
#include "matrix_sparse.h"

using namespace Disa;

//----------------------------------------------------------------------------------------------------------------------
// Constructors/Destructors
//----------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse_row, constructors_row_column) {
  Matrix_Sparse matrix(2, 4);
  EXPECT_EQ(matrix.size_row(), 2);
  EXPECT_EQ(matrix.size_column(), 4);
}

TEST(test_matrix_sparse_row, constructors_initialiser_lists) {
  Matrix_Sparse matrix({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  EXPECT_EQ(matrix.size_row(), 4);
  EXPECT_EQ(matrix.size_column(), 5);
  EXPECT_EQ(matrix.size_non_zero(), 7);
  EXPECT_DOUBLE_EQ(matrix[0][1], 1.0);
  EXPECT_DOUBLE_EQ(matrix[0][3], 2.0);
  EXPECT_DOUBLE_EQ(matrix[1][0], 4.0);
  EXPECT_DOUBLE_EQ(matrix[1][2], 3.0);
  EXPECT_DOUBLE_EQ(matrix[1][3], 5.0);
  EXPECT_DOUBLE_EQ(matrix[3][3], 7.0);
  EXPECT_DOUBLE_EQ(matrix[3][4], 6.0);

  // Valid construction 2x2: Matrix_Sparse({0, 2, 3}, {1, 0, 0}, {1.0, 2.0, 3.0}, 2)
  EXPECT_DEATH(Matrix_Sparse({0, 2, 3}, {1, 0, 0, 1}, {1.0, 2.0, 3.0, 4.0}, 2),
               ".*"); // more column indexes than non-zero.back().
  EXPECT_DEATH(Matrix_Sparse({0, 2, 3}, {1, 0, 0}, {1.0, 2.0, 3.0, 4.0}, 2), ".*"); // mismatch column and entry size.
  EXPECT_DEATH(Matrix_Sparse({1, 2, 3}, {1, 0, 0}, {1.0, 2.0, 3.0}, 2), ".*"); // first non-zero value is not 0.
  EXPECT_DEATH(Matrix_Sparse({2, 0, 3}, {1, 0, 0}, {1.0, 2.0, 3.0}, 2), ".*"); // un-ordered non-zeros
  EXPECT_DEATH(Matrix_Sparse({0, 2, 3}, {1, 0, 2}, {1.0, 2.0, 3.0}, 2), ".*"); // column index out of range
  EXPECT_DEATH(Matrix_Sparse({0, 2, 3}, {1, 1, 0}, {1.0, 2.0, 3.0}, 2), ".*"); // repeated column index
}

//----------------------------------------------------------------------------------------------------------------------
// Element Access
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
// Iterators
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Capacity/Modifiers
//----------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse_row, empty_size_row_column_size_resize_clear) {

  Matrix_Sparse matrix;
  EXPECT_TRUE(matrix.empty());
  EXPECT_EQ(matrix.size_row(), 0);
  EXPECT_EQ(matrix.size_column(), 0);
  EXPECT_EQ(matrix.size_non_zero(), 0);
  EXPECT_EQ(matrix.size().first, std::make_pair(0, 0).first);
  EXPECT_EQ(matrix.size().second, std::make_pair(0, 0).second);

  matrix.resize(5, 9);
  EXPECT_FALSE(matrix.empty());
  EXPECT_EQ(matrix.size_row(), 5);
  EXPECT_EQ(matrix.size_column(), 9);
  EXPECT_EQ(matrix.size_non_zero(), 0);
  EXPECT_EQ(matrix.size().first, std::make_pair(5, 9).first);
  EXPECT_EQ(matrix.size().second, std::make_pair(5, 9).second);

  matrix[3][8] = 10; // add a random entry, see that non-zero size changes.
  EXPECT_FALSE(matrix.empty());
  EXPECT_EQ(matrix.size_row(), 5);
  EXPECT_EQ(matrix.size_column(), 9);
  EXPECT_EQ(matrix.size_non_zero(), 1);
  EXPECT_EQ(matrix.size().first, std::make_pair(5, 9).first);
  EXPECT_EQ(matrix.size().second, std::make_pair(5, 9).second);

  matrix.clear();
  EXPECT_TRUE(matrix.empty());
  EXPECT_EQ(matrix.size_row(), 0);
  EXPECT_EQ(matrix.size_column(), 0);
  EXPECT_EQ(matrix.size_non_zero(), 0);
  EXPECT_EQ(matrix.size().first, std::make_pair(0, 0).first);
  EXPECT_EQ(matrix.size().second, std::make_pair(0, 0).second);
  EXPECT_EQ(matrix.capacity().first, std::make_pair(6, 1).first); // make sure we have not lost capacity.
  EXPECT_EQ(matrix.capacity().second, std::make_pair(6, 1).second);  // make sure we have not lost capacity.

}

TEST(test_matrix_sparse_row, reserve_capacity) {
  Matrix_Sparse matrix;
  EXPECT_EQ(matrix.capacity().first, 0);
  EXPECT_EQ(matrix.capacity().second, 0);

  matrix.reserve(10, 40);
  EXPECT_EQ(matrix.capacity().first, 10);
  EXPECT_EQ(matrix.capacity().second, 40);

  matrix.resize(5, 30);
  matrix[4][8] = 10;
  matrix.shrink_to_fit();
  EXPECT_EQ(matrix.capacity().first, 6);
  EXPECT_EQ(matrix.capacity().second, 1);
}

TEST(test_matrix_sparse_row, insert) {

  Matrix_Sparse matrix;
  matrix.resize(5, 5);

  matrix.insert(3, 2, 1);
  EXPECT_EQ(matrix.size_non_zero(), 1);
  EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

  matrix.insert(3, 1, 3);
  EXPECT_EQ(matrix.size_non_zero(), 2);
  EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

  matrix.insert(2, 1, 4);
  EXPECT_DOUBLE_EQ(matrix.size_non_zero(), 3);
  EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
  EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

  matrix.insert(2, 4, 5);
  EXPECT_EQ(matrix.size_non_zero(), 4);
  EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
  EXPECT_DOUBLE_EQ(matrix[2][4], 5.0);
  EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

  // check that the columns are ordered.
  FOR(row, matrix.size_row()) {
    EXPECT_TRUE(std::is_sorted(matrix.begin_column(row).first, matrix.begin_column(row).first));
  }

  EXPECT_DEATH(matrix.insert(3, 2, 2.0), "./*");
  EXPECT_DEATH(matrix.insert(6, 2, 1.0), "./*");
  EXPECT_DEATH(matrix.insert(2, 6, 1.0), "./*");
}

TEST(test_matrix_sparse_row, erase) {
  Matrix_Sparse matrix({0, 2, 3}, {1, 0, 0}, {1.0, 2.0, 3.0}, 2);

  matrix.erase(0, 1);
  EXPECT_EQ(matrix.size_row(), 2);
  EXPECT_EQ(matrix.size_column(), 2);
  EXPECT_EQ(matrix.size_non_zero(), 2);
  EXPECT_DOUBLE_EQ(matrix[0][0], 2.0);
  EXPECT_DOUBLE_EQ(matrix[1][0], 3.0);

  EXPECT_DEATH(matrix.erase(3, 0), "./*");
  EXPECT_DEATH(matrix.erase(0, 2), "./*");
  EXPECT_DEATH(matrix.erase(1, 1), "./*");

}

TEST(test_matrix_sparse_row, swap) {
  Matrix_Sparse matrix_0({0, 1, 2}, {1, 0}, {1.0, 2.0}, 2);
  Matrix_Sparse matrix_1({0, 1, 2, 3}, {1, 0, 1}, {3.0, 4.0, 5.0}, 3);

  matrix_0.swap(matrix_1);
  EXPECT_EQ(matrix_0.size_row(), 3);
  EXPECT_EQ(matrix_0.size_column(), 3);
  EXPECT_EQ(matrix_0.size_non_zero(), 3);
  EXPECT_EQ(matrix_1.size_row(), 2);
  EXPECT_EQ(matrix_1.size_column(), 2);
  EXPECT_EQ(matrix_1.size_non_zero(), 2);
  EXPECT_DOUBLE_EQ(matrix_0[0][1], 3.0);
  EXPECT_DOUBLE_EQ(matrix_0[1][0], 4.0);
  EXPECT_DOUBLE_EQ(matrix_0[2][1], 5.0);
  EXPECT_DOUBLE_EQ(matrix_1[0][1], 1.0);
  EXPECT_DOUBLE_EQ(matrix_1[1][0], 2.0);
}