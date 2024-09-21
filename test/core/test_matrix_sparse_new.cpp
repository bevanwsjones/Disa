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
#include "matrix_sparse_new.hpp"

using namespace Disa;

TEST(MatrixSparseElementTest, DefaultConstructor) {
  Matrix_Sparse_Element<int, int> element;
  // Add assertions as necessary, depending on what your default constructor does
}

TEST(MatrixSparseElementTest, ValueConstructor) {
  int row = 1;
  int column = 2;
  int value = 3;
  Matrix_Sparse_Element<int, int> element(row, &column, &value);
  ASSERT_EQ(element.row(), row);
  ASSERT_EQ(element.column(), column);
  ASSERT_EQ(element.value(), value);
}

TEST(MatrixSparseElementTest, IncrementDecrementOperators) {
  int row = 1;
  int column[] = {2, 3};
  int value[] = {4, 5};
  Matrix_Sparse_Element<int, int> element(row, column, value);
  element++;
  ASSERT_EQ(element.column(), column[1]);
  ASSERT_EQ(element.value(), value[1]);
  element--;
  ASSERT_EQ(element.column(), column[0]);
  ASSERT_EQ(element.value(), value[0]);
}

TEST(MatrixSparseElementTest, EqualityOperators) {
  int row1 = 1;
  int column1 = 2;
  int value1 = 3;
  Matrix_Sparse_Element<int, int> element1(row1, &column1, &value1);

  int row2 = 1;
  int column2 = 2;
  int value2 = 3;
  Matrix_Sparse_Element<int, int> element2(row2, &column2, &value2);

  ASSERT_EQ(element1, element2);

  value2 = 4;  // Change the value that element2 points to
  ASSERT_NE(element1, element2);
}

TEST(IteratorMatrixSparseElementTest, DefaultConstructor) {
  Iterator_Matrix_Sparse_Element<int, int> iterator;
  // Add assertions as necessary, depending on what your default constructor does
}

TEST(IteratorMatrixSparseElementTest, ValueConstructor) {
  int row = 1;
  int column = 2;
  int value = 3;
  Matrix_Sparse_Element<int, int> element(row, &column, &value);
  Iterator_Matrix_Sparse_Element<int, int> iterator(element);
  ASSERT_EQ(*iterator, element);
}

TEST(IteratorMatrixSparseElementTest, IncrementDecrementOperators) {
  int row = 1;
  int column[] = {2, 3};
  int value[] = {4, 5};
  Matrix_Sparse_Element<int, int> element1(row, column, value);
  Matrix_Sparse_Element<int, int> element2(row, column, value);
  ++element2;
  Iterator_Matrix_Sparse_Element<int, int> iterator = element1;
  ++iterator;
  ASSERT_EQ(*iterator, element2);
  --iterator;
  ASSERT_EQ(*iterator, element1);
}

TEST(IteratorMatrixSparseElementTest, EqualityOperators) {
  int row = 1;
  int column[] = {2, 3};
  int value[] = {4, 5};
  Matrix_Sparse_Element<int, int> element1(row, column, value);
  Matrix_Sparse_Element<int, int> element2(row, column, value);
  ++element2;

  Iterator_Matrix_Sparse_Element<int, int> iterator1 = element1;
  Iterator_Matrix_Sparse_Element<int, int> iterator2 = element2;
  ASSERT_EQ(iterator1, iterator1);
  ASSERT_NE(iterator1, iterator2);
}

TEST(ConstIteratorMatrixSparseElementTest, DefaultConstructor) {
  Const_Iterator_Matrix_Sparse_Element<int, int> iterator;
  // Add assertions as necessary, depending on what your default constructor does
}

TEST(ConstIteratorMatrixSparseElementTest, ValueConstructor) {
  int row = 1;
  int column[] = {2, 3};
  int value[] = {4, 5};
  Matrix_Sparse_Element<int, int> element(row, column, value);
  Const_Iterator_Matrix_Sparse_Element<int, int> iterator(element);
  ASSERT_EQ(*iterator, element);
}

