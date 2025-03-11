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

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Element and Iterators
// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Element
// ---------------------------------------------------------------------------------------------------------------------

class matrix_sparse_element_test : public ::testing::Test {
 protected:
  template<bool _is_const>
  using element_type = Matrix_Sparse_Element<double, int, _is_const>;
  int column_index;
  double value;
  element_type<false>* element;

  void SetUp() override {
    column_index = 5;
    value = 3.14;
    element = new element_type<false>(&column_index, &value);
  }

  void TearDown() override { delete element; }
};

TEST_F(matrix_sparse_element_test, constructor_and_accessors) {
  EXPECT_EQ(element->i_column(), 5);
  EXPECT_DOUBLE_EQ(element->value(), 3.14);
}

TEST_F(matrix_sparse_element_test, const_accessors) {
  const element_type<true> const_element(&column_index, &value);
  EXPECT_EQ(const_element.i_column(), 5);
  EXPECT_DOUBLE_EQ(const_element.value(), 3.14);
}

TEST_F(matrix_sparse_element_test, equality_comparison) {
  element_type<false> same_element(&column_index, &value);
  EXPECT_TRUE(*element == same_element);

  int different_column = 6;
  double different_value = 2.71;
  element_type<false> different_element(&different_column, &different_value);
  EXPECT_FALSE(*element == different_element);
}

TEST_F(matrix_sparse_element_test, inequality_comparison) {
  element_type<false> same_element(&column_index, &value);
  EXPECT_FALSE(*element != same_element);

  int different_column = 6;
  double different_value = 2.71;
  element_type<false> different_element(&different_column, &different_value);
  EXPECT_TRUE(*element != different_element);
}

TEST_F(matrix_sparse_element_test, modifiers) {
  element->value() = 6.28;
  EXPECT_DOUBLE_EQ(element->value(), 6.28);

  // Note: We don't test modifying the column because it's a reference
  // and shouldn't be changed to maintain CSR ordering
}

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Element Iterators
// ---------------------------------------------------------------------------------------------------------------------

class base_iterator_matrix_sparse_element_test : public ::testing::Test {
 protected:
  void SetUp() override {
    // Set up test data
    columns = {0, 1, 2, 3, 4};
    entries = {1.0, 2.0, 3.0, 4.0, 5.0};
  }

  std::vector<int> columns;
  std::vector<double> entries;
};

TEST_F(base_iterator_matrix_sparse_element_test, default_constructor) {
  Base_Iterator_Matrix_Sparse_Element<double, int, false> it;
  // Default-constructed iterator should be in a valid but unspecified state
  // We can't make many assumptions about its behavior, but it shouldn't crash
}

TEST_F(base_iterator_matrix_sparse_element_test, constructor_and_dereference) {
  Base_Iterator_Matrix_Sparse_Element<double, int, false> iter(&columns[0], &entries[0]);
  EXPECT_EQ((*iter).i_column(), 0);
  EXPECT_EQ((*iter).value(), 1.0);
}

TEST_F(base_iterator_matrix_sparse_element_test, increment_and_decrement) {
  Base_Iterator_Matrix_Sparse_Element<double, int, false> iter(&columns[0], &entries[0]);
  ++iter;
  EXPECT_EQ((*iter).i_column(), 1);
  EXPECT_EQ(iter->i_column(), 1);
  EXPECT_EQ((*iter).value(), 2.0);
  EXPECT_EQ(iter->value(), 2.0);

  iter++;
  EXPECT_EQ((*iter).i_column(), 2);
  EXPECT_EQ(iter->i_column(), 2);
  EXPECT_EQ((*iter).value(), 3.0);
  EXPECT_EQ(iter->value(), 3.0);

  --iter;
  EXPECT_EQ((*iter).i_column(), 1);
  EXPECT_EQ(iter->i_column(), 1);
  EXPECT_EQ((*iter).value(), 2.0);
  EXPECT_EQ(iter->value(), 2.0);

  iter--;
  EXPECT_EQ((*iter).i_column(), 0);
  EXPECT_EQ(iter->i_column(), 0);
  EXPECT_EQ((*iter).value(), 1.0);
  EXPECT_EQ(iter->value(), 1.0);
}

TEST_F(base_iterator_matrix_sparse_element_test, comparison) {
  Base_Iterator_Matrix_Sparse_Element<double, int, false> iter1(&columns[0], &entries[0]);
  Base_Iterator_Matrix_Sparse_Element<double, int, false> iter2(&columns[0], &entries[0]);
  Base_Iterator_Matrix_Sparse_Element<double, int, false> iter3(&columns[1], &entries[1]);

  EXPECT_EQ(iter1, iter2);
  EXPECT_NE(iter1, iter3);
}

