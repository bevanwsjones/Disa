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
// Matrix Sparse
//----------------------------------------------------------------------------------------------------------------------
// Member functions
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

TEST(test_matrix_sparse_row, operator_assignment) {
  Matrix_Sparse matrix_0({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  Matrix_Sparse matrix_1;
  matrix_1 = matrix_0;

  EXPECT_EQ(matrix_0.size_non_zero(), matrix_1.size_non_zero());
  EXPECT_EQ(matrix_0.size(), matrix_1.size());
  FOR(i_row, matrix_0.size_row())
    FOR(i_column, matrix_0.size_column()){
      EXPECT_DOUBLE_EQ(matrix_0[i_row][i_column], matrix_1[i_row][i_column]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Element Access
//----------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse_row, operator_subscript) {
  Matrix_Sparse matrix_0({0, 1, 2}, {0, 1}, {1.0, 2.0}, 2);

  EXPECT_DOUBLE_EQ(matrix_0[0][0], 1.0);
  EXPECT_DOUBLE_EQ(matrix_0[1][1], 2.0);
  matrix_0[0][1] = 3.0; // this should insert a new element.

  const Matrix_Sparse matrix_1 = matrix_0;
  EXPECT_DOUBLE_EQ(matrix_1[0][0], 1.0);
  EXPECT_DOUBLE_EQ(matrix_1[0][1], 3.0);
  EXPECT_DOUBLE_EQ(matrix_1[1][1], 2.0);

  // Check debug for const matrix.
  EXPECT_DEATH(matrix_1[1][0], "./*");
}

//----------------------------------------------------------------------------------------------------------------------
// Iterators
//----------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse_row, begin) {
  Matrix_Sparse matrix({0, 1, 2}, {1, 1}, {1.0, 2.0}, 2);

  // These are more indirect checks, but essentially we should be looking at row 0 data.
  auto iter = matrix.begin();
  EXPECT_DOUBLE_EQ(iter[0][1], 1.0);
  EXPECT_DOUBLE_EQ(iter[0].begin().i_row(), 0);
  EXPECT_DOUBLE_EQ(iter[0].begin().i_column(), 1);

  auto iter_const = matrix.cbegin();
  EXPECT_DOUBLE_EQ(iter_const[0][1], 1.0);
  EXPECT_DOUBLE_EQ(iter_const[0].begin().i_row(), 0);
  EXPECT_DOUBLE_EQ(iter_const[0].begin().i_column(), 1);
}

TEST(test_matrix_sparse_row, end) {
  Matrix_Sparse matrix({0, 1, 2}, {1, 1}, {1.0, 2.0}, 2);

  // These are more indirect checks, but essentially we should be looking at the last row's data.
  auto iter = matrix.end();
  EXPECT_DOUBLE_EQ(iter[-1][1], 2.0);
  EXPECT_DOUBLE_EQ(iter[-1].begin().i_row(), 1);
  EXPECT_DOUBLE_EQ(iter[-1].begin().i_column(), 1);

  auto iter_const = matrix.end();
  EXPECT_DOUBLE_EQ(iter_const[-1][1], 2.0);
  EXPECT_DOUBLE_EQ(iter_const[-1].begin().i_row(), 1);
  EXPECT_DOUBLE_EQ(iter_const[-1].begin().i_column(), 1);
}

//--------------------------------------------------------------------------------------------------------------------
// Capacity
//--------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse_row, empty) {
  Matrix_Sparse matrix;
  EXPECT_TRUE(matrix.empty());
  EXPECT_EQ(matrix.size_row(), 0);
  EXPECT_EQ(matrix.size_column(), 0);
  EXPECT_EQ(matrix.size_non_zero(), 0);
  EXPECT_EQ(matrix.size().first, std::make_pair(0, 0).first);
  EXPECT_EQ(matrix.size().second, std::make_pair(0, 0).second);

  // check edge cases.
  matrix.resize(0, 2);
  EXPECT_TRUE(matrix.empty());
  matrix.resize(1, 0);
  EXPECT_FALSE(matrix.empty());
  matrix.resize(2, 2);
  EXPECT_FALSE(matrix.empty());
}

TEST(test_matrix_sparse_row, size) {
  Matrix_Sparse matrix;
  EXPECT_EQ(matrix.size_row(), 0);
  EXPECT_EQ(matrix.size_column(), 0);
  EXPECT_EQ(matrix.size_non_zero(), 0);
  EXPECT_EQ(matrix.size().first, std::make_pair(0, 0).first);
  EXPECT_EQ(matrix.size().second, std::make_pair(0, 0).second);

  matrix.resize(7, 6);
  matrix[0][5] = 1.0; // add additional entry - show that non-zero increased.
  EXPECT_FALSE(matrix.empty());
  EXPECT_EQ(matrix.size_row(), 7);
  EXPECT_EQ(matrix.size_column(), 6);
  EXPECT_EQ(matrix.size_non_zero(), 1);
  EXPECT_EQ(matrix.size().first, std::make_pair(7, 6).first);
  EXPECT_EQ(matrix.size().second, std::make_pair(7, 6).second);
}

TEST(test_matrix_sparse_row, reserve_capacity) {
  Matrix_Sparse matrix;
  EXPECT_EQ(matrix.capacity().first, 0);
  EXPECT_EQ(matrix.capacity().second, 0);

  matrix.reserve(10, 40);
  EXPECT_EQ(matrix.capacity().first, 10);
  EXPECT_EQ(matrix.capacity().second, 40);
}

TEST(test_matrix_sparse_row, shrink_to_fit) {
  Matrix_Sparse matrix;
  matrix.reserve(10, 40);
  EXPECT_EQ(matrix.capacity().first, 10);
  EXPECT_EQ(matrix.capacity().second, 40);

  matrix.resize(5, 30);
  matrix[4][8] = 10;
  matrix.shrink_to_fit();
  EXPECT_EQ(matrix.capacity().first, 6);
  EXPECT_EQ(matrix.capacity().second, 1);
}

//--------------------------------------------------------------------------------------------------------------------
// Modifiers
//--------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse_row, clear) {
  Matrix_Sparse matrix;
  matrix.resize(7, 6);
  matrix[0][5] = 1.0; // add additional entry - show that non-zero increased.

  matrix.clear();
  EXPECT_TRUE(matrix.empty());
  EXPECT_EQ(matrix.size_row(), 0);
  EXPECT_EQ(matrix.size_column(), 0);
  EXPECT_EQ(matrix.size_non_zero(), 0);
  EXPECT_EQ(matrix.size().first, std::make_pair(0, 0).first);
  EXPECT_EQ(matrix.size().second, std::make_pair(0, 0).second);
  EXPECT_EQ(matrix.capacity().first, std::make_pair(8, 1).first);   // make sure we have not lost capacity.
  EXPECT_EQ(matrix.capacity().second, std::make_pair(8, 1).second); // make sure we have not lost capacity.
}

TEST(test_matrix_sparse_row, insert_insert_or_assign) {
  Matrix_Sparse matrix;
  matrix.resize(5, 5);

  matrix.insert(3, 2, 1);
  EXPECT_EQ(matrix.size_non_zero(), 1);
  EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

  auto iter_bool_pair = matrix.insert(3, 1, 3); // out of order testing - will check order at the end.
  EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 3.0);
  EXPECT_TRUE(iter_bool_pair.second);
  EXPECT_EQ(matrix.size_non_zero(), 2);
  EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

  iter_bool_pair = matrix.insert(2, 1, 4);
  EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 4.0);
  EXPECT_TRUE(iter_bool_pair.second);
  EXPECT_DOUBLE_EQ(matrix.size_non_zero(), 3);
  EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
  EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

  iter_bool_pair = matrix.insert(2, 4, 5); // last of columns test
  EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 5.0);
  EXPECT_TRUE(iter_bool_pair.second);
  EXPECT_EQ(matrix.size_non_zero(), 4);
  EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
  EXPECT_DOUBLE_EQ(matrix[2][4], 5.0);
  EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

  iter_bool_pair = matrix.insert(4, 0, 8); // last of rows test
  EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 8.0);
  EXPECT_TRUE(iter_bool_pair.second);
  EXPECT_EQ(matrix.size_non_zero(), 5);
  EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
  EXPECT_DOUBLE_EQ(matrix[2][4], 5.0);
  EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);
  EXPECT_DOUBLE_EQ(matrix[4][0], 8.0);

  iter_bool_pair = matrix.insert(4, 4, -5); // last row and column test
  EXPECT_DOUBLE_EQ(*iter_bool_pair.first, -5.0);
  EXPECT_TRUE(iter_bool_pair.second);
  EXPECT_EQ(matrix.size_non_zero(), 6);
  EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
  EXPECT_DOUBLE_EQ(matrix[2][4], 5.0);
  EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);
  EXPECT_DOUBLE_EQ(matrix[4][0], 8.0);
  EXPECT_DOUBLE_EQ(matrix[4][4], -5.0);

  iter_bool_pair = matrix.insert(3, 2, 2.0); // no insert test
  EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 1.0);
  EXPECT_FALSE(iter_bool_pair.second);
  EXPECT_EQ(matrix.size_non_zero(), 6);
  EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
  EXPECT_DOUBLE_EQ(matrix[2][4], 5.0);
  EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);
  EXPECT_DOUBLE_EQ(matrix[4][0], 8.0);
  EXPECT_DOUBLE_EQ(matrix[4][4], -5.0);

  iter_bool_pair = matrix.insert(6, 2, 10.0); // extra rows
  EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 10.0);
  EXPECT_TRUE(iter_bool_pair.second);
  EXPECT_EQ(matrix.size_row(), 7);
  EXPECT_EQ(matrix.size_non_zero(), 7);
  EXPECT_DOUBLE_EQ(matrix[6][2], 10.0);

  iter_bool_pair = matrix.insert(2, 6, 50.0); // extra columns
  EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 50.0);
  EXPECT_TRUE(iter_bool_pair.second);
  EXPECT_EQ(matrix.size_row(), 7);
  EXPECT_EQ(matrix.size_column(), 7);
  EXPECT_EQ(matrix.size_non_zero(), 8);
  EXPECT_DOUBLE_EQ(matrix[2][6], 50.0);

  iter_bool_pair = matrix.insert_or_assign(2, 6, -50.0); // check insert_or_assign
  EXPECT_DOUBLE_EQ(*iter_bool_pair.first, -50.0);
  EXPECT_FALSE(iter_bool_pair.second);
  EXPECT_EQ(matrix.size_row(), 7);
  EXPECT_EQ(matrix.size_column(), 7);
  EXPECT_EQ(matrix.size_non_zero(), 8);
  EXPECT_DOUBLE_EQ(matrix[2][6], -50.0);

  // check ascending order has been maintained.
  FOR_EACH(row_vector, matrix) {
    auto prev = row_vector.begin();
    FOR_ITER(iter, row_vector) {
      if(iter != row_vector.begin()) {
        EXPECT_GT(iter.i_column(), prev.i_column());
        EXPECT_EQ(iter.i_row(), prev.i_row()); // more of a sanity check.
        ++prev;
      }
    }
  }
}