TEST(ConstIteratorMatrixSparseElementTest, IncrementDecrementOperators) {
  int row = 1;
  int column[] = {2, 3};
  int value[] = {4, 5};
  Matrix_Sparse_Element<int, int> element1(row, column, value);
  Matrix_Sparse_Element<int, int> element2 = element1;
  ++element2;

  Const_Iterator_Matrix_Sparse_Element<int, int> iterator = element1;
  ++iterator;
  ASSERT_EQ(*iterator, element2);
  --iterator;
  ASSERT_EQ(*iterator, element1);
}

TEST(ConstIteratorMatrixSparseElementTest, EqualityOperators) {
  int row = 1;
  int column[] = {2, 3};
  int value[] = {4, 5};
  Matrix_Sparse_Element<int, int> element1(row, column, value);
  Matrix_Sparse_Element<int, int> element2(row, column, value);
  ++element2;

  Const_Iterator_Matrix_Sparse_Element<int, int> iterator1 = element1;
  Const_Iterator_Matrix_Sparse_Element<int, int> iterator2 = element2;
  ASSERT_EQ(iterator1, iterator1);
  ASSERT_NE(iterator1, iterator2);
}

//Row:

TEST(MatrixSparseRowTest, BeginEndIterators) {
  // Create a Matrix_Sparse_Row object
  Matrix_Sparse<int, int> matrix;
  int row = 0;
  std::vector<int> column_data = {1, 2, 3};
  std::vector<int> entry_data = {4, 5, 6};
  std::span<int> column(column_data);
  std::span<int> entries(entry_data);
  Matrix_Sparse_Row<int, int> matrix_row(&matrix, row, column, entries);

  // Test non-const begin() and end()
  auto it = matrix_row.begin();
  auto end = matrix_row.end();
  ASSERT_NE(it, end);  // Assuming the matrix_row is not empty

  // Check that the values returned by the iterator match the original data
  ASSERT_EQ(it->row(), row);
  ASSERT_EQ(it->column(), column_data[0]);
  ASSERT_EQ(it->value(), entry_data[0]);

  // Test const begin() and end()
  const auto& const_matrix_row = matrix_row;
  auto const_it = const_matrix_row.begin();
  auto const_end = const_matrix_row.end();
  ASSERT_NE(const_it, const_end);  // Assuming the matrix_row is not empty

  // Check that the values returned by the const iterator match the original data
  ASSERT_EQ(const_it->row(), row);
  ASSERT_EQ(const_it->column(), column_data[0]);
  ASSERT_EQ(const_it->value(), entry_data[0]);

  // Test cbegin() and cend()
  auto cbegin_it = matrix_row.cbegin();
  auto cend_it = matrix_row.cend();
  ASSERT_NE(cbegin_it, cend_it);  // Assuming the matrix_row is not empty

  // Check that the values returned by the cbegin iterator match the original data
  ASSERT_EQ(cbegin_it->row(), row);
  ASSERT_EQ(cbegin_it->column(), column_data[0]);
  ASSERT_EQ(cbegin_it->value(), entry_data[0]);
}