TEST_F(base_iterator_matrix_sparse_element_test, const_iterator) {
  Base_Iterator_Matrix_Sparse_Element<double, int, true> const_it(&columns[0], &entries[0]);
  EXPECT_EQ((*const_it).i_column(), 0);
  EXPECT_EQ((*const_it).value(), 1.0);

  // ensure we can increment and decrement the const iterator
  ++const_it;
  const_it++;
  EXPECT_EQ((*const_it).i_column(), 2);
  EXPECT_EQ(const_it->i_column(), 2);
  EXPECT_EQ((*const_it).value(), 3.0);
  EXPECT_EQ(const_it->value(), 3.0);
  --const_it;
  const_it--;
  EXPECT_EQ((*const_it).i_column(), 0);
  EXPECT_EQ(const_it->i_column(), 0);
  EXPECT_EQ((*const_it).value(), 1.0);
  EXPECT_EQ(const_it->value(), 1.0);
}

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Row and Iterators
// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Row
// ---------------------------------------------------------------------------------------------------------------------

class matrix_sparse_row_test : public ::testing::Test {
 protected:
  void SetUp() override {
    csr_data = CSR_Data<double, int>({.row_offset = {0, 2, 2, 5, 7},
                                      .i_column = {0, 2, 1, 3, 4, 0, 2},
                                      .value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
                                      .columns = 6});
  }

  CSR_Data<double, int> csr_data;
};

TEST_F(matrix_sparse_row_test, constructor_and_row_indexing) {
  int row_index = 0;
  Matrix_Sparse_Row row0(&csr_data, csr_data.row_offset.begin() + row_index);
  EXPECT_EQ(row0.row_offset(), 2);
  EXPECT_EQ(row0.size_non_zero(), 2);
  EXPECT_EQ(row0.row(), row_index);

  ++row_index;
  Matrix_Sparse_Row row1(&csr_data, csr_data.row_offset.begin() + row_index);
  EXPECT_EQ(row1.row_offset(), 2);
  EXPECT_EQ(row1.size_non_zero(), 0);
  EXPECT_EQ(row1.row(), row_index);

  ++row_index;
  Matrix_Sparse_Row row2(&csr_data, csr_data.row_offset.begin() + row_index);
  EXPECT_EQ(row2.row_offset(), 5);
  EXPECT_EQ(row2.size_non_zero(), 3);
  EXPECT_EQ(row2.row(), row_index);

  ++row_index;
  Matrix_Sparse_Row row3(&csr_data, csr_data.row_offset.begin() + row_index);
  EXPECT_EQ(row3.row_offset(), 7);
  EXPECT_EQ(row3.size_non_zero(), 2);
  EXPECT_EQ(row3.row(), row_index);
}

TEST_F(matrix_sparse_row_test, iterators) {
  Matrix_Sparse_Row<double, int> row(&csr_data, csr_data.row_offset.begin() + 2);

  auto it = row.begin();  // start of row 0
  EXPECT_EQ((*it).i_column(), 1);
  EXPECT_DOUBLE_EQ((*it).value(), 3.0);

  ++it;  // start of row 1
  EXPECT_EQ((*it).i_column(), 3);
  EXPECT_DOUBLE_EQ((*it).value(), 4.0);

  ++it;  // start of row 2
  EXPECT_EQ((*it).i_column(), 4);
  EXPECT_DOUBLE_EQ((*it).value(), 5.0);

  ++it;  // one past last row
  EXPECT_EQ(it, row.end());
}

TEST_F(matrix_sparse_row_test, const_iterators) {
  const Matrix_Sparse_Row<double, int> row(&csr_data, csr_data.row_offset.begin() + 2);

  auto it = row.begin();  // start of row 0
  EXPECT_EQ((*it).i_column(), 1);
  EXPECT_DOUBLE_EQ((*it).value(), 3.0);

  ++it;  // start of row 1
  EXPECT_EQ((*it).i_column(), 3);
  EXPECT_DOUBLE_EQ((*it).value(), 4.0);

  ++it;  // start of row 2
  EXPECT_EQ((*it).i_column(), 4);
  EXPECT_DOUBLE_EQ((*it).value(), 5.0);

  ++it;  // one past last row
  EXPECT_EQ(it, row.end());
}

TEST_F(matrix_sparse_row_test, at) {
  Matrix_Sparse_Row<double, int> row(&csr_data, csr_data.row_offset.begin() + 2);

  EXPECT_DOUBLE_EQ(row.at(1), 3.0);
  EXPECT_DOUBLE_EQ(row.at(3), 4.0);
  EXPECT_DOUBLE_EQ(row.at(4), 5.0);

  double remove_warning = 0;
  EXPECT_DEATH(remove_warning = row.at(0), "./*");
  EXPECT_DEATH(remove_warning = row.at(2), "./*");
  EXPECT_DEATH(remove_warning = row.at(5), "./*");
}

TEST_F(matrix_sparse_row_test, const_as) {
  const Matrix_Sparse_Row<double, int> row(&csr_data, csr_data.row_offset.begin() + 2);

  EXPECT_DOUBLE_EQ(row.at(1), 3.0);
  EXPECT_DOUBLE_EQ(row.at(3), 4.0);
  EXPECT_DOUBLE_EQ(row.at(4), 5.0);

  double remove_warning = 0;
  EXPECT_DEATH(remove_warning = row.at(0), "./*");
  EXPECT_DEATH(remove_warning = row.at(2), "./*");
  EXPECT_DEATH(remove_warning = row.at(5), "./*");
}