TEST(test_matrix_sparse_row, erase) {
  Matrix_Sparse matrix({0, 2, 3}, {1, 0, 0}, {1.0, 2.0, 3.0}, 2);
  auto iter = matrix.erase(matrix.find(0, 1));
  EXPECT_EQ(matrix.size_row(), 2);
  EXPECT_EQ(matrix.size_column(), 2);
  EXPECT_EQ(matrix.size_non_zero(), 2);
  EXPECT_TRUE(iter == matrix[0].end()); // should point to end
  EXPECT_DOUBLE_EQ(*iter, 3.0); // end should be the next row over.
  EXPECT_DOUBLE_EQ(matrix[0][0], 2.0);
  EXPECT_DOUBLE_EQ(matrix[1][0], 3.0);

  // Check debug undefined behaviour
  EXPECT_DEATH(matrix.erase(matrix.find(3, 0)), "./*");
  EXPECT_DEATH(matrix.erase(matrix.find(0, 2)), "./*");
  EXPECT_DEATH(matrix.erase(matrix.find(1, 1)), "./*");
}

TEST(test_matrix_sparse_row, resize) {
  Matrix_Sparse matrix;

  // check row and column expansion
  matrix.resize(5, 9);
  EXPECT_FALSE(matrix.empty());
  EXPECT_EQ(matrix.size_row(), 5);
  EXPECT_EQ(matrix.size_column(), 9);
  EXPECT_EQ(matrix.size_non_zero(), 0);
  EXPECT_EQ(matrix.size().first, std::make_pair(5, 9).first);
  EXPECT_EQ(matrix.size().second, std::make_pair(5, 9).second);

  // check row reduction
  matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  matrix.resize(3, 5);
  EXPECT_EQ(matrix.size_row(), 3);
  EXPECT_EQ(matrix.size_column(), 5);
  EXPECT_EQ(matrix.size_non_zero(), 5);

  // check column reduction
  matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  matrix.resize(4, 2);
  EXPECT_EQ(matrix.size_row(), 4);
  EXPECT_EQ(matrix.size_column(), 2);
  EXPECT_EQ(matrix.size_non_zero(), 2);
  EXPECT_EQ(matrix[0][1], 1.0);
  EXPECT_EQ(matrix[1][0], 4.0);

  // check row reduction with column expansion
  matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  matrix.resize(2, 8);
  EXPECT_EQ(matrix.size_row(), 2);
  EXPECT_EQ(matrix.size_column(), 8);
  EXPECT_EQ(matrix.size_non_zero(), 5);
  EXPECT_EQ(matrix[0][1], 1.0);
  EXPECT_EQ(matrix[0][3], 2.0);
  EXPECT_EQ(matrix[1][0], 4.0);
  EXPECT_EQ(matrix[1][2], 3.0);
  EXPECT_EQ(matrix[1][3], 5.0);

  // check row expansion with column reduction
  matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  matrix.resize(10, 2);
  EXPECT_EQ(matrix.size_row(), 10);
  EXPECT_EQ(matrix.size_column(), 2);
  EXPECT_EQ(matrix.size_non_zero(), 2);
  EXPECT_EQ(matrix[0][1], 1.0);
  EXPECT_EQ(matrix[1][0], 4.0);

  // check row and column reduction
  matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  matrix.resize(1, 2);
  EXPECT_EQ(matrix.size_row(), 1);
  EXPECT_EQ(matrix.size_column(), 2);
  EXPECT_EQ(matrix.size_non_zero(), 1);
  EXPECT_EQ(matrix[0][1], 1.0);

  // check zero sizing.
  matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  matrix.resize(0, 0);
  EXPECT_EQ(matrix.size_row(), 0);
  EXPECT_EQ(matrix.size_column(), 0);
  EXPECT_EQ(matrix.size_non_zero(), 0);

  EXPECT_DEATH(matrix.resize(0, 1), "./*");
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

//--------------------------------------------------------------------------------------------------------------------
// Lookup
//--------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse_row, find) {
  Matrix_Sparse matrix({0, 1, 3, 3}, {1, 0, 2}, {3.0, 4.0, 5.0}, 3);
  EXPECT_EQ(matrix.find(0, 0), matrix[0].end());
  EXPECT_EQ(matrix.find(0, 1).i_row(), 0);
  EXPECT_EQ(matrix.find(0, 1).i_column(), 1);
  EXPECT_EQ(matrix.find(0, 2), matrix[0].end());

  EXPECT_EQ(matrix.find(1, 0).i_row(), 1);
  EXPECT_EQ(matrix.find(1, 0).i_column(), 0);
  EXPECT_EQ(matrix.find(1, 1), matrix[1].end());
  EXPECT_EQ(matrix.find(1, 2).i_row(), 1);
  EXPECT_EQ(matrix.find(1, 2).i_column(), 2);

  EXPECT_EQ(matrix.find(2, 1), matrix[2].end());
  EXPECT_EQ(matrix.find(2, 2), matrix[2].end());
  EXPECT_EQ(matrix.find(2, 3), matrix[2].end());
  EXPECT_EQ(matrix.find(3, 3), matrix.end()->end());

  const Matrix_Sparse matrix_const({0, 1, 3, 3}, {1, 0, 2}, {3.0, 4.0, 5.0}, 3);
  EXPECT_EQ(matrix_const.find(0, 0), matrix_const[0].end());
  EXPECT_EQ(matrix_const.find(0, 1).i_row(), 0);
  EXPECT_EQ(matrix_const.find(0, 1).i_column(), 1);
  EXPECT_EQ(matrix_const.find(0, 2), matrix_const[0].end());

  EXPECT_EQ(matrix_const.find(1, 0).i_row(), 1);
  EXPECT_EQ(matrix_const.find(1, 0).i_column(), 0);
  EXPECT_EQ(matrix_const.find(1, 1), matrix_const[1].end());
  EXPECT_EQ(matrix_const.find(1, 2).i_row(), 1);
  EXPECT_EQ(matrix_const.find(1, 2).i_column(), 2);

  EXPECT_EQ(matrix_const.find(2, 1), matrix_const[2].end());
  EXPECT_EQ(matrix_const.find(2, 2), matrix_const[2].end());
  EXPECT_EQ(matrix_const.find(2, 3), matrix_const[2].end());
  EXPECT_EQ(matrix_const.find(3, 3), (*matrix_const.end()).end());
}