TEST(MatrixSparseRowTest, ArithmeticOperators) {
  // Create a Matrix_Sparse_Row object
  Matrix_Sparse<int, int> matrix;
  int row = 0;
  std::span<int> column;
  std::span<int> entries;
  Matrix_Sparse_Row<int, int> matrix_row(&matrix, row, column, entries);

  // Test operator++
  auto old_row = matrix_row;
  auto new_row = ++matrix_row;
  ASSERT_EQ(new_row.row(), old_row.row() + 1);

  // Test operator++(int)
  old_row = matrix_row;
  new_row = matrix_row++;
  ASSERT_EQ(new_row.row(), old_row.row());
  ASSERT_EQ(matrix_row.row(), old_row.row() + 1);

  // Test operator--
  old_row = matrix_row;
  new_row = --matrix_row;
  ASSERT_EQ(new_row.row(), old_row.row() - 1);

  // Test operator--(int)
  old_row = matrix_row;
  new_row = matrix_row--;
  ASSERT_EQ(new_row.row(), old_row.row());
  ASSERT_EQ(matrix_row.row(), old_row.row() - 1);
}

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse
// ---------------------------------------------------------------------------------------------------------------------
// Constructors/Destructors
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, constructors_row_column) {
  Matrix_Sparse<Scalar, std::size_t> matrix(2, 4);
  EXPECT_EQ(matrix.size_row(), 2);
  EXPECT_EQ(matrix.size_column(), 4);
}

TEST(test_matrix_sparse, constructors_initialiser_lists) {
  Matrix_Sparse<Scalar, std::size_t> matrix({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                                            5);
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
  EXPECT_DEATH((Matrix_Sparse<Scalar, std::size_t>({0, 2, 3}, {1, 0, 0, 1}, {1.0, 2.0, 3.0, 4.0}, 2)),
               ".*");  // more column indexes than non-zero.back().
  EXPECT_DEATH((Matrix_Sparse<Scalar, std::size_t>({0, 2, 3}, {1, 0, 0}, {1.0, 2.0, 3.0, 4.0}, 2)),
               ".*");  // mismatch column and entry size.
  EXPECT_DEATH((Matrix_Sparse<Scalar, std::size_t>({1, 2, 3}, {1, 0, 0}, {1.0, 2.0, 3.0}, 2)),
               ".*");  // first non-zero value is not 0.
  EXPECT_DEATH((Matrix_Sparse<Scalar, std::size_t>({2, 0, 3}, {1, 0, 0}, {1.0, 2.0, 3.0}, 2)),
               ".*");  // un-ordered non-zeros
  EXPECT_DEATH((Matrix_Sparse<Scalar, std::size_t>({0, 2, 3}, {1, 0, 2}, {1.0, 2.0, 3.0}, 2)),
               ".*");  // column index out of range
  EXPECT_DEATH((Matrix_Sparse<Scalar, std::size_t>({0, 2, 3}, {1, 1, 0}, {1.0, 2.0, 3.0}, 2)),
               ".*");  // repeated column index
}

// ---------------------------------------------------------------------------------------------------------------------
// Element Access
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, operator_subscript) {
  Matrix_Sparse<Scalar, std::size_t> matrix_0({0, 1, 2}, {0, 1}, {1.0, 2.0}, 2);

  EXPECT_DOUBLE_EQ(matrix_0[0][0], 1.0);
  EXPECT_DOUBLE_EQ(matrix_0[1][1], 2.0);
  //EXPECT_TRUE(false);
  //matrix_0[0][1] = 3.0; // this should insert a new element.

  const Matrix_Sparse<Scalar, std::size_t> matrix_1 = matrix_0;
  EXPECT_DOUBLE_EQ(matrix_1[0][0], 1.0);
  // EXPECT_DOUBLE_EQ(matrix_1[0][1], 3.0);
  // EXPECT_DOUBLE_EQ(matrix_1[1][1], 2.0);

  //Check debug for const matrix.
  // EXPECT_DEATH(matrix_1[1][0], "./*");
}