TEST_F(matrix_sparse_row_test, subscript_operators) {
  Matrix_Sparse_Row<double, int> row(&csr_data, csr_data.row_offset.begin() + 2);

  EXPECT_DOUBLE_EQ(row[1], 3.0);
  EXPECT_DOUBLE_EQ(row[3], 4.0);
  EXPECT_DOUBLE_EQ(row[4], 5.0);

  double remove_warning = 0;
  EXPECT_DEATH(remove_warning = row[0], "./*");
  EXPECT_DEATH(remove_warning = row[2], "./*");
  EXPECT_DEATH(remove_warning = row[5], "./*");
}

TEST_F(matrix_sparse_row_test, const_subscript_operators) {
  const Matrix_Sparse_Row<double, int> row(&csr_data, csr_data.row_offset.begin() + 2);

  EXPECT_DOUBLE_EQ(row[1], 3.0);
  EXPECT_DOUBLE_EQ(row[3], 4.0);
  EXPECT_DOUBLE_EQ(row[4], 5.0);
  double remove_warning = 0;
  EXPECT_DEATH(remove_warning = row[0], "./*");
  EXPECT_DEATH(remove_warning = row[2], "./*");
  EXPECT_DEATH(remove_warning = row[5], "./*");
}

TEST_F(matrix_sparse_row_test, sparse_accessor) {
  Matrix_Sparse_Row<double, int> row(&csr_data, csr_data.row_offset.begin() + 2);
  const int initial_offset = csr_data.row_offset.back();
  // Read existing element
  EXPECT_DOUBLE_EQ(row[1], 3.0);

  // Death on 'read' like operation of non existing column.
  double force_read;
  EXPECT_DEATH(force_read = row[0], "./*");

  // Modify existing element
  row[3] = 10.0;
  EXPECT_DOUBLE_EQ(row[3], 10.0);

  // Insert new element
  row[2] = 8.0;
  EXPECT_DOUBLE_EQ(row[2], 8.0);

  // Verify row properties after modification
  EXPECT_EQ(row.row_offset(), 6);                             // added one entry
  EXPECT_EQ(row.size_non_zero(), 4);                          // added one entry
  EXPECT_EQ(csr_data.row_offset.back(), initial_offset + 1);  // added one entry

  auto it = row.begin();
  EXPECT_EQ((*it).i_column(), 1);
  EXPECT_DOUBLE_EQ((*it).value(), 3.0);
  ++it;
  EXPECT_EQ((*it).i_column(), 2);
  EXPECT_DOUBLE_EQ((*it).value(), 8.0);
  ++it;
  EXPECT_EQ((*it).i_column(), 3);
  EXPECT_DOUBLE_EQ((*it).value(), 10.0);
  ++it;
  EXPECT_EQ((*it).i_column(), 4);
  EXPECT_DOUBLE_EQ((*it).value(), 5.0);
}

TEST_F(matrix_sparse_row_test, empty_row) {
  Matrix_Sparse_Row<double, int> row(&csr_data, csr_data.row_offset.begin() + 1);

  EXPECT_EQ(row.row_offset(), 2);
  EXPECT_EQ(row.size_non_zero(), 0);
  EXPECT_EQ(row.row(), 1);
  EXPECT_EQ(row.begin(), row.end());
  EXPECT_EQ(row.cbegin(), row.cend());
  double remove_warning = 0;
  EXPECT_DEATH(remove_warning = row.at(0), ".*");
}

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Row Iterator
// ---------------------------------------------------------------------------------------------------------------------

class base_iterator_matrix_sparse_row_test : public ::testing::Test {
 protected:
  void SetUp() override {
    // Set up a sample CSR matrix
    csr_data.row_offset = {0, 2, 4, 7};
    csr_data.i_column = {0, 2, 1, 3, 0, 2, 4};
    csr_data.value = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
    csr_data.columns = 5;
  }

  CSR_Data<double, int> csr_data;
};

TEST_F(base_iterator_matrix_sparse_row_test, default_constructor) {
  Base_Iterator_Matrix_Sparse_Row<double, int, false> it;
  // Default-constructed iterator should be in a valid but unspecified state
  // We can't make many assumptions about its behavior, but it shouldn't crash
  EXPECT_NO_THROW({
    auto dummy = *it;
    (void)dummy;  // Avoid unused variable warning
  });
}

TEST_F(base_iterator_matrix_sparse_row_test, constructor_and_dereference) {}

TEST_F(base_iterator_matrix_sparse_row_test, increment_and_decrement) {}

TEST_F(base_iterator_matrix_sparse_row_test, comparison) {}

TEST_F(base_iterator_matrix_sparse_row_test, const_iterator) {}

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse
// ---------------------------------------------------------------------------------------------------------------------
// Constructors/Destructors
// ---------------------------------------------------------------------------------------------------------------------

// TEST(test_matrix_sparse, constructors_row_column) {
//   EXPECT_TRUE(false);

//   // Matrix_Sparse<Scalar, std::size_t> matrix(2, 4);
//   // EXPECT_EQ(matrix.size_row(), 2);
//   // EXPECT_EQ(matrix.size_column(), 4);
// }

// TEST(test_matrix_sparse, constructors_initialiser_lists) {
//   EXPECT_TRUE(false);