TEST(test_matrix_sparse_row, contains) {
  Matrix_Sparse matrix({0, 1, 2, 3}, {1, 2, 1}, {3.0, 4.0, 5.0}, 3);
  EXPECT_TRUE(matrix.contains(0, 1));
  EXPECT_TRUE(matrix.contains(1, 2));
  EXPECT_FALSE(matrix.contains(0, 0));
  EXPECT_FALSE(matrix.contains(2, 0));
}

TEST(test_matrix_sparse_row, lower_bound) {
  Matrix_Sparse matrix({0, 1, 3, 3}, {1, 0, 2}, {3.0, 4.0, 5.0}, 3);
  EXPECT_EQ(matrix.lower_bound(0, 0).i_row(), 0);
  EXPECT_EQ(matrix.lower_bound(0, 0).i_column(), 1);
  EXPECT_EQ(matrix.lower_bound(0, 1).i_row(), 0);
  EXPECT_EQ(matrix.lower_bound(0, 1).i_column(), 1);
  EXPECT_EQ(matrix.lower_bound(0, 2), matrix[0].end());

  EXPECT_EQ(matrix.lower_bound(1, 0).i_row(), 1);
  EXPECT_EQ(matrix.lower_bound(1, 0).i_column(), 0);
  EXPECT_EQ(matrix.lower_bound(1, 1).i_row(), 1);
  EXPECT_EQ(matrix.lower_bound(1, 1).i_column(), 2);
  EXPECT_EQ(matrix.lower_bound(1, 2).i_row(), 1);
  EXPECT_EQ(matrix.lower_bound(1, 2).i_column(), 2);

  EXPECT_EQ(matrix.lower_bound(2, 0), matrix[2].end());
  EXPECT_EQ(matrix.lower_bound(3, 3), matrix.end()->end());

  const Matrix_Sparse matrix_const({0, 1, 3, 3}, {1, 0, 2}, {3.0, 4.0, 5.0}, 3);
  EXPECT_EQ(matrix_const.lower_bound(0, 0).i_row(), 0);
  EXPECT_EQ(matrix_const.lower_bound(0, 0).i_column(), 1);
  EXPECT_EQ(matrix_const.lower_bound(0, 1).i_row(), 0);
  EXPECT_EQ(matrix_const.lower_bound(0, 1).i_column(), 1);
  EXPECT_EQ(matrix_const.lower_bound(0, 2), matrix_const[0].end());

  EXPECT_EQ(matrix_const.lower_bound(1, 0).i_row(), 1);
  EXPECT_EQ(matrix_const.lower_bound(1, 0).i_column(), 0);
  EXPECT_EQ(matrix_const.lower_bound(1, 1).i_row(), 1);
  EXPECT_EQ(matrix_const.lower_bound(1, 1).i_column(), 2);
  EXPECT_EQ(matrix_const.lower_bound(1, 2).i_row(), 1);
  EXPECT_EQ(matrix_const.lower_bound(1, 2).i_column(), 2);

  EXPECT_EQ(matrix_const.lower_bound(2, 0), matrix_const[2].end());
  EXPECT_EQ(matrix_const.lower_bound(3, 3), (*matrix_const.end()).end());
}

//----------------------------------------------------------------------------------------------------------------------
// Stand Alone Sparse Matrix Operators
//----------------------------------------------------------------------------------------------------------------------