// ---------------------------------------------------------------------------------------------------------------------
// Capacity
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, empty) {
  Matrix_Sparse<Scalar, std::size_t> matrix;
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
  Matrix_Sparse<Scalar, std::size_t> matrix;
  // EXPECT_EQ(matrix.size_row(), 0);
  // EXPECT_EQ(matrix.size_column(), 0);
  // EXPECT_EQ(matrix.size_non_zero(), 0);
  // EXPECT_EQ(matrix.size().first, std::make_pair(0, 0).first);
  // EXPECT_EQ(matrix.size().second, std::make_pair(0, 0).second);

  // matrix.resize(7, 6);
  // // EXPECT_TRUE(false);
  // //  matrix[0][5] = 1.0; // add additional entry - show that non-zero increased.
  // EXPECT_FALSE(matrix.empty());
  // EXPECT_EQ(matrix.size_row(), 7);
  // EXPECT_EQ(matrix.size_column(), 6);
  // EXPECT_EQ(matrix.size_non_zero(), 1);
  // EXPECT_EQ(matrix.size().first, std::make_pair(7, 6).first);
  // EXPECT_EQ(matrix.size().second, std::make_pair(7, 6).second);
}

TEST(test_matrix_sparse, reserve_capacity) {
  // Matrix_Sparse<Scalar, std::size_t> matrix;
  // EXPECT_EQ(matrix.capacity().first, 0);
  // EXPECT_EQ(matrix.capacity().second, 0);

  // matrix.reserve(10, 40);
  // EXPECT_EQ(matrix.capacity().first, 11);
  // EXPECT_EQ(matrix.capacity().second, 40);
}

TEST(test_matrix_sparse, shrink_to_fit) {
  // Matrix_Sparse<Scalar, std::size_t> matrix;
  // matrix.reserve(10, 40);
  // EXPECT_EQ(matrix.capacity().first, 11);
  // EXPECT_EQ(matrix.capacity().second, 40);

  // matrix.resize(5, 30);
  // //EXPECT_TRUE(false);
  // //matrix[4][8] = 10;
  // matrix.shrink_to_fit();
  // EXPECT_EQ(matrix.capacity().first, 6);
  // EXPECT_EQ(matrix.capacity().second, 1);
}

// -------------------------------------------------------------------------------------------------------------------
// Modifiers
// -------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, clear) {
  Matrix_Sparse<Scalar, std::size_t> matrix;
  matrix.resize(7, 6);
  //EXPECT_TRUE(false);
  //matrix[0][5] = 1.0; // add additional entry - show that non-zero increased.

  // matrix.clear();
  // EXPECT_TRUE(matrix.empty());
  // EXPECT_EQ(matrix.size_row(), 0);
  // EXPECT_EQ(matrix.size_column(), 0);
  // EXPECT_EQ(matrix.size_non_zero(), 0);
  // EXPECT_EQ(matrix.size().first, std::make_pair(0, 0).first);
  // EXPECT_EQ(matrix.size().second, std::make_pair(0, 0).second);
  // EXPECT_EQ(matrix.capacity().first, std::make_pair(8, 1).first);    // make sure we have not lost capacity.
  // EXPECT_EQ(matrix.capacity().second, std::make_pair(8, 1).second);  // make sure we have not lost capacity.
}