//   // Matrix_Sparse<Scalar, std::size_t> matrix({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
//   //                                           5);
//   // EXPECT_EQ(matrix.size_row(), 4);
//   // EXPECT_EQ(matrix.size_column(), 5);
//   // EXPECT_EQ(matrix.size_non_zero(), 7);
//   // EXPECT_DOUBLE_EQ(matrix[0][1], 1.0);
//   // EXPECT_DOUBLE_EQ(matrix[0][3], 2.0);
//   // EXPECT_DOUBLE_EQ(matrix[1][0], 4.0);
//   // EXPECT_DOUBLE_EQ(matrix[1][2], 3.0);
//   // EXPECT_DOUBLE_EQ(matrix[1][3], 5.0);
//   // EXPECT_DOUBLE_EQ(matrix[3][3], 7.0);
//   // EXPECT_DOUBLE_EQ(matrix[3][4], 6.0);

//   // // Valid construction 2x2: Matrix_Sparse({0, 2, 3}, {1, 0, 0}, {1.0, 2.0, 3.0}, 2)
//   // EXPECT_DEATH((Matrix_Sparse<Scalar, std::size_t>({0, 2, 3}, {1, 0, 0, 1}, {1.0, 2.0, 3.0, 4.0}, 2)),
//   //              ".*");  // more column indexes than non-zero.back().
//   // EXPECT_DEATH((Matrix_Sparse<Scalar, std::size_t>({0, 2, 3}, {1, 0, 0}, {1.0, 2.0, 3.0, 4.0}, 2)),
//   //              ".*");  // mismatch column and entry size.
//   // EXPECT_DEATH((Matrix_Sparse<Scalar, std::size_t>({1, 2, 3}, {1, 0, 0}, {1.0, 2.0, 3.0}, 2)),
//   //              ".*");  // first non-zero value is not 0.
//   // EXPECT_DEATH((Matrix_Sparse<Scalar, std::size_t>({2, 0, 3}, {1, 0, 0}, {1.0, 2.0, 3.0}, 2)),
//   //              ".*");  // un-ordered non-zeros
//   // EXPECT_DEATH((Matrix_Sparse<Scalar, std::size_t>({0, 2, 3}, {1, 0, 2}, {1.0, 2.0, 3.0}, 2)),
//   //              ".*");  // column index out of range
//   // EXPECT_DEATH((Matrix_Sparse<Scalar, std::size_t>({0, 2, 3}, {1, 1, 0}, {1.0, 2.0, 3.0}, 2)),
//   //              ".*");  // repeated column index
// }

// // ---------------------------------------------------------------------------------------------------------------------
// // Element Access
// // ---------------------------------------------------------------------------------------------------------------------

// TEST(test_matrix_sparse, operator_subscript) {
//   EXPECT_TRUE(false);

//   // Matrix_Sparse<Scalar, std::size_t> matrix_0({0, 1, 2}, {0, 1}, {1.0, 2.0}, 2);

//   // EXPECT_DOUBLE_EQ(matrix_0[0][0], 1.0);
//   // EXPECT_DOUBLE_EQ(matrix_0[1][1], 2.0);
//   // EXPECT_TRUE(false);
//   // //matrix_0[0][1] = 3.0; // this should insert a new element.

//   // const Matrix_Sparse<Scalar, std::size_t> matrix_1 = matrix_0;
//   // EXPECT_DOUBLE_EQ(matrix_1[0][0], 1.0);
//   // EXPECT_DOUBLE_EQ(matrix_1[0][1], 3.0);
//   // EXPECT_DOUBLE_EQ(matrix_1[1][1], 2.0);

//   //Check debug for const matrix.
//   // EXPECT_DEATH(matrix_1[1][0], "./*");
// }

// // ---------------------------------------------------------------------------------------------------------------------
// // Capacity
// // ---------------------------------------------------------------------------------------------------------------------

// TEST(test_matrix_sparse, empty) {
//   EXPECT_TRUE(false);

//   // Matrix_Sparse<Scalar, std::size_t> matrix;
//   // EXPECT_TRUE(matrix.empty());
//   // EXPECT_EQ(matrix.size_row(), 0);
//   // EXPECT_EQ(matrix.size_column(), 0);
//   // EXPECT_EQ(matrix.size_non_zero(), 0);
//   // EXPECT_EQ(matrix.size().first, std::make_pair(0, 0).first);
//   // EXPECT_EQ(matrix.size().second, std::make_pair(0, 0).second);

//   // // check edge cases.
//   // matrix.resize(0, 2);
//   // EXPECT_TRUE(matrix.empty());
//   // matrix.resize(1, 0);
//   // EXPECT_FALSE(matrix.empty());
//   // matrix.resize(2, 2);
//   // EXPECT_FALSE(matrix.empty());
// }

// TEST(test_matrix_sparse, size) {
//   EXPECT_TRUE(false);

//   // Matrix_Sparse<Scalar, std::size_t> matrix;
//   // EXPECT_EQ(matrix.size_row(), 0);
//   // EXPECT_EQ(matrix.size_column(), 0);
//   // EXPECT_EQ(matrix.size_non_zero(), 0);
//   // EXPECT_EQ(matrix.size().first, std::make_pair(0, 0).first);
//   // EXPECT_EQ(matrix.size().second, std::make_pair(0, 0).second);

