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
// File Name: test_matrix_operators.h
// Description: Unit tests for matrix operations.
// ----------------------------------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "matrix_operators.h"

using namespace Disa;

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Type
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, is_square) {
  Matrix_Dense<0, 0> matrix_square_dynamic({{1.0, 2.0, 3.0},
                                            {4.0, 5.0, 6.0},
                                            {7.0, 8.0, 9.0}});
  Matrix_Dense<0, 0> matrix_row_rectangular_dynamic({{1.0, 2.0, 3.0},
                                                     {4.0, 5.0, 6.0}});
  Matrix_Dense<0, 0> matrix_column_rectangular_dynamic({{1.0, 2.0},
                                                        {3.0, 4.0},
                                                        {5.0, 6.0}});
  Matrix_Dense<3, 3> matrix_square_static({{1.0, 2.0, 3.0},
                                           {4.0, 5.0, 6.0},
                                           {7.0, 8.0, 9.0}});
  Matrix_Dense<2, 3> matrix_row_rectangular_static({{1.0, 2.0, 3.0},
                                                    {4.0, 5.0, 6.0}});
  Matrix_Dense<3, 2> matrix_column_rectangular_static({{1.0, 2.0},
                                                       {3.0, 4.0},
                                                       {5.0, 6.0}});
  Matrix_Sparse matrix_square({0, 2, 4, 6}, {0, 1, 0, 2, 1, 2}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0}, 3);
  Matrix_Sparse matrix_row_rectangular({0, 2, 3}, {0, 1, 0}, {1.0, 2.0, 3.0}, 3);
  Matrix_Sparse matrix_column_rectangular({0, 1, 3, 5}, {0, 0, 1, 0, 1}, {1.0, 2.0, 3.0, 4.0, 5.0}, 2);

  EXPECT_TRUE(is_square(matrix_square_dynamic));
  EXPECT_FALSE(is_square(matrix_row_rectangular_dynamic));
  EXPECT_FALSE(is_square(matrix_column_rectangular_dynamic));
  EXPECT_TRUE(is_square(matrix_square_static));
  EXPECT_FALSE(is_square(matrix_row_rectangular_static));
  EXPECT_FALSE(is_square(matrix_column_rectangular_static));
  EXPECT_TRUE(is_square(matrix_square));
  EXPECT_FALSE(is_square(matrix_row_rectangular));
  EXPECT_FALSE(is_square(matrix_column_rectangular));
}

TEST(test_matrix_sparse, is_symmetric_dense) {
  Matrix_Dense<0, 0> matrix_symmetric_dynamic({{1.0, 2.0, 7.0},
                                               {2.0, 5.0, -6.0},
                                               {7.0, -6.0, 9.0}});
  Matrix_Dense<0, 0> matrix_not_symmetric_dynamic({{1.0, 2.0, 3.0},
                                                   {4.0, 5.0, 6.0},
                                                   {7.0, 8.0, 9.0}});
  Matrix_Dense<0, 0> matrix_not_square_0_dynamic({{1.0, 2.0, 3.0},
                                                 {4.0, 5.0, 6.0}});
  Matrix_Dense<0, 0> matrix_not_square_1_dynamic({{1.0, 2.0},
                                                 {3.0, 4.0},
                                                 {5.0, 6.0}});
  Matrix_Dense<3, 3> matrix_symmetric_static({{1.0, 2.0, 7.0},
                                              {2.0, 5.0, -6.0},
                                              {7.0, -6.0, 9.0}});
  Matrix_Dense<3, 3> matrix_not_symmetric_static({{1.0, 2.0, 3.0},
                                                  {4.0, 5.0, 6.0},
                                                  {7.0, 8.0, 9.0}});
  Matrix_Dense<2, 3> matrix_not_square_0_static({{1.0, 2.0, 3.0},
                                                 {4.0, 5.0, 6.0}});
  Matrix_Dense<3, 2> matrix_not_square_1_static({{1.0, 2.0},
                                                 {3.0, 4.0},
                                                 {5.0, 6.0}});

  EXPECT_TRUE(is_symmetric(matrix_symmetric_dynamic));
  EXPECT_FALSE(is_symmetric(matrix_not_symmetric_dynamic));
  EXPECT_FALSE(is_symmetric(matrix_not_square_0_dynamic));
  EXPECT_FALSE(is_symmetric(matrix_not_square_1_dynamic));
  EXPECT_TRUE(is_symmetric(matrix_symmetric_static));
  EXPECT_FALSE(is_symmetric(matrix_not_symmetric_static));
  EXPECT_FALSE(is_symmetric(matrix_not_square_0_static));
  EXPECT_FALSE(is_symmetric(matrix_not_square_1_static));
}