TEST(test_matrix_sparse, insert_insert_or_assign) {
  // EXPECT_TRUE(false);

  Matrix_Sparse<Scalar, std::size_t> matrix;
  matrix.resize(5, 5);

  //   matrix.insert(3, 2, 1);
  //   EXPECT_EQ(matrix.size_non_zero(), 1);
  //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

  //   auto iter_bool_pair = matrix.insert(3, 1, 3); // out of order testing - will check order at the end.
  //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 3.0);
  //   EXPECT_TRUE(iter_bool_pair.second);
  //   EXPECT_EQ(matrix.size_non_zero(), 2);
  //   EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

  //   iter_bool_pair = matrix.insert(2, 1, 4);
  //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 4.0);
  //   EXPECT_TRUE(iter_bool_pair.second);
  //   EXPECT_DOUBLE_EQ(matrix.size_non_zero(), 3);
  //   EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
  //   EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

  //   iter_bool_pair = matrix.insert(2, 4, 5); // last of columns test
  //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 5.0);
  //   EXPECT_TRUE(iter_bool_pair.second);
  //   EXPECT_EQ(matrix.size_non_zero(), 4);
  //   EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
  //   EXPECT_DOUBLE_EQ(matrix[2][4], 5.0);
  //   EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

  //   iter_bool_pair = matrix.insert(4, 0, 8); // last of rows test
  //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 8.0);
  //   EXPECT_TRUE(iter_bool_pair.second);
  //   EXPECT_EQ(matrix.size_non_zero(), 5);
  //   EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
  //   EXPECT_DOUBLE_EQ(matrix[2][4], 5.0);
  //   EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);
  //   EXPECT_DOUBLE_EQ(matrix[4][0], 8.0);

  //   iter_bool_pair = matrix.insert(4, 4, -5); // last row and column test
  //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, -5.0);
  //   EXPECT_TRUE(iter_bool_pair.second);
  //   EXPECT_EQ(matrix.size_non_zero(), 6);
  //   EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
  //   EXPECT_DOUBLE_EQ(matrix[2][4], 5.0);
  //   EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);
  //   EXPECT_DOUBLE_EQ(matrix[4][0], 8.0);
  //   EXPECT_DOUBLE_EQ(matrix[4][4], -5.0);

  //   iter_bool_pair = matrix.insert(3, 2, 2.0); // no insert test
  //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 1.0);
  //   EXPECT_FALSE(iter_bool_pair.second);
  //   EXPECT_EQ(matrix.size_non_zero(), 6);
  //   EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
  //   EXPECT_DOUBLE_EQ(matrix[2][4], 5.0);
  //   EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
  //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);
  //   EXPECT_DOUBLE_EQ(matrix[4][0], 8.0);
  //   EXPECT_DOUBLE_EQ(matrix[4][4], -5.0);

  //   iter_bool_pair = matrix.insert(6, 2, 10.0); // extra rows
  //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 10.0);
  //   EXPECT_TRUE(iter_bool_pair.second);
  //   EXPECT_EQ(matrix.size_row(), 7);
  //   EXPECT_EQ(matrix.size_non_zero(), 7);
  //   EXPECT_DOUBLE_EQ(matrix[6][2], 10.0);

  //   iter_bool_pair = matrix.insert(2, 6, 50.0); // extra columns
  //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 50.0);
  //   EXPECT_TRUE(iter_bool_pair.second);
  //   EXPECT_EQ(matrix.size_row(), 7);
  //   EXPECT_EQ(matrix.size_column(), 7);
  //   EXPECT_EQ(matrix.size_non_zero(), 8);
  //   EXPECT_DOUBLE_EQ(matrix[2][6], 50.0);

  //   iter_bool_pair = matrix.insert_or_assign(2, 6, -50.0); // check insert_or_assign
  //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, -50.0);
  //   EXPECT_FALSE(iter_bool_pair.second);
  //   EXPECT_EQ(matrix.size_row(), 7);
  //   EXPECT_EQ(matrix.size_column(), 7);
  //   EXPECT_EQ(matrix.size_non_zero(), 8);
  //   EXPECT_DOUBLE_EQ(matrix[2][6], -50.0);

  //   // check ascending order has been maintained.
  //   FOR_EACH(row_vector, matrix) {
  //     auto prev = row_vector.begin();
  //     FOR_ITER(iter, row_vector) {
  //       if(iter != row_vector.begin()) {
  //         EXPECT_GT(iter.i_column(), prev.i_column());
  //         EXPECT_EQ(iter.i_row(), prev.i_row()); // more of a sanity check.
  //         ++prev;
  //       }
  //     }
  //   }
}