//   // matrix.resize(7, 6);
//   // EXPECT_TRUE(false);
//   // //  matrix[0][5] = 1.0; // add additional entry - show that non-zero increased.
//   // EXPECT_FALSE(matrix.empty());
//   // EXPECT_EQ(matrix.size_row(), 7);
//   // EXPECT_EQ(matrix.size_column(), 6);
//   // EXPECT_EQ(matrix.size_non_zero(), 1);
//   // EXPECT_EQ(matrix.size().first, std::make_pair(7, 6).first);
//   // EXPECT_EQ(matrix.size().second, std::make_pair(7, 6).second);
// }

// TEST(test_matrix_sparse, reserve_capacity) {
//   EXPECT_TRUE(false);

//   // Matrix_Sparse<Scalar, std::size_t> matrix;
//   // EXPECT_EQ(matrix.capacity().first, 0);
//   // EXPECT_EQ(matrix.capacity().second, 0);

//   // matrix.reserve(10, 40);
//   // EXPECT_EQ(matrix.capacity().first, 11);
//   // EXPECT_EQ(matrix.capacity().second, 40);
// }

// TEST(test_matrix_sparse, shrink_to_fit) {
//   EXPECT_TRUE(false);

//   // Matrix_Sparse<Scalar, std::size_t> matrix;
//   // matrix.reserve(10, 40);
//   // EXPECT_EQ(matrix.capacity().first, 11);
//   // EXPECT_EQ(matrix.capacity().second, 40);

//   // matrix.resize(5, 30);
//   // EXPECT_TRUE(false);
//   // //matrix[4][8] = 10;
//   // matrix.shrink_to_fit();
//   // EXPECT_EQ(matrix.capacity().first, 6);
//   // EXPECT_EQ(matrix.capacity().second, 1);
// }

// // -------------------------------------------------------------------------------------------------------------------
// // Modifiers
// // -------------------------------------------------------------------------------------------------------------------

// TEST(test_matrix_sparse, clear) {
//   EXPECT_TRUE(false);

//   // Matrix_Sparse<Scalar, std::size_t> matrix;
//   // matrix.resize(7, 6);
//   // EXPECT_TRUE(false);
//   // //matrix[0][5] = 1.0; // add additional entry - show that non-zero increased.

//   // matrix.clear();
//   // EXPECT_TRUE(matrix.empty());
//   // EXPECT_EQ(matrix.size_row(), 0);
//   // EXPECT_EQ(matrix.size_column(), 0);
//   // EXPECT_EQ(matrix.size_non_zero(), 0);
//   // EXPECT_EQ(matrix.size().first, std::make_pair(0, 0).first);
//   // EXPECT_EQ(matrix.size().second, std::make_pair(0, 0).second);
//   // EXPECT_EQ(matrix.capacity().first, std::make_pair(8, 1).first);    // make sure we have not lost capacity.
//   // EXPECT_EQ(matrix.capacity().second, std::make_pair(8, 1).second);  // make sure we have not lost capacity.
// }

// TEST(test_matrix_sparse, insert_insert_or_assign) {
//   EXPECT_TRUE(false);

//   // Matrix_Sparse<Scalar, std::size_t> matrix;
//   // matrix.resize(5, 5);

//   //   matrix.insert(3, 2, 1);
//   //   EXPECT_EQ(matrix.size_non_zero(), 1);
//   //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

//   //   auto iter_bool_pair = matrix.insert(3, 1, 3); // out of order testing - will check order at the end.
//   //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 3.0);
//   //   EXPECT_TRUE(iter_bool_pair.second);
//   //   EXPECT_EQ(matrix.size_non_zero(), 2);
//   //   EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
//   //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

//   //   iter_bool_pair = matrix.insert(2, 1, 4);
//   //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 4.0);
//   //   EXPECT_TRUE(iter_bool_pair.second);
//   //   EXPECT_DOUBLE_EQ(matrix.size_non_zero(), 3);
//   //   EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
//   //   EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
//   //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

//   //   iter_bool_pair = matrix.insert(2, 4, 5); // last of columns test
//   //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 5.0);
//   //   EXPECT_TRUE(iter_bool_pair.second);
//   //   EXPECT_EQ(matrix.size_non_zero(), 4);
//   //   EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
//   //   EXPECT_DOUBLE_EQ(matrix[2][4], 5.0);
//   //   EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
//   //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);

//   //   iter_bool_pair = matrix.insert(4, 0, 8); // last of rows test
//   //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 8.0);
//   //   EXPECT_TRUE(iter_bool_pair.second);
//   //   EXPECT_EQ(matrix.size_non_zero(), 5);
//   //   EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
//   //   EXPECT_DOUBLE_EQ(matrix[2][4], 5.0);
//   //   EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
//   //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);
//   //   EXPECT_DOUBLE_EQ(matrix[4][0], 8.0);

//   //   iter_bool_pair = matrix.insert(4, 4, -5); // last row and column test
//   //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, -5.0);
//   //   EXPECT_TRUE(iter_bool_pair.second);
//   //   EXPECT_EQ(matrix.size_non_zero(), 6);
//   //   EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
//   //   EXPECT_DOUBLE_EQ(matrix[2][4], 5.0);
//   //   EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
//   //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);
//   //   EXPECT_DOUBLE_EQ(matrix[4][0], 8.0);
//   //   EXPECT_DOUBLE_EQ(matrix[4][4], -5.0);