TEST(test_matrix_sparse, is_symmetric_sparse) {
  Matrix_Sparse matrix_symmetric({0, 2, 5, 6}, {0, 1, 0, 1, 2, 1}, {1.0, 2.0, 2.0, 4.0, -5.0, -5.0}, 3);
  Matrix_Sparse matrix_not_symmetric({0, 2, 4, 6}, {0, 1, 0, 2, 1, 2}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0}, 3);
  Matrix_Sparse matrix_not_square_0({0, 2, 3}, {0, 1, 0}, {1.0, 2.0, 3.0}, 3);
  Matrix_Sparse matrix_not_square_1({0, 1, 3, 5}, {0, 0, 1, 0, 1}, {1.0, 2.0, 3.0, 4.0, 5.0}, 2);

  EXPECT_TRUE(is_symmetric(matrix_symmetric));
  EXPECT_FALSE(is_symmetric(matrix_not_symmetric));
  EXPECT_FALSE(is_symmetric(matrix_not_square_0));
  EXPECT_FALSE(is_symmetric(matrix_not_square_1));
}

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Operations
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_sparse, trace_dense) {

  // Dynamic matrix
  Matrix_Dense<0, 0> matrix_square_dynamic({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}});
  Matrix_Dense<0, 0> matrix_row_rectangular_dynamic({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});
  Matrix_Dense<0, 0> matrix_column_rectangular_dynamic({{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}});

  EXPECT_DOUBLE_EQ(trace(matrix_square_dynamic), 15.0);
  EXPECT_DOUBLE_EQ(trace(matrix_row_rectangular_dynamic), 6.0);
  EXPECT_DOUBLE_EQ(trace(matrix_column_rectangular_dynamic), 5.0);

  // Static matrix
  Matrix_Dense<3, 3> matrix_square_static({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}});
  Matrix_Dense<2, 3> matrix_row_rectangular_static({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});
  Matrix_Dense<3, 2> matrix_column_rectangular_static({{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}});

  EXPECT_DOUBLE_EQ(trace(matrix_square_static), 15.0);
  EXPECT_DOUBLE_EQ(trace(matrix_row_rectangular_static), 6.0);
  EXPECT_DOUBLE_EQ(trace(matrix_column_rectangular_static), 5.0);
}

TEST(test_matrix_sparse, trace_sparse) {

  Matrix_Sparse matrix_square({0, 2, 4, 6}, {0, 1, 0, 2, 1, 2}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0}, 3);
  Matrix_Sparse matrix_row_rectangular({0, 2, 3}, {0, 1, 0}, {1.0, 2.0, 3.0}, 3);
  Matrix_Sparse matrix_column_rectangular({0, 1, 3, 5}, {0, 0, 1, 0, 1}, {1.0, 2.0, 3.0, 4.0, 5.0}, 2);

  EXPECT_DOUBLE_EQ(trace(matrix_square), 7.0);
  EXPECT_DOUBLE_EQ(trace(matrix_row_rectangular), 1.0);
  EXPECT_DOUBLE_EQ(trace(matrix_column_rectangular), 4.0);
}