TEST(test_matrix_sparse, erase) {
  // EXPECT_TRUE(false);

  //   Matrix_Sparse<Scalar, std::size_t> matrix({0, 2, 3}, {1, 0, 0}, {1.0, 2.0, 3.0}, 2);
  //   auto iter = matrix.erase(matrix.find(0, 1));
  //   EXPECT_EQ(matrix.size_row(), 2);
  //   EXPECT_EQ(matrix.size_column(), 2);
  //   EXPECT_EQ(matrix.size_non_zero(), 2);
  //   EXPECT_TRUE(iter == matrix[0].end()); // should point to end
  //   EXPECT_DOUBLE_EQ(*iter, 3.0); // end should be the next row over.
  //   EXPECT_DOUBLE_EQ(matrix[0][0], 2.0);
  //   EXPECT_DOUBLE_EQ(matrix[1][0], 3.0);

  //   // Check debug undefined behaviour
  //   EXPECT_DEATH(matrix.erase(matrix.find(3, 0)), "./*");
  //   EXPECT_DEATH(matrix.erase(matrix.find(0, 2)), "./*");
  //   EXPECT_DEATH(matrix.erase(matrix.find(1, 1)), "./*");
}

TEST(test_matrix_sparse, resize) {

  // EXPECT_TRUE(false);

  //   Matrix_Sparse<Scalar, std::size_t> matrix;

  //   // check row and column expansion
  //   matrix.resize(5, 9);
  //   EXPECT_FALSE(matrix.empty());
  //   EXPECT_EQ(matrix.size_row(), 5);
  //   EXPECT_EQ(matrix.size_column(), 9);
  //   EXPECT_EQ(matrix.size_non_zero(), 0);
  //   EXPECT_EQ(matrix.size().first, std::make_pair(5, 9).first);
  //   EXPECT_EQ(matrix.size().second, std::make_pair(5, 9).second);

  //   // check row reduction
  //   matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  //   matrix.resize(3, 5);
  //   EXPECT_EQ(matrix.size_row(), 3);
  //   EXPECT_EQ(matrix.size_column(), 5);
  //   EXPECT_EQ(matrix.size_non_zero(), 5);

  //   // check column reduction
  //   matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  //   matrix.resize(4, 2);
  //   EXPECT_EQ(matrix.size_row(), 4);
  //   EXPECT_EQ(matrix.size_column(), 2);
  //   EXPECT_EQ(matrix.size_non_zero(), 2);
  //   EXPECT_EQ(matrix[0][1], 1.0);
  //   EXPECT_EQ(matrix[1][0], 4.0);

  //   // check row reduction with column expansion
  //   matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  //   matrix.resize(2, 8);
  //   EXPECT_EQ(matrix.size_row(), 2);
  //   EXPECT_EQ(matrix.size_column(), 8);
  //   EXPECT_EQ(matrix.size_non_zero(), 5);
  //   EXPECT_EQ(matrix[0][1], 1.0);
  //   EXPECT_EQ(matrix[0][3], 2.0);
  //   EXPECT_EQ(matrix[1][0], 4.0);
  //   EXPECT_EQ(matrix[1][2], 3.0);
  //   EXPECT_EQ(matrix[1][3], 5.0);

  //   // check row expansion with column reduction
  //   matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  //   matrix.resize(10, 2);
  //   EXPECT_EQ(matrix.size_row(), 10);
  //   EXPECT_EQ(matrix.size_column(), 2);
  //   EXPECT_EQ(matrix.size_non_zero(), 2);
  //   EXPECT_EQ(matrix[0][1], 1.0);
  //   EXPECT_EQ(matrix[1][0], 4.0);

  //   // check row and column reduction
  //   matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  //   matrix.resize(1, 2);
  //   EXPECT_EQ(matrix.size_row(), 1);
  //   EXPECT_EQ(matrix.size_column(), 2);
  //   EXPECT_EQ(matrix.size_non_zero(), 1);
  //   EXPECT_EQ(matrix[0][1], 1.0);

  //   // check zero sizing.
  //   matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
  //   matrix.resize(0, 0);
  //   EXPECT_EQ(matrix.size_row(), 0);
  //   EXPECT_EQ(matrix.size_column(), 0);
  //   EXPECT_EQ(matrix.size_non_zero(), 0);
}