//   //   iter_bool_pair = matrix.insert(3, 2, 2.0); // no insert test
//   //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 1.0);
//   //   EXPECT_FALSE(iter_bool_pair.second);
//   //   EXPECT_EQ(matrix.size_non_zero(), 6);
//   //   EXPECT_DOUBLE_EQ(matrix[2][1], 4.0);
//   //   EXPECT_DOUBLE_EQ(matrix[2][4], 5.0);
//   //   EXPECT_DOUBLE_EQ(matrix[3][1], 3.0);
//   //   EXPECT_DOUBLE_EQ(matrix[3][2], 1.0);
//   //   EXPECT_DOUBLE_EQ(matrix[4][0], 8.0);
//   //   EXPECT_DOUBLE_EQ(matrix[4][4], -5.0);

//   //   iter_bool_pair = matrix.insert(6, 2, 10.0); // extra rows
//   //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 10.0);
//   //   EXPECT_TRUE(iter_bool_pair.second);
//   //   EXPECT_EQ(matrix.size_row(), 7);
//   //   EXPECT_EQ(matrix.size_non_zero(), 7);
//   //   EXPECT_DOUBLE_EQ(matrix[6][2], 10.0);

//   //   iter_bool_pair = matrix.insert(2, 6, 50.0); // extra columns
//   //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, 50.0);
//   //   EXPECT_TRUE(iter_bool_pair.second);
//   //   EXPECT_EQ(matrix.size_row(), 7);
//   //   EXPECT_EQ(matrix.size_column(), 7);
//   //   EXPECT_EQ(matrix.size_non_zero(), 8);
//   //   EXPECT_DOUBLE_EQ(matrix[2][6], 50.0);

//   //   iter_bool_pair = matrix.insert_or_assign(2, 6, -50.0); // check insert_or_assign
//   //   EXPECT_DOUBLE_EQ(*iter_bool_pair.first, -50.0);
//   //   EXPECT_FALSE(iter_bool_pair.second);
//   //   EXPECT_EQ(matrix.size_row(), 7);
//   //   EXPECT_EQ(matrix.size_column(), 7);
//   //   EXPECT_EQ(matrix.size_non_zero(), 8);
//   //   EXPECT_DOUBLE_EQ(matrix[2][6], -50.0);

//   //   // check ascending order has been maintained.
//   //   FOR_EACH(row_vector, matrix) {
//   //     auto prev = row_vector.begin();
//   //     FOR_ITER(iter, row_vector) {
//   //       if(iter != row_vector.begin()) {
//   //         EXPECT_GT(iter.i_column(), prev.i_column());
//   //         EXPECT_EQ(iter.i_row(), prev.i_row()); // more of a sanity check.
//   //         ++prev;
//   //       }
//   //     }
//   //   }
// }

// TEST(test_matrix_sparse, erase) {
//   EXPECT_TRUE(false);

//   //   Matrix_Sparse<Scalar, std::size_t> matrix({0, 2, 3}, {1, 0, 0}, {1.0, 2.0, 3.0}, 2);
//   //   auto iter = matrix.erase(matrix.find(0, 1));
//   //   EXPECT_EQ(matrix.size_row(), 2);
//   //   EXPECT_EQ(matrix.size_column(), 2);
//   //   EXPECT_EQ(matrix.size_non_zero(), 2);
//   //   EXPECT_TRUE(iter == matrix[0].end()); // should point to end
//   //   EXPECT_DOUBLE_EQ(*iter, 3.0); // end should be the next row over.
//   //   EXPECT_DOUBLE_EQ(matrix[0][0], 2.0);
//   //   EXPECT_DOUBLE_EQ(matrix[1][0], 3.0);

//   //   // Check debug undefined behaviour
//   //   EXPECT_DEATH(matrix.erase(matrix.find(3, 0)), "./*");
//   //   EXPECT_DEATH(matrix.erase(matrix.find(0, 2)), "./*");
//   //   EXPECT_DEATH(matrix.erase(matrix.find(1, 1)), "./*");
// }

// TEST(test_matrix_sparse, resize) {

//   EXPECT_TRUE(false);

//   //   Matrix_Sparse<Scalar, std::size_t> matrix;

//   //   // check row and column expansion
//   //   matrix.resize(5, 9);
//   //   EXPECT_FALSE(matrix.empty());
//   //   EXPECT_EQ(matrix.size_row(), 5);
//   //   EXPECT_EQ(matrix.size_column(), 9);
//   //   EXPECT_EQ(matrix.size_non_zero(), 0);
//   //   EXPECT_EQ(matrix.size().first, std::make_pair(5, 9).first);
//   //   EXPECT_EQ(matrix.size().second, std::make_pair(5, 9).second);

//   //   // check row reduction
//   //   matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
//   //   matrix.resize(3, 5);
//   //   EXPECT_EQ(matrix.size_row(), 3);
//   //   EXPECT_EQ(matrix.size_column(), 5);
//   //   EXPECT_EQ(matrix.size_non_zero(), 5);

