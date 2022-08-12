//----------------------------------------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------------------------------------
// File Name: test_matrix_dense.h
// Description: Unit tests for the header matrix_dense.h
//----------------------------------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "matrix_dense.h"

#ifdef DISA_DEBUG

using namespace Disa;

//----------------------------------------------------------------------------------------------------------------------
// Constructors/Destructors
//----------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_dense, constructors_initialiser_lists) {

  Matrix_Dense<0, 0> dynamic_matrix = {{1.0, 2.0},
                                       {3.0, 4.0}};
  Matrix_Dense<2, 2> static_matrix = {{5.0, 6.0},
                                      {7.0, 8.0}};

  EXPECT_DOUBLE_EQ(dynamic_matrix[0][0], 1.0);
  EXPECT_DOUBLE_EQ(dynamic_matrix[0][1], 2.0);
  EXPECT_DOUBLE_EQ(dynamic_matrix[1][0], 3.0);
  EXPECT_DOUBLE_EQ(dynamic_matrix[1][1], 4.0);
  EXPECT_DOUBLE_EQ(static_matrix[0][0], 5.0);
  EXPECT_DOUBLE_EQ(static_matrix[0][1], 6.0);
  EXPECT_DOUBLE_EQ(static_matrix[1][0], 7.0);
  EXPECT_DOUBLE_EQ(static_matrix[1][1], 8.0);
}

TEST(test_matrix_dense, constructors_lambda) {
  Matrix_Dense<0, 0> dynamic_matrix([](const std::size_t row, const std::size_t column) {
    return static_cast<double>(row)*2.0 + static_cast<double>(column);
  }, 2, 2);
  Matrix_Dense<2, 2> static_matrix([](const std::size_t row, const std::size_t column) {
    return static_cast<double>(row)*2.0 + 4.0 + static_cast<double>(column);
  });
  EXPECT_DOUBLE_EQ(dynamic_matrix[0][0], 0.0);
  EXPECT_DOUBLE_EQ(dynamic_matrix[0][1], 1.0);
  EXPECT_DOUBLE_EQ(dynamic_matrix[1][0], 2.0);
  EXPECT_DOUBLE_EQ(dynamic_matrix[1][1], 3.0);
  EXPECT_DOUBLE_EQ(static_matrix[0][0], 4.0);
  EXPECT_DOUBLE_EQ(static_matrix[0][1], 5.0);
  EXPECT_DOUBLE_EQ(static_matrix[1][0], 6.0);
  EXPECT_DOUBLE_EQ(static_matrix[1][1], 7.0);

  EXPECT_DEATH((Matrix_Dense<2, 2>([](const std::size_t row, const std::size_t column) {
    return static_cast<double>(row)*2.0 + 4.0 + static_cast<double>(column);
  }, 1, 2)), "./*");
  EXPECT_DEATH((Matrix_Dense<2, 2>([](const std::size_t row, const std::size_t column) {
    return static_cast<double>(row)*2.0 + 4.0 + static_cast<double>(column);
  }, 2, 1)), "./*");
}

//----------------------------------------------------------------------------------------------------------------------
// Size Functions
//----------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_dense, size_row_column_size_resize) {
  Matrix_Dense<0, 0> dynamic_matrix;
  Matrix_Dense<2, 2> static_matrix;

  EXPECT_EQ(dynamic_matrix.size().first, 0);
  EXPECT_EQ(dynamic_matrix.size().second, 0);
  EXPECT_EQ(static_matrix.size().first, 2);
  EXPECT_EQ(static_matrix.size().second, 2);

  EXPECT_EQ(dynamic_matrix.size_row(), 0);
  EXPECT_EQ(dynamic_matrix.size_column(), 0);
  EXPECT_EQ(static_matrix.size_row(), 2);
  EXPECT_EQ(static_matrix.size_column(), 2);

  dynamic_matrix.resize(1);
  EXPECT_EQ(dynamic_matrix.size().first, 1);
  EXPECT_EQ(dynamic_matrix.size().second, 0);
  EXPECT_EQ(dynamic_matrix.size_row(), 1);
  EXPECT_EQ(dynamic_matrix.size_column(), 0);
  FOR_EACH(row, dynamic_matrix) EXPECT_EQ(row.size(), 0);

  dynamic_matrix.resize(3, 2);
  EXPECT_EQ(dynamic_matrix.size().first, 3);
  EXPECT_EQ(dynamic_matrix.size().second, 2);
  EXPECT_EQ(dynamic_matrix.size_row(), 3);
  EXPECT_EQ(dynamic_matrix.size_column(), 2);
  FOR_EACH(row, dynamic_matrix) EXPECT_EQ(row.size(), 2);
}


#endif//DISA_DEBUG