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

#include "gtest/gtest.h"
#include "matrix_sparse.hpp"

using namespace Disa;

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse
// ---------------------------------------------------------------------------------------------------------------------
// Member functions
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, constructors_row_column) {
  Matrix_Sparse matrix(2, 4);
  EXPECT_EQ(matrix.size_row(), 2);
  EXPECT_EQ(matrix.size_column(), 4);
}

TEST(test_matrix_sparse, constructors_initialiser_lists) {
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

TEST(test_matrix_sparse, operator_assignment) {
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

// ---------------------------------------------------------------------------------------------------------------------
// Element Access
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, at) {
  Matrix_Sparse matrix_non_const({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  EXPECT_DOUBLE_EQ(matrix_non_const.at(3, 4), 6.0);
  matrix_non_const.at(3, 4) = -10.0;
  EXPECT_DOUBLE_EQ(matrix_non_const.at(3, 4), -10.0);
  EXPECT_DEATH(double x = matrix_non_const.at(10, 4), "./*");
  EXPECT_DEATH(double x = matrix_non_const.at(0, 6), "./*");
  EXPECT_DEATH(double x = matrix_non_const.at(10, 2), "./*");

  const Matrix_Sparse matrix_const({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  EXPECT_DOUBLE_EQ(matrix_const.at(3, 4), 6.0);
  EXPECT_DEATH(double x = matrix_const.at(10, 4), "./*");
  EXPECT_DEATH(double x = matrix_const.at(0, 6), "./*");
  EXPECT_DEATH(double x = matrix_const.at(10, 2), "./*");
}

TEST(test_matrix_sparse, operator_subscript) {
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

// ---------------------------------------------------------------------------------------------------------------------
// Iterators
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, begin) {
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

TEST(test_matrix_sparse, end) {
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

// -------------------------------------------------------------------------------------------------------------------
// Capacity
// -------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, empty) {
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

TEST(test_matrix_sparse, size) {
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

TEST(test_matrix_sparse, reserve_capacity) {
  Matrix_Sparse matrix;
  EXPECT_EQ(matrix.capacity().first, 0);
  EXPECT_EQ(matrix.capacity().second, 0);

  matrix.reserve(10, 40);
  EXPECT_EQ(matrix.capacity().first, 11);
  EXPECT_EQ(matrix.capacity().second, 40);
}

TEST(test_matrix_sparse, shrink_to_fit) {
  Matrix_Sparse matrix;
  matrix.reserve(10, 40);
  EXPECT_EQ(matrix.capacity().first, 11);
  EXPECT_EQ(matrix.capacity().second, 40);

  matrix.resize(5, 30);
  matrix[4][8] = 10;
  matrix.shrink_to_fit();
  EXPECT_EQ(matrix.capacity().first, 6);
  EXPECT_EQ(matrix.capacity().second, 1);
}

// -------------------------------------------------------------------------------------------------------------------
// Modifiers
// -------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, clear) {
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

TEST(test_matrix_sparse, insert_insert_or_assign) {
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

TEST(test_matrix_sparse, erase) {
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

TEST(test_matrix_sparse, resize) {
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
}

TEST(test_matrix_sparse, swap) {
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

// -------------------------------------------------------------------------------------------------------------------
// Lookup
// -------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, find) {
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

TEST(test_matrix_sparse, contains) {
  Matrix_Sparse matrix({0, 1, 2, 3}, {1, 2, 1}, {3.0, 4.0, 5.0}, 3);
  EXPECT_TRUE(matrix.contains(0, 1));
  EXPECT_TRUE(matrix.contains(1, 2));
  EXPECT_FALSE(matrix.contains(0, 0));
  EXPECT_FALSE(matrix.contains(2, 0));

  // edge cases
  matrix = Matrix_Sparse({0, 0, 0}, {}, {}, 2);
  EXPECT_FALSE(matrix.contains(0, 0));
  EXPECT_FALSE(matrix.contains(0, 1));
  EXPECT_FALSE(matrix.contains(1, 0));
  EXPECT_FALSE(matrix.contains(1, 1));
}

TEST(test_matrix_sparse, lower_bound) {
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

// ---------------------------------------------------------------------------------------------------------------------
// Mathematical Assignment Operators
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, operator_scalar_multiplication_assignement){
  Matrix_Sparse matrix({0, 1, 3, 3}, {1, 0, 2}, {3.0, -4.0, 5.0}, 3);
  matrix *= -10.0;

  EXPECT_EQ(matrix.size_row(), 3);
  EXPECT_EQ(matrix.size_column(), 3);
  EXPECT_EQ(matrix.size_non_zero(), 3);
  EXPECT_DOUBLE_EQ(matrix[0][1], -30.0);
  EXPECT_DOUBLE_EQ(matrix[1][0], 40.0);
  EXPECT_DOUBLE_EQ(matrix[1][2], -50.0);
}

TEST(test_matrix_sparse, operator_scalar_division_assignement) {
  Matrix_Sparse matrix({0, 1, 3, 3}, {1, 0, 2}, {3.0, -4.0, 5.0}, 3);
  matrix /= -10.0;

  EXPECT_EQ(matrix.size_row(), 3);
  EXPECT_EQ(matrix.size_column(), 3);
  EXPECT_EQ(matrix.size_non_zero(), 3);
  EXPECT_DOUBLE_EQ(matrix[0][1], -0.30);
  EXPECT_DOUBLE_EQ(matrix[1][0], 0.4);
  EXPECT_DOUBLE_EQ(matrix[1][2], -0.5);
}

TEST(test_matrix_sparse, operator_matrix_addition_assignement){
  Matrix_Sparse identity({0, 1, 2, 3}, {0, 1, 2}, {1.0, 1.0, 1.0}, 3);
  Matrix_Sparse matrix({0, 1, 2, 3}, {2, 1, 0}, {3.0, -4.0, 5.0}, 3);
  matrix += identity;

  EXPECT_EQ(identity.size_row(), 3);
  EXPECT_EQ(identity.size_column(), 3);
  EXPECT_EQ(identity.size_non_zero(), 3);

  EXPECT_EQ(matrix.size_row(), 3);
  EXPECT_EQ(matrix.size_column(), 3);
  EXPECT_EQ(matrix.size_non_zero(), 5);
  EXPECT_DOUBLE_EQ(matrix[0][0], 1.0);
  EXPECT_DOUBLE_EQ(matrix[0][2], 3.0);
  EXPECT_DOUBLE_EQ(matrix[1][1], -3.0);
  EXPECT_DOUBLE_EQ(matrix[2][0], 5.0);
  EXPECT_DOUBLE_EQ(matrix[2][2], 1.0);

  Matrix_Sparse matrix_wrong_size({0, 1, 2, 3}, {2, 1, 0}, {3.0, -4.0, 5.0}, 4);
  EXPECT_DEATH(matrix += matrix_wrong_size, "./*");
}

TEST(test_matrix_sparse, operator_matrix_subraction_assignement){
  Matrix_Sparse identity({0, 1, 2, 3}, {0, 1, 2}, {1.0, 1.0, 1.0}, 3);
  Matrix_Sparse matrix({0, 1, 2, 3}, {2, 1, 0}, {3.0, -4.0, 5.0}, 3);
  matrix -= identity;

  EXPECT_EQ(identity.size_row(), 3);
  EXPECT_EQ(identity.size_column(), 3);
  EXPECT_EQ(identity.size_non_zero(), 3);

  EXPECT_EQ(matrix.size_row(), 3);
  EXPECT_EQ(matrix.size_column(), 3);
  EXPECT_EQ(matrix.size_non_zero(), 5);
  EXPECT_DOUBLE_EQ(matrix[0][0], -1.0);
  EXPECT_DOUBLE_EQ(matrix[0][2], 3.0);
  EXPECT_DOUBLE_EQ(matrix[1][1], -5.0);
  EXPECT_DOUBLE_EQ(matrix[2][0], 5.0);
  EXPECT_DOUBLE_EQ(matrix[2][2], -1.0);

  Matrix_Sparse matrix_wrong_size({0, 1, 2, 3}, {2, 1, 0}, {3.0, -4.0, 5.0}, 4);
  EXPECT_DEATH(matrix -= matrix_wrong_size, "./*");
}

TEST(test_matrix_sparse, operator_matrix_multiplication_assignement){
  Matrix_Sparse matrix_0({0, 2, 3, 4, 6, 7}, {0, 2, 2, 0, 1, 4, 3}, {1.0, 2.0, -7.0, 4.0, 3.0, -9.0, 5.0}, 5);
  Matrix_Sparse matrix_1({0, 2, 4, 5, 6, 7}, {1, 4, 0, 3, 1, 4, 2}, {6.0, 9.0, 1.0, -8.0, 6.0, 5.0, -7.0}, 5);

  matrix_0 *= matrix_1;
  EXPECT_EQ(matrix_1.size_row(), 5);
  EXPECT_EQ(matrix_1.size_column(), 5);
  EXPECT_EQ(matrix_1.size_non_zero(), 7);

  EXPECT_EQ(matrix_0.size_row(), 5);
  EXPECT_EQ(matrix_0.size_column(), 5);
  EXPECT_EQ(matrix_0.size_non_zero(), 9);

  EXPECT_DOUBLE_EQ(matrix_0[0][1], 18.0);
  EXPECT_DOUBLE_EQ(matrix_0[0][4], 9.0);
  EXPECT_DOUBLE_EQ(matrix_0[1][1], -42.0);
  EXPECT_DOUBLE_EQ(matrix_0[2][1], 24.0);
  EXPECT_DOUBLE_EQ(matrix_0[2][4], 36.0);
  EXPECT_DOUBLE_EQ(matrix_0[3][0], 3.0);
  EXPECT_DOUBLE_EQ(matrix_0[3][2], 63.0);
  EXPECT_DOUBLE_EQ(matrix_0[3][3], -24.0);
  EXPECT_DOUBLE_EQ(matrix_0[4][4], 25.0);

  // reset
  matrix_0 = Matrix_Sparse({0, 2, 3, 4, 6}, {0, 2, 0, 1, 1, 2}, {2.0, 8.0, -6.0, -3.0, 4.0, 7.0}, 3);
  matrix_1 = Matrix_Sparse({0, 1, 2, 3}, {0, 1, 0}, {6.0, 4.0, -5.0}, 2);
  matrix_0 *= matrix_1;

  EXPECT_EQ(matrix_1.size_row(), 3);
  EXPECT_EQ(matrix_1.size_column(), 2);
  EXPECT_EQ(matrix_1.size_non_zero(), 3);

  EXPECT_EQ(matrix_0.size_row(), 4);
  EXPECT_EQ(matrix_0.size_column(), 2);
  EXPECT_EQ(matrix_0.size_non_zero(), 5);
  EXPECT_DOUBLE_EQ(matrix_0[0][0], -28.0);
  EXPECT_DOUBLE_EQ(matrix_0[1][0], -36.0);
  EXPECT_DOUBLE_EQ(matrix_0[2][1], -12.0);
  EXPECT_DOUBLE_EQ(matrix_0[3][0], -35.0);
  EXPECT_DOUBLE_EQ(matrix_0[3][1], 16.0);

  // reset
  matrix_0 = Matrix_Sparse({0, 2, 3, 4, 6}, {0, 2, 0, 1, 1, 2}, {2.0, 8.0, -6.0, -3.0, 4.0, 7.0}, 3);
  EXPECT_DEATH(matrix_0 *= matrix_0, "./*");
}

// ---------------------------------------------------------------------------------------------------------------------
// Stand Alone Sparse Matrix Operators
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, scalar_matrix_multiplication) {
  Matrix_Sparse matrix({0, 1, 3, 3}, {1, 0, 2}, {3.0, -4.0, 5.0}, 3);
  Matrix_Sparse matrix_result = -10.0*matrix;

  EXPECT_EQ(matrix_result.size_row(), 3);
  EXPECT_EQ(matrix_result.size_column(), 3);
  EXPECT_EQ(matrix_result.size_non_zero(), 3);
  EXPECT_DOUBLE_EQ(matrix_result[0][1], -30.0);
  EXPECT_DOUBLE_EQ(matrix_result[1][0], 40.0);
  EXPECT_DOUBLE_EQ(matrix_result[1][2], -50.0);
}

TEST(test_matrix_sparse, scalar_matrix_division) {
  Matrix_Sparse matrix({0, 1, 3, 3}, {1, 0, 2}, {3.0, -4.0, 5.0}, 3);
  Matrix_Sparse matrix_result = matrix/10.0;

  EXPECT_EQ(matrix_result.size_row(), 3);
  EXPECT_EQ(matrix_result.size_column(), 3);
  EXPECT_EQ(matrix_result.size_non_zero(), 3);
  EXPECT_DOUBLE_EQ(matrix_result[0][1], 0.30);
  EXPECT_DOUBLE_EQ(matrix_result[1][0], -0.4);
  EXPECT_DOUBLE_EQ(matrix_result[1][2], 0.5);
}

TEST(test_matrix_sparse, matrix_vector_multiplication) {
  Matrix_Sparse matrix({0, 1, 4}, {1, 0, 1, 2}, {3.0, -4.0, 5.0, -2.0}, 3);
  Vector_Dense<Scalar, 0> dynamic_vector = {-1.0, 2.0, 3.0};
  Vector_Dense<Scalar, 3> static_vector = {-10.0, 20.0, -30};

  Vector_Dense<Scalar, 0> result_dynamic = matrix*dynamic_vector;
  EXPECT_EQ(result_dynamic.size(), 2);
  EXPECT_DOUBLE_EQ(result_dynamic[0], 6.0);
  EXPECT_DOUBLE_EQ(result_dynamic[1], 8.0);

  matrix.resize(3, 3);
  Vector_Dense<Scalar, 3> result_static = matrix*static_vector;
  EXPECT_DOUBLE_EQ(result_static[0], 60.0);
  EXPECT_DOUBLE_EQ(result_static[1], 200.0);
  EXPECT_DOUBLE_EQ(result_static[2], 0.0);

  Matrix_Sparse dynamic_matrix_incorrect;
  Vector_Dense<Scalar, 0> dynamic_vector_incorrect;
  Vector_Dense<Scalar, 2> static_vector_incorrect;
  EXPECT_DEATH(dynamic_matrix_incorrect*dynamic_vector, "./*");
  EXPECT_DEATH(dynamic_matrix_incorrect*static_vector_incorrect, "./*");
  dynamic_matrix_incorrect.resize(4, 2);
  EXPECT_DEATH(matrix*static_vector_incorrect, "./*");
}

TEST(test_matrix_sparse, matrix_matrix_addition) {
  Matrix_Sparse identity({0, 1, 2, 3}, {0, 1, 2}, {1.0, 1.0, 1.0}, 3);
  Matrix_Sparse matrix({0, 1, 2, 3}, {2, 1, 0}, {3.0, -4.0, 5.0}, 3);
  Matrix_Sparse matrix_result;
  matrix_result = identity + matrix;

  EXPECT_EQ(identity.size_row(), 3);
  EXPECT_EQ(identity.size_column(), 3);
  EXPECT_EQ(identity.size_non_zero(), 3);
  EXPECT_DOUBLE_EQ(identity[0][0], 1.0);
  EXPECT_DOUBLE_EQ(identity[1][1], 1.0);
  EXPECT_DOUBLE_EQ(identity[2][2], 1.0);

  EXPECT_EQ(matrix.size_row(), 3);
  EXPECT_EQ(matrix.size_column(), 3);
  EXPECT_EQ(matrix.size_non_zero(), 3);
  EXPECT_DOUBLE_EQ(matrix[0][2], 3.0);
  EXPECT_DOUBLE_EQ(matrix[1][1], -4.0);
  EXPECT_DOUBLE_EQ(matrix[2][0], 5.0);

  EXPECT_EQ(matrix_result.size_row(), 3);
  EXPECT_EQ(matrix_result.size_column(), 3);
  EXPECT_EQ(matrix_result.size_non_zero(), 5);
  EXPECT_DOUBLE_EQ(matrix_result[0][0], 1.0);
  EXPECT_DOUBLE_EQ(matrix_result[0][2], 3.0);
  EXPECT_DOUBLE_EQ(matrix_result[1][1], -3.0);
  EXPECT_DOUBLE_EQ(matrix_result[2][0], 5.0);
  EXPECT_DOUBLE_EQ(matrix_result[2][2], 1.0);

  Matrix_Sparse matrix_wrong_size({0, 1, 2, 3}, {2, 1, 0}, {3.0, -4.0, 5.0}, 4);
  EXPECT_DEATH(matrix + matrix_wrong_size, "./*");
}

TEST(test_matrix_sparse, matrix_matrix_subtraction) {
  Matrix_Sparse identity({0, 1, 2, 3}, {0, 1, 2}, {1.0, 1.0, 1.0}, 3);
  Matrix_Sparse matrix({0, 1, 2, 3}, {2, 1, 0}, {3.0, -4.0, 5.0}, 3);
  Matrix_Sparse matrix_result;
  matrix_result = matrix - identity;

  EXPECT_EQ(identity.size_row(), 3);
  EXPECT_EQ(identity.size_column(), 3);
  EXPECT_EQ(identity.size_non_zero(), 3);
  EXPECT_DOUBLE_EQ(identity[0][0], 1.0);
  EXPECT_DOUBLE_EQ(identity[1][1], 1.0);
  EXPECT_DOUBLE_EQ(identity[2][2], 1.0);

  EXPECT_EQ(matrix.size_row(), 3);
  EXPECT_EQ(matrix.size_column(), 3);
  EXPECT_EQ(matrix.size_non_zero(), 3);
  EXPECT_DOUBLE_EQ(matrix[0][2], 3.0);
  EXPECT_DOUBLE_EQ(matrix[1][1], -4.0);
  EXPECT_DOUBLE_EQ(matrix[2][0], 5.0);

  EXPECT_EQ(matrix_result.size_row(), 3);
  EXPECT_EQ(matrix_result.size_column(), 3);
  EXPECT_EQ(matrix_result.size_non_zero(), 5);
  EXPECT_DOUBLE_EQ(matrix_result[0][0], -1.0);
  EXPECT_DOUBLE_EQ(matrix_result[0][2], 3.0);
  EXPECT_DOUBLE_EQ(matrix_result[1][1], -5.0);
  EXPECT_DOUBLE_EQ(matrix_result[2][0], 5.0);
  EXPECT_DOUBLE_EQ(matrix_result[2][2], -1.0);

  Matrix_Sparse matrix_wrong_size({0, 1, 2, 3}, {2, 1, 0}, {3.0, -4.0, 5.0}, 4);
  EXPECT_DEATH(matrix - matrix_wrong_size, "./*");
}

TEST(test_matrix_sparse, matrix_matrix_multiplication) {
  Matrix_Sparse matrix_0({0, 2, 3, 4, 6, 7}, {0, 2, 2, 0, 1, 4, 3}, {1.0, 2.0, -7.0, 4.0, 3.0, -9.0, 5.0}, 5);
  Matrix_Sparse matrix_1({0, 2, 4, 5, 6, 7}, {1, 4, 0, 3, 1, 4, 2}, {6.0, 9.0, 1.0, -8.0, 6.0, 5.0, -7.0}, 5);
  Matrix_Sparse matrix_result;
  matrix_result = matrix_0*matrix_1;

  EXPECT_EQ(matrix_0.size_row(), 5);
  EXPECT_EQ(matrix_0.size_column(), 5);
  EXPECT_EQ(matrix_0.size_non_zero(), 7);

  EXPECT_EQ(matrix_1.size_row(), 5);
  EXPECT_EQ(matrix_1.size_column(), 5);
  EXPECT_EQ(matrix_1.size_non_zero(), 7);

  EXPECT_EQ(matrix_result.size_row(), 5);
  EXPECT_EQ(matrix_result.size_column(), 5);
  EXPECT_EQ(matrix_result.size_non_zero(), 9);
  EXPECT_DOUBLE_EQ(matrix_result[0][1], 18.0);
  EXPECT_DOUBLE_EQ(matrix_result[0][4], 9.0);
  EXPECT_DOUBLE_EQ(matrix_result[1][1], -42.0);
  EXPECT_DOUBLE_EQ(matrix_result[2][1], 24.0);
  EXPECT_DOUBLE_EQ(matrix_result[2][4], 36.0);
  EXPECT_DOUBLE_EQ(matrix_result[3][0], 3.0);
  EXPECT_DOUBLE_EQ(matrix_result[3][2], 63.0);
  EXPECT_DOUBLE_EQ(matrix_result[3][3], -24.0);
  EXPECT_DOUBLE_EQ(matrix_result[4][4], 25.0);

  // reset
  matrix_0 = Matrix_Sparse({0, 2, 3, 4, 6}, {0, 2, 0, 1, 1, 2}, {2.0, 8.0, -6.0, -3.0, 4.0, 7.0}, 3);
  matrix_1 = Matrix_Sparse({0, 1, 2, 3}, {0, 1, 0}, {6.0, 4.0, -5.0}, 2);
  matrix_result = matrix_0*matrix_1;

  EXPECT_EQ(matrix_0.size_row(), 4);
  EXPECT_EQ(matrix_0.size_column(), 3);
  EXPECT_EQ(matrix_0.size_non_zero(), 6);

  EXPECT_EQ(matrix_1.size_row(), 3);
  EXPECT_EQ(matrix_1.size_column(), 2);
  EXPECT_EQ(matrix_1.size_non_zero(), 3);

  EXPECT_EQ(matrix_result.size_row(), 4);
  EXPECT_EQ(matrix_result.size_column(), 2);
  EXPECT_EQ(matrix_result.size_non_zero(), 5);
  EXPECT_DOUBLE_EQ(matrix_result[0][0], -28.0);
  EXPECT_DOUBLE_EQ(matrix_result[1][0], -36.0);
  EXPECT_DOUBLE_EQ(matrix_result[2][1], -12.0);
  EXPECT_DOUBLE_EQ(matrix_result[3][0], -35.0);
  EXPECT_DOUBLE_EQ(matrix_result[3][1], 16.0);

  // reset
  matrix_0 = Matrix_Sparse({0, 2, 3, 4, 6}, {0, 2, 0, 1, 1, 2}, {2.0, 8.0, -6.0, -3.0, 4.0, 7.0}, 3);
  EXPECT_DEATH(matrix_0*matrix_0, "./*");
}