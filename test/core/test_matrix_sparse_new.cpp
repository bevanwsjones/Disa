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
    int row = 1;
    int column = 2;
    int value = 3;
    Matrix_Sparse_Element<int, int> element1(row, &column, &value);
    Matrix_Sparse_Element<int, int> element2(row, &column, &value);
    ASSERT_EQ(element1, element2);
    element2++;
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

TEST(Matrix_SCRATCH, test){
    Matrix_Sparse_Row<int, int> row;
    row[1] = 0;
}