//   //   // check column reduction
//   //   matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
//   //   matrix.resize(4, 2);
//   //   EXPECT_EQ(matrix.size_row(), 4);
//   //   EXPECT_EQ(matrix.size_column(), 2);
//   //   EXPECT_EQ(matrix.size_non_zero(), 2);
//   //   EXPECT_EQ(matrix[0][1], 1.0);
//   //   EXPECT_EQ(matrix[1][0], 4.0);

//   //   // check row reduction with column expansion
//   //   matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
//   //   matrix.resize(2, 8);
//   //   EXPECT_EQ(matrix.size_row(), 2);
//   //   EXPECT_EQ(matrix.size_column(), 8);
//   //   EXPECT_EQ(matrix.size_non_zero(), 5);
//   //   EXPECT_EQ(matrix[0][1], 1.0);
//   //   EXPECT_EQ(matrix[0][3], 2.0);
//   //   EXPECT_EQ(matrix[1][0], 4.0);
//   //   EXPECT_EQ(matrix[1][2], 3.0);
//   //   EXPECT_EQ(matrix[1][3], 5.0);

//   //   // check row expansion with column reduction
//   //   matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
//   //   matrix.resize(10, 2);
//   //   EXPECT_EQ(matrix.size_row(), 10);
//   //   EXPECT_EQ(matrix.size_column(), 2);
//   //   EXPECT_EQ(matrix.size_non_zero(), 2);
//   //   EXPECT_EQ(matrix[0][1], 1.0);
//   //   EXPECT_EQ(matrix[1][0], 4.0);

//   //   // check row and column reduction
//   //   matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
//   //   matrix.resize(1, 2);
//   //   EXPECT_EQ(matrix.size_row(), 1);
//   //   EXPECT_EQ(matrix.size_column(), 2);
//   //   EXPECT_EQ(matrix.size_non_zero(), 1);
//   //   EXPECT_EQ(matrix[0][1], 1.0);

//   //   // check zero sizing.
//   //   matrix = Matrix_Sparse ({0, 2, 5, 5, 7}, {1, 3, 2, 0, 3, 4, 3}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0}, 5);
//   //   matrix.resize(0, 0);
//   //   EXPECT_EQ(matrix.size_row(), 0);
//   //   EXPECT_EQ(matrix.size_column(), 0);
//   //   EXPECT_EQ(matrix.size_non_zero(), 0);
// }

// TEST(test_matrix_sparse, swap) {

//   EXPECT_TRUE(false);
//   //   Matrix_Sparse<Scalar, std::size_t> matrix_0({0, 1, 2}, {1, 0}, {1.0, 2.0}, 2);
//   //   Matrix_Sparse<Scalar, std::size_t> matrix_1({0, 1, 2, 3}, {1, 0, 1}, {3.0, 4.0, 5.0}, 3);

//   //   matrix_0.swap(matrix_1);
//   //   EXPECT_EQ(matrix_0.size_row(), 3);
//   //   EXPECT_EQ(matrix_0.size_column(), 3);
//   //   EXPECT_EQ(matrix_0.size_non_zero(), 3);
//   //   EXPECT_EQ(matrix_1.size_row(), 2);
//   //   EXPECT_EQ(matrix_1.size_column(), 2);
//   //   EXPECT_EQ(matrix_1.size_non_zero(), 2);
//   //   EXPECT_DOUBLE_EQ(matrix_0[0][1], 3.0);
//   //   EXPECT_DOUBLE_EQ(matrix_0[1][0], 4.0);
//   //   EXPECT_DOUBLE_EQ(matrix_0[2][1], 5.0);
//   //   EXPECT_DOUBLE_EQ(matrix_1[0][1], 1.0);
//   //   EXPECT_DOUBLE_EQ(matrix_1[1][0], 2.0);
// }

// // -------------------------------------------------------------------------------------------------------------------
// // Lookup
// // -------------------------------------------------------------------------------------------------------------------

// // TEST(test_matrix_sparse, find) {
// //   Matrix_Sparse<Scalar, std::size_t> matrix({0, 1, 3, 3}, {1, 0, 2}, {3.0, 4.0, 5.0}, 3);
// //   EXPECT_EQ(matrix.find(0, 0), matrix[0].end());
// //   EXPECT_EQ(matrix.find(0, 1).i_row(), 0);
// //   EXPECT_EQ(matrix.find(0, 1).i_column(), 1);
// //   EXPECT_EQ(matrix.find(0, 2), matrix[0].end());

// //   EXPECT_EQ(matrix.find(1, 0).i_row(), 1);
// //   EXPECT_EQ(matrix.find(1, 0).i_column(), 0);
// //   EXPECT_EQ(matrix.find(1, 1), matrix[1].end());
// //   EXPECT_EQ(matrix.find(1, 2).i_row(), 1);
// //   EXPECT_EQ(matrix.find(1, 2).i_column(), 2);

// //   EXPECT_EQ(matrix.find(2, 1), matrix[2].end());
// //   EXPECT_EQ(matrix.find(2, 2), matrix[2].end());
// //   EXPECT_EQ(matrix.find(2, 3), matrix[2].end());
// //   EXPECT_EQ(matrix.find(3, 3), matrix.end()->end());