TEST(test_matrix_sparse, swap) {

  // EXPECT_TRUE(false);
  //   Matrix_Sparse<Scalar, std::size_t> matrix_0({0, 1, 2}, {1, 0}, {1.0, 2.0}, 2);
  //   Matrix_Sparse<Scalar, std::size_t> matrix_1({0, 1, 2, 3}, {1, 0, 1}, {3.0, 4.0, 5.0}, 3);

  //   matrix_0.swap(matrix_1);
  //   EXPECT_EQ(matrix_0.size_row(), 3);
  //   EXPECT_EQ(matrix_0.size_column(), 3);
  //   EXPECT_EQ(matrix_0.size_non_zero(), 3);
  //   EXPECT_EQ(matrix_1.size_row(), 2);
  //   EXPECT_EQ(matrix_1.size_column(), 2);
  //   EXPECT_EQ(matrix_1.size_non_zero(), 2);
  //   EXPECT_DOUBLE_EQ(matrix_0[0][1], 3.0);
  //   EXPECT_DOUBLE_EQ(matrix_0[1][0], 4.0);
  //   EXPECT_DOUBLE_EQ(matrix_0[2][1], 5.0);
  //   EXPECT_DOUBLE_EQ(matrix_1[0][1], 1.0);
  //   EXPECT_DOUBLE_EQ(matrix_1[1][0], 2.0);
}

// -------------------------------------------------------------------------------------------------------------------
// Lookup
// -------------------------------------------------------------------------------------------------------------------

// TEST(test_matrix_sparse, find) {
//   Matrix_Sparse<Scalar, std::size_t> matrix({0, 1, 3, 3}, {1, 0, 2}, {3.0, 4.0, 5.0}, 3);
//   EXPECT_EQ(matrix.find(0, 0), matrix[0].end());
//   EXPECT_EQ(matrix.find(0, 1).i_row(), 0);
//   EXPECT_EQ(matrix.find(0, 1).i_column(), 1);
//   EXPECT_EQ(matrix.find(0, 2), matrix[0].end());

//   EXPECT_EQ(matrix.find(1, 0).i_row(), 1);
//   EXPECT_EQ(matrix.find(1, 0).i_column(), 0);
//   EXPECT_EQ(matrix.find(1, 1), matrix[1].end());
//   EXPECT_EQ(matrix.find(1, 2).i_row(), 1);
//   EXPECT_EQ(matrix.find(1, 2).i_column(), 2);

//   EXPECT_EQ(matrix.find(2, 1), matrix[2].end());
//   EXPECT_EQ(matrix.find(2, 2), matrix[2].end());
//   EXPECT_EQ(matrix.find(2, 3), matrix[2].end());
//   EXPECT_EQ(matrix.find(3, 3), matrix.end()->end());

//   const Matrix_Sparse<Scalar, std::size_t> matrix_const({0, 1, 3, 3}, {1, 0, 2}, {3.0, 4.0, 5.0}, 3);
//   EXPECT_EQ(matrix_const.find(0, 0), matrix_const[0].end());
//   EXPECT_EQ(matrix_const.find(0, 1).i_row(), 0);
//   EXPECT_EQ(matrix_const.find(0, 1).i_column(), 1);
//   EXPECT_EQ(matrix_const.find(0, 2), matrix_const[0].end());

//   EXPECT_EQ(matrix_const.find(1, 0).i_row(), 1);
//   EXPECT_EQ(matrix_const.find(1, 0).i_column(), 0);
//   EXPECT_EQ(matrix_const.find(1, 1), matrix_const[1].end());
//   EXPECT_EQ(matrix_const.find(1, 2).i_row(), 1);
//   EXPECT_EQ(matrix_const.find(1, 2).i_column(), 2);

//   EXPECT_EQ(matrix_const.find(2, 1), matrix_const[2].end());
//   EXPECT_EQ(matrix_const.find(2, 2), matrix_const[2].end());
//   EXPECT_EQ(matrix_const.find(2, 3), matrix_const[2].end());
//   EXPECT_EQ(matrix_const.find(3, 3), (*matrix_const.end()).end());
// }