// //   const Matrix_Sparse<Scalar, std::size_t> matrix_const({0, 1, 3, 3}, {1, 0, 2}, {3.0, 4.0, 5.0}, 3);
// //   EXPECT_EQ(matrix_const.find(0, 0), matrix_const[0].end());
// //   EXPECT_EQ(matrix_const.find(0, 1).i_row(), 0);
// //   EXPECT_EQ(matrix_const.find(0, 1).i_column(), 1);
// //   EXPECT_EQ(matrix_const.find(0, 2), matrix_const[0].end());

// //   EXPECT_EQ(matrix_const.find(1, 0).i_row(), 1);
// //   EXPECT_EQ(matrix_const.find(1, 0).i_column(), 0);
// //   EXPECT_EQ(matrix_const.find(1, 1), matrix_const[1].end());
// //   EXPECT_EQ(matrix_const.find(1, 2).i_row(), 1);
// //   EXPECT_EQ(matrix_const.find(1, 2).i_column(), 2);

// //   EXPECT_EQ(matrix_const.find(2, 1), matrix_const[2].end());
// //   EXPECT_EQ(matrix_const.find(2, 2), matrix_const[2].end());
// //   EXPECT_EQ(matrix_const.find(2, 3), matrix_const[2].end());
// //   EXPECT_EQ(matrix_const.find(3, 3), (*matrix_const.end()).end());
// // }

// // TEST(test_matrix_sparse, contains) {
// //   Matrix_Sparse<Scalar, std::size_t> matrix({0, 1, 2, 3}, {1, 2, 1}, {3.0, 4.0, 5.0}, 3);
// //   EXPECT_TRUE(matrix.contains(0, 1));
// //   EXPECT_TRUE(matrix.contains(1, 2));
// //   EXPECT_FALSE(matrix.contains(0, 0));
// //   EXPECT_FALSE(matrix.contains(2, 0));

// //   // edge cases
// //   matrix = Matrix_Sparse<Scalar, std::size_t>({0, 0, 0}, {}, {}, 2);
// //   EXPECT_FALSE(matrix.contains(0, 0));
// //   EXPECT_FALSE(matrix.contains(0, 1));
// //   EXPECT_FALSE(matrix.contains(1, 0));
// //   EXPECT_FALSE(matrix.contains(1, 1));
// // }

// // TEST(test_matrix_sparse, lower_bound) {
// //   Matrix_Sparse<Scalar, std::size_t> matrix({0, 1, 3, 3}, {1, 0, 2}, {3.0, 4.0, 5.0}, 3);
// //   EXPECT_EQ(matrix.lower_bound(0, 0).i_row(), 0);
// //   EXPECT_EQ(matrix.lower_bound(0, 0).i_column(), 1);
// //   EXPECT_EQ(matrix.lower_bound(0, 1).i_row(), 0);
// //   EXPECT_EQ(matrix.lower_bound(0, 1).i_column(), 1);
// //   EXPECT_EQ(matrix.lower_bound(0, 2), matrix[0].end());

// //   EXPECT_EQ(matrix.lower_bound(1, 0).i_row(), 1);
// //   EXPECT_EQ(matrix.lower_bound(1, 0).i_column(), 0);
// //   EXPECT_EQ(matrix.lower_bound(1, 1).i_row(), 1);
// //   EXPECT_EQ(matrix.lower_bound(1, 1).i_column(), 2);
// //   EXPECT_EQ(matrix.lower_bound(1, 2).i_row(), 1);
// //   EXPECT_EQ(matrix.lower_bound(1, 2).i_column(), 2);

// //   EXPECT_EQ(matrix.lower_bound(2, 0), matrix[2].end());
// //   EXPECT_EQ(matrix.lower_bound(3, 3), matrix.end()->end());

// //   const Matrix_Sparse<Scalar, std::size_t> matrix_const({0, 1, 3, 3}, {1, 0, 2}, {3.0, 4.0, 5.0}, 3);
// //   EXPECT_EQ(matrix_const.lower_bound(0, 0).i_row(), 0);
// //   EXPECT_EQ(matrix_const.lower_bound(0, 0).i_column(), 1);
// //   EXPECT_EQ(matrix_const.lower_bound(0, 1).i_row(), 0);
// //   EXPECT_EQ(matrix_const.lower_bound(0, 1).i_column(), 1);
// //   EXPECT_EQ(matrix_const.lower_bound(0, 2), matrix_const[0].end());

// //   EXPECT_EQ(matrix_const.lower_bound(1, 0).i_row(), 1);
// //   EXPECT_EQ(matrix_const.lower_bound(1, 0).i_column(), 0);
// //   EXPECT_EQ(matrix_const.lower_bound(1, 1).i_row(), 1);
// //   EXPECT_EQ(matrix_const.lower_bound(1, 1).i_column(), 2);
// //   EXPECT_EQ(matrix_const.lower_bound(1, 2).i_row(), 1);
// //   EXPECT_EQ(matrix_const.lower_bound(1, 2).i_column(), 2);

// //   EXPECT_EQ(matrix_const.lower_bound(2, 0), matrix_const[2].end());
// //   EXPECT_EQ(matrix_const.lower_bound(3, 3), (*matrix_const.end()).end());
// // }