// TEST(test_matrix_sparse, contains) {
//   Matrix_Sparse<Scalar, std::size_t> matrix({0, 1, 2, 3}, {1, 2, 1}, {3.0, 4.0, 5.0}, 3);
//   EXPECT_TRUE(matrix.contains(0, 1));
//   EXPECT_TRUE(matrix.contains(1, 2));
//   EXPECT_FALSE(matrix.contains(0, 0));
//   EXPECT_FALSE(matrix.contains(2, 0));

//   // edge cases
//   matrix = Matrix_Sparse<Scalar, std::size_t>({0, 0, 0}, {}, {}, 2);
//   EXPECT_FALSE(matrix.contains(0, 0));
//   EXPECT_FALSE(matrix.contains(0, 1));
//   EXPECT_FALSE(matrix.contains(1, 0));
//   EXPECT_FALSE(matrix.contains(1, 1));
// }

// TEST(test_matrix_sparse, lower_bound) {
//   Matrix_Sparse<Scalar, std::size_t> matrix({0, 1, 3, 3}, {1, 0, 2}, {3.0, 4.0, 5.0}, 3);
//   EXPECT_EQ(matrix.lower_bound(0, 0).i_row(), 0);
//   EXPECT_EQ(matrix.lower_bound(0, 0).i_column(), 1);
//   EXPECT_EQ(matrix.lower_bound(0, 1).i_row(), 0);
//   EXPECT_EQ(matrix.lower_bound(0, 1).i_column(), 1);
//   EXPECT_EQ(matrix.lower_bound(0, 2), matrix[0].end());

//   EXPECT_EQ(matrix.lower_bound(1, 0).i_row(), 1);
//   EXPECT_EQ(matrix.lower_bound(1, 0).i_column(), 0);
//   EXPECT_EQ(matrix.lower_bound(1, 1).i_row(), 1);
//   EXPECT_EQ(matrix.lower_bound(1, 1).i_column(), 2);
//   EXPECT_EQ(matrix.lower_bound(1, 2).i_row(), 1);
//   EXPECT_EQ(matrix.lower_bound(1, 2).i_column(), 2);

//   EXPECT_EQ(matrix.lower_bound(2, 0), matrix[2].end());
//   EXPECT_EQ(matrix.lower_bound(3, 3), matrix.end()->end());

//   const Matrix_Sparse<Scalar, std::size_t> matrix_const({0, 1, 3, 3}, {1, 0, 2}, {3.0, 4.0, 5.0}, 3);
//   EXPECT_EQ(matrix_const.lower_bound(0, 0).i_row(), 0);
//   EXPECT_EQ(matrix_const.lower_bound(0, 0).i_column(), 1);
//   EXPECT_EQ(matrix_const.lower_bound(0, 1).i_row(), 0);
//   EXPECT_EQ(matrix_const.lower_bound(0, 1).i_column(), 1);
//   EXPECT_EQ(matrix_const.lower_bound(0, 2), matrix_const[0].end());

//   EXPECT_EQ(matrix_const.lower_bound(1, 0).i_row(), 1);
//   EXPECT_EQ(matrix_const.lower_bound(1, 0).i_column(), 0);
//   EXPECT_EQ(matrix_const.lower_bound(1, 1).i_row(), 1);
//   EXPECT_EQ(matrix_const.lower_bound(1, 1).i_column(), 2);
//   EXPECT_EQ(matrix_const.lower_bound(1, 2).i_row(), 1);
//   EXPECT_EQ(matrix_const.lower_bound(1, 2).i_column(), 2);

//   EXPECT_EQ(matrix_const.lower_bound(2, 0), matrix_const[2].end());
//   EXPECT_EQ(matrix_const.lower_bound(3, 3), (*matrix_const.end()).end());
// }