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

//----------------------------------------------------------------------------------------------------------------------
// Assignment Operators
//----------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_dense, scalar_matrix_multiplication_assignment) {
  Matrix_Dense<0, 0> dynamic_matrix = {{1, 2, 3},
                                       {4, 5, 6},
                                       {7, 8, 9}};
  Matrix_Dense<3, 3> static_matrix = {{10, 20, 30},
                                      {40, 50, 60},
                                      {70, 80, 90}};

  dynamic_matrix *= -10.0;
  EXPECT_DOUBLE_EQ(dynamic_matrix[0][0], -1.0*static_matrix[0][0]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[0][1], -1.0*static_matrix[0][1]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[0][2], -1.0*static_matrix[0][2]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[1][0], -1.0*static_matrix[1][0]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[1][1], -1.0*static_matrix[1][1]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[1][2], -1.0*static_matrix[1][2]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[2][0], -1.0*static_matrix[2][0]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[2][1], -1.0*static_matrix[2][1]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[2][2], -1.0*static_matrix[2][2]);

  static_matrix *= 0.1;
  EXPECT_DOUBLE_EQ(-0.1*dynamic_matrix[0][0], static_matrix[0][0]);
  EXPECT_DOUBLE_EQ(-0.1*dynamic_matrix[0][1], static_matrix[0][1]);
  EXPECT_DOUBLE_EQ(-0.1*dynamic_matrix[0][2], static_matrix[0][2]);
  EXPECT_DOUBLE_EQ(-0.1*dynamic_matrix[1][0], static_matrix[1][0]);
  EXPECT_DOUBLE_EQ(-0.1*dynamic_matrix[1][1], static_matrix[1][1]);
  EXPECT_DOUBLE_EQ(-0.1*dynamic_matrix[1][2], static_matrix[1][2]);
  EXPECT_DOUBLE_EQ(-0.1*dynamic_matrix[2][0], static_matrix[2][0]);
  EXPECT_DOUBLE_EQ(-0.1*dynamic_matrix[2][1], static_matrix[2][1]);
  EXPECT_DOUBLE_EQ(-0.1*dynamic_matrix[2][2], static_matrix[2][2]);
}

TEST(test_matrix_dense, scalar_matrix_division_assignment) {
  Matrix_Dense<0, 0> dynamic_matrix = {{1, 2, 3},
                                       {4, 5, 6},
                                       {7, 8, 9}};
  Matrix_Dense<3, 3> static_matrix = {{10, 20, 30},
                                      {40, 50, 60},
                                      {70, 80, 90}};

  static_matrix /= -10.0;
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[0][0], static_matrix[0][0]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[0][1], static_matrix[0][1]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[0][2], static_matrix[0][2]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[1][0], static_matrix[1][0]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[1][1], static_matrix[1][1]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[1][2], static_matrix[1][2]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[2][0], static_matrix[2][0]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[2][1], static_matrix[2][1]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[2][2], static_matrix[2][2]);

  dynamic_matrix /= 0.1;
  EXPECT_DOUBLE_EQ(dynamic_matrix[0][0], -10.0*static_matrix[0][0]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[0][1], -10.0*static_matrix[0][1]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[0][2], -10.0*static_matrix[0][2]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[1][0], -10.0*static_matrix[1][0]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[1][1], -10.0*static_matrix[1][1]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[1][2], -10.0*static_matrix[1][2]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[2][0], -10.0*static_matrix[2][0]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[2][1], -10.0*static_matrix[2][1]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[2][2], -10.0*static_matrix[2][2]);
}

TEST(test_matrix_dense, matrix_matrix_addition_assignment) {
  Matrix_Dense<0, 0> dynamic_matrix_0 = {{1, 2},
                                         {3, 4}};
  Matrix_Dense<0, 0> dynamic_matrix_1 = {{4, 3},
                                         {2, 1}};
  Matrix_Dense<2, 2> static_matrix_0 = {{10, 20},
                                        {30, 40}};
  Matrix_Dense<2, 2> static_matrix_1 = {{40, 30},
                                        {20, 10}};

  dynamic_matrix_0 += dynamic_matrix_1;
  static_matrix_0 += static_matrix_1;
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[0][0], 5);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[0][1], 5);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[1][0], 5);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[1][1], 5);
  EXPECT_DOUBLE_EQ(static_matrix_0[0][0], 50);
  EXPECT_DOUBLE_EQ(static_matrix_0[0][1], 50);
  EXPECT_DOUBLE_EQ(static_matrix_0[1][0], 50);
  EXPECT_DOUBLE_EQ(static_matrix_0[1][1], 50);

  dynamic_matrix_1 += static_matrix_0;
  static_matrix_1 += dynamic_matrix_0;
  EXPECT_DOUBLE_EQ(dynamic_matrix_1[0][0], 54);
  EXPECT_DOUBLE_EQ(dynamic_matrix_1[0][1], 53);
  EXPECT_DOUBLE_EQ(dynamic_matrix_1[1][0], 52);
  EXPECT_DOUBLE_EQ(dynamic_matrix_1[1][1], 51);
  EXPECT_DOUBLE_EQ(static_matrix_1[0][0], 45);
  EXPECT_DOUBLE_EQ(static_matrix_1[0][1], 35);
  EXPECT_DOUBLE_EQ(static_matrix_1[1][0], 25);
  EXPECT_DOUBLE_EQ(static_matrix_1[1][1], 15);

  Matrix_Dense<0, 0> dynamic_incorrect;
  Matrix_Dense<2, 3> static_incorrect;
  EXPECT_DEATH(dynamic_matrix_0 += dynamic_incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_0 += static_incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_0 += dynamic_incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_0 += static_incorrect, "./*");
}

TEST(test_matrix_dense, matrix_matrix_subtraction_assignment) {
  Matrix_Dense<0, 0> dynamic_matrix_0 = {{1, 2},
                                         {3, 4}};
  Matrix_Dense<0, 0> dynamic_matrix_1 = {{2, 2},
                                         {3, 3}};
  Matrix_Dense<2, 2> static_matrix_0 = {{10, 20},
                                        {30, 40}};
  Matrix_Dense<2, 2> static_matrix_1 = {{20, 20},
                                        {30, 30}};

  dynamic_matrix_0 -= dynamic_matrix_1;
  static_matrix_0 -= static_matrix_1;
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[0][0], -1);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[0][1], 0);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[1][0], 0);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[1][1], 1);
  EXPECT_DOUBLE_EQ(static_matrix_0[0][0], -10);
  EXPECT_DOUBLE_EQ(static_matrix_0[0][1], 0);
  EXPECT_DOUBLE_EQ(static_matrix_0[1][0], 0);
  EXPECT_DOUBLE_EQ(static_matrix_0[1][1], 10);

  dynamic_matrix_1 -= static_matrix_0;
  static_matrix_1 -= dynamic_matrix_0;
  EXPECT_DOUBLE_EQ(dynamic_matrix_1[0][0], 12);
  EXPECT_DOUBLE_EQ(dynamic_matrix_1[0][1], 2);
  EXPECT_DOUBLE_EQ(dynamic_matrix_1[1][0], 3);
  EXPECT_DOUBLE_EQ(dynamic_matrix_1[1][1], -7);
  EXPECT_DOUBLE_EQ(static_matrix_1[0][0], 21);
  EXPECT_DOUBLE_EQ(static_matrix_1[0][1], 20);
  EXPECT_DOUBLE_EQ(static_matrix_1[1][0], 30);
  EXPECT_DOUBLE_EQ(static_matrix_1[1][1], 29);

  Matrix_Dense<0, 0> dynamic_incorrect;
  Matrix_Dense<2, 3> static_incorrect;
  EXPECT_DEATH(dynamic_matrix_0 -= dynamic_incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_0 -= static_incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_0 -= dynamic_incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_0 -= static_incorrect, "./*");
}

TEST(test_matrix_dense, matrix_matrix_multiplication_assignment) {
  Matrix_Dense<0, 0> dynamic_matrix_0 = {{1, 2},
                                         {3, 4}};
  Matrix_Dense<0, 0> dynamic_matrix_1 = {{5, 6},
                                         {7, 8}};
  Matrix_Dense<2, 2> static_matrix_0 = {{1, 2},
                                        {3, 4}};
  Matrix_Dense<2, 2> static_matrix_1 = {{5, 6},
                                        {7, 8}};

  dynamic_matrix_0 *= dynamic_matrix_1;
  static_matrix_0 *= static_matrix_1;
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[0][0], 19);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[0][1], 22);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[1][0], 43);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[1][1], 50);
  EXPECT_DOUBLE_EQ(static_matrix_0[0][0], 19);
  EXPECT_DOUBLE_EQ(static_matrix_0[0][1], 22);
  EXPECT_DOUBLE_EQ(static_matrix_0[1][0], 43);
  EXPECT_DOUBLE_EQ(static_matrix_0[1][1], 50);

  // reset
  dynamic_matrix_0 = {{1, 2},
                      {3, 4}};
  static_matrix_0 = {{1, 2},
                     {3, 4}};
  dynamic_matrix_0 *= static_matrix_1;
  static_matrix_0 *= dynamic_matrix_1;
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[0][0], 19);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[0][1], 22);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[1][0], 43);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[1][1], 50);
  EXPECT_DOUBLE_EQ(static_matrix_0[0][0], 19);
  EXPECT_DOUBLE_EQ(static_matrix_0[0][1], 22);
  EXPECT_DOUBLE_EQ(static_matrix_0[1][0], 43);
  EXPECT_DOUBLE_EQ(static_matrix_0[1][1], 50);

  // non-square
  dynamic_matrix_0 = {{1, 2},
                      {3, 4},
                      {5, 6}};
  dynamic_matrix_1 = {{7,  8,  9,  10},
                      {11, 12, 13, 14}};
  dynamic_matrix_0 *= dynamic_matrix_1;
  EXPECT_EQ(dynamic_matrix_0.size_row(), 3);
  EXPECT_EQ(dynamic_matrix_0.size_column(), 4);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[0][0], 29);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[0][1], 32);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[0][2], 35);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[0][3], 38);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[1][0], 65);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[1][1], 72);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[1][2], 79);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[1][3], 86);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[2][0], 101);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[2][1], 112);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[2][2], 123);
  EXPECT_DOUBLE_EQ(dynamic_matrix_0[2][3], 134);

  dynamic_matrix_0.resize(1, 1);
  Matrix_Dense<2, 3> incorrect;
  EXPECT_DEATH(dynamic_matrix_1 *= incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_1 *= dynamic_matrix_0, "./*");
  EXPECT_DEATH(static_matrix_0 *= incorrect, "./*");
  EXPECT_DEATH(static_matrix_0 *= dynamic_matrix_0, "./*");
}

//----------------------------------------------------------------------------------------------------------------------
// Arithmetic Operators
//----------------------------------------------------------------------------------------------------------------------

TEST(test_matrix_dense, scalar_matrix_multiplication) {
  Matrix_Dense<0, 0> dynamic_matrix = {{1, 2, 3},
                                       {4, 5, 6},
                                       {7, 8, 9}};
  Matrix_Dense<3, 3> static_matrix = {{10, 20, 30},
                                      {40, 50, 60},
                                      {70, 80, 90}};

  Matrix_Dense<0, 0> result_dynamic = -10.0*dynamic_matrix;
  EXPECT_DOUBLE_EQ(result_dynamic[0][0], -1.0*static_matrix[0][0]);
  EXPECT_DOUBLE_EQ(result_dynamic[0][1], -1.0*static_matrix[0][1]);
  EXPECT_DOUBLE_EQ(result_dynamic[0][2], -1.0*static_matrix[0][2]);
  EXPECT_DOUBLE_EQ(result_dynamic[1][0], -1.0*static_matrix[1][0]);
  EXPECT_DOUBLE_EQ(result_dynamic[1][1], -1.0*static_matrix[1][1]);
  EXPECT_DOUBLE_EQ(result_dynamic[1][2], -1.0*static_matrix[1][2]);
  EXPECT_DOUBLE_EQ(result_dynamic[2][0], -1.0*static_matrix[2][0]);
  EXPECT_DOUBLE_EQ(result_dynamic[2][1], -1.0*static_matrix[2][1]);
  EXPECT_DOUBLE_EQ(result_dynamic[2][2], -1.0*static_matrix[2][2]);

  Matrix_Dense<3, 3> result_static = 0.1*static_matrix;
  EXPECT_DOUBLE_EQ(dynamic_matrix[0][0], result_static[0][0]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[0][1], result_static[0][1]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[0][2], result_static[0][2]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[1][0], result_static[1][0]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[1][1], result_static[1][1]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[1][2], result_static[1][2]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[2][0], result_static[2][0]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[2][1], result_static[2][1]);
  EXPECT_DOUBLE_EQ(dynamic_matrix[2][2], result_static[2][2]);
}

TEST(test_matrix_dense, scalar_matrix_division) {
  Matrix_Dense<0, 0> dynamic_matrix = {{1, 2, 3},
                                       {4, 5, 6},
                                       {7, 8, 9}};
  Matrix_Dense<3, 3> static_matrix = {{10, 20, 30},
                                      {40, 50, 60},
                                      {70, 80, 90}};

  Matrix_Dense<3, 3> result_static = static_matrix/-10.0;
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[0][0], result_static[0][0]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[0][1], result_static[0][1]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[0][2], result_static[0][2]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[1][0], result_static[1][0]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[1][1], result_static[1][1]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[1][2], result_static[1][2]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[2][0], result_static[2][0]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[2][1], result_static[2][1]);
  EXPECT_DOUBLE_EQ(-1.0*dynamic_matrix[2][2], result_static[2][2]);

  Matrix_Dense<0, 0> result_dynamic = -10.0*dynamic_matrix;
  EXPECT_DOUBLE_EQ(result_dynamic[0][0], -1.0*static_matrix[0][0]);
  EXPECT_DOUBLE_EQ(result_dynamic[0][1], -1.0*static_matrix[0][1]);
  EXPECT_DOUBLE_EQ(result_dynamic[0][2], -1.0*static_matrix[0][2]);
  EXPECT_DOUBLE_EQ(result_dynamic[1][0], -1.0*static_matrix[1][0]);
  EXPECT_DOUBLE_EQ(result_dynamic[1][1], -1.0*static_matrix[1][1]);
  EXPECT_DOUBLE_EQ(result_dynamic[1][2], -1.0*static_matrix[1][2]);
  EXPECT_DOUBLE_EQ(result_dynamic[2][0], -1.0*static_matrix[2][0]);
  EXPECT_DOUBLE_EQ(result_dynamic[2][1], -1.0*static_matrix[2][1]);
  EXPECT_DOUBLE_EQ(result_dynamic[2][2], -1.0*static_matrix[2][2]);
}

TEST(test_matrix_dense, matrix_vector_multiplication) {
  Matrix_Dense<0, 0> dynamic_matrix = {{1, 2},
                                       {3, 4}};
  Matrix_Dense<2, 2> static_matrix = {{10, 20},
                                      {30, 40}};
  Vector_Dense<0> dynamic_vector = {-1, 2};
  Vector_Dense<2> static_vector = {-10, 20};

  Vector_Dense<0> result_dynamic = dynamic_matrix*dynamic_vector;
  Vector_Dense<2> result_static_0 = static_matrix*static_vector;
  EXPECT_DOUBLE_EQ(result_dynamic[0], 3);
  EXPECT_DOUBLE_EQ(result_dynamic[1], 5);
  EXPECT_DOUBLE_EQ(result_static_0[0], 300);
  EXPECT_DOUBLE_EQ(result_static_0[1], 500);

  result_static_0 = dynamic_matrix*static_vector;
  Vector_Dense<2> result_static_1 = static_matrix*dynamic_vector;
  EXPECT_DOUBLE_EQ(result_static_0[0], 30);
  EXPECT_DOUBLE_EQ(result_static_0[1], 50);
  EXPECT_DOUBLE_EQ(result_static_1[0], 30);
  EXPECT_DOUBLE_EQ(result_static_1[1], 50);

  Matrix_Dense<0, 0> dynamic_matrix_incorrect;
  Matrix_Dense<3, 3> static_matrix_incorrect;
  Vector_Dense<0> dynamic_vector_incorrect;
  Vector_Dense<3> static_vector_incorrect;
  EXPECT_DEATH(dynamic_matrix_incorrect*dynamic_vector, "./*");
  EXPECT_DEATH(dynamic_matrix_incorrect*static_vector, "./*");
  EXPECT_DEATH(static_matrix_incorrect*dynamic_vector, "./*");
  EXPECT_DEATH(static_matrix_incorrect*static_vector, "./*");
  EXPECT_DEATH(dynamic_matrix*dynamic_vector_incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix*static_vector_incorrect, "./*");
  EXPECT_DEATH(static_matrix*dynamic_vector_incorrect, "./*");
  EXPECT_DEATH(static_matrix*static_vector_incorrect, "./*");
}

TEST(test_matrix_dense, matrix_matrix_addition) {
  Matrix_Dense<0, 0> dynamic_matrix_0 = {{1, 2},
                                         {3, 4}};
  Matrix_Dense<0, 0> dynamic_matrix_1 = {{4, 3},
                                         {2, 1}};
  Matrix_Dense<2, 2> static_matrix_0 = {{10, 20},
                                        {30, 40}};
  Matrix_Dense<2, 2> static_matrix_1 = {{40, 30},
                                        {20, 10}};

  Matrix_Dense<0, 0> result_dynamic = dynamic_matrix_0 + dynamic_matrix_1;
  Matrix_Dense<2, 2> result_static_0 = static_matrix_0 + static_matrix_1;
  EXPECT_DOUBLE_EQ(result_dynamic[0][0], 5);
  EXPECT_DOUBLE_EQ(result_dynamic[0][1], 5);
  EXPECT_DOUBLE_EQ(result_dynamic[1][0], 5);
  EXPECT_DOUBLE_EQ(result_dynamic[1][1], 5);
  EXPECT_DOUBLE_EQ(result_static_0[0][0], 50);
  EXPECT_DOUBLE_EQ(result_static_0[0][1], 50);
  EXPECT_DOUBLE_EQ(result_static_0[1][0], 50);
  EXPECT_DOUBLE_EQ(result_static_0[1][1], 50);

  result_static_0 = dynamic_matrix_1 + static_matrix_0;
  Matrix_Dense<2, 2> result_static_1 = static_matrix_1 + dynamic_matrix_0;
  EXPECT_DOUBLE_EQ(result_static_0[0][0], 14);
  EXPECT_DOUBLE_EQ(result_static_0[0][1], 23);
  EXPECT_DOUBLE_EQ(result_static_0[1][0], 32);
  EXPECT_DOUBLE_EQ(result_static_0[1][1], 41);
  EXPECT_DOUBLE_EQ(result_static_1[0][0], 41);
  EXPECT_DOUBLE_EQ(result_static_1[0][1], 32);
  EXPECT_DOUBLE_EQ(result_static_1[1][0], 23);
  EXPECT_DOUBLE_EQ(result_static_1[1][1], 14);

  Matrix_Dense<0, 0> dynamic_incorrect;
  Matrix_Dense<2, 3> static_incorrect;
  EXPECT_DEATH(dynamic_matrix_0 + dynamic_incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_0 + static_incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_0 + dynamic_incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_0 + static_incorrect, "./*");
}

TEST(test_matrix_dense, matrix_matrix_subtraction) {
  Matrix_Dense<0, 0> dynamic_matrix_0 = {{1, 2},
                                         {3, 4}};
  Matrix_Dense<0, 0> dynamic_matrix_1 = {{4, 3},
                                         {2, 1}};
  Matrix_Dense<2, 2> static_matrix_0 = {{10, 20},
                                        {30, 40}};
  Matrix_Dense<2, 2> static_matrix_1 = {{40, 30},
                                        {20, 10}};

  Matrix_Dense<0, 0> result_dynamic = dynamic_matrix_0 - dynamic_matrix_1;
  Matrix_Dense<2, 2> result_static_0 = static_matrix_0 - static_matrix_1;
  EXPECT_DOUBLE_EQ(result_dynamic[0][0], -3);
  EXPECT_DOUBLE_EQ(result_dynamic[0][1], -1);
  EXPECT_DOUBLE_EQ(result_dynamic[1][0], 1);
  EXPECT_DOUBLE_EQ(result_dynamic[1][1], 3);
  EXPECT_DOUBLE_EQ(result_static_0[0][0], -30);
  EXPECT_DOUBLE_EQ(result_static_0[0][1], -10);
  EXPECT_DOUBLE_EQ(result_static_0[1][0], 10);
  EXPECT_DOUBLE_EQ(result_static_0[1][1], 30);

  result_static_0 = dynamic_matrix_1 - static_matrix_0;
  Matrix_Dense<2, 2> result_static_1 = static_matrix_1 - dynamic_matrix_0;
  EXPECT_DOUBLE_EQ(result_static_0[0][0], -6);
  EXPECT_DOUBLE_EQ(result_static_0[0][1], -17);
  EXPECT_DOUBLE_EQ(result_static_0[1][0], -28);
  EXPECT_DOUBLE_EQ(result_static_0[1][1], -39);
  EXPECT_DOUBLE_EQ(result_static_1[0][0], 39);
  EXPECT_DOUBLE_EQ(result_static_1[0][1], 28);
  EXPECT_DOUBLE_EQ(result_static_1[1][0], 17);
  EXPECT_DOUBLE_EQ(result_static_1[1][1], 6);

  Matrix_Dense<0, 0> dynamic_incorrect;
  Matrix_Dense<2, 3> static_incorrect;
  EXPECT_DEATH(dynamic_matrix_0 - dynamic_incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_0 - static_incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_0 - dynamic_incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_0 - static_incorrect, "./*");
}

TEST(test_matrix_dense, matrix_matrix_multiplication) {
  Matrix_Dense<0, 0> dynamic_matrix_0 = {{1, 2},
                                         {3, 4}};
  Matrix_Dense<0, 0> dynamic_matrix_1 = {{5, 6},
                                         {7, 8}};
  Matrix_Dense<2, 2> static_matrix_0 = {{1, 2},
                                        {3, 4}};
  Matrix_Dense<2, 2> static_matrix_1 = {{5, 6},
                                        {7, 8}};

  Matrix_Dense<0, 0> result_dynamic_0 = dynamic_matrix_0*dynamic_matrix_1;
  Matrix_Dense<2, 2> result_static = static_matrix_0*static_matrix_1;
  EXPECT_EQ(dynamic_matrix_0.size_row(), 2);
  EXPECT_EQ(dynamic_matrix_0.size_column(), 2);
  EXPECT_DOUBLE_EQ(result_dynamic_0[0][0], 19);
  EXPECT_DOUBLE_EQ(result_dynamic_0[0][1], 22);
  EXPECT_DOUBLE_EQ(result_dynamic_0[1][0], 43);
  EXPECT_DOUBLE_EQ(result_dynamic_0[1][1], 50);
  EXPECT_DOUBLE_EQ(result_static[0][0], 19);
  EXPECT_DOUBLE_EQ(result_static[0][1], 22);
  EXPECT_DOUBLE_EQ(result_static[1][0], 43);
  EXPECT_DOUBLE_EQ(result_static[1][1], 50);

  result_dynamic_0 = dynamic_matrix_0*static_matrix_1;
  Matrix_Dense<0, 0> result_dynamic_1 = static_matrix_0*dynamic_matrix_1;
  EXPECT_EQ(dynamic_matrix_0.size_row(), 2);
  EXPECT_EQ(dynamic_matrix_0.size_column(), 2);
  EXPECT_DOUBLE_EQ(result_dynamic_0[0][0], 19);
  EXPECT_DOUBLE_EQ(result_dynamic_0[0][1], 22);
  EXPECT_DOUBLE_EQ(result_dynamic_0[1][0], 43);
  EXPECT_DOUBLE_EQ(result_dynamic_0[1][1], 50);
  EXPECT_EQ(dynamic_matrix_0.size_row(), 2);
  EXPECT_EQ(dynamic_matrix_0.size_column(), 2);
  EXPECT_DOUBLE_EQ(result_dynamic_1[0][0], 19);
  EXPECT_DOUBLE_EQ(result_dynamic_1[0][1], 22);
  EXPECT_DOUBLE_EQ(result_dynamic_1[1][0], 43);
  EXPECT_DOUBLE_EQ(result_dynamic_1[1][1], 50);

  // non-square
  dynamic_matrix_0 = {{1, 2},
                      {3, 4},
                      {5, 6}};
  dynamic_matrix_1 = {{7,  8,  9,  10},
                      {11, 12, 13, 14}};
  result_dynamic_0 = dynamic_matrix_0*dynamic_matrix_1;
  EXPECT_EQ(result_dynamic_0.size_row(), 3);
  EXPECT_EQ(result_dynamic_0.size_column(), 4);
  EXPECT_DOUBLE_EQ(result_dynamic_0[0][0], 29);
  EXPECT_DOUBLE_EQ(result_dynamic_0[0][1], 32);
  EXPECT_DOUBLE_EQ(result_dynamic_0[0][2], 35);
  EXPECT_DOUBLE_EQ(result_dynamic_0[0][3], 38);
  EXPECT_DOUBLE_EQ(result_dynamic_0[1][0], 65);
  EXPECT_DOUBLE_EQ(result_dynamic_0[1][1], 72);
  EXPECT_DOUBLE_EQ(result_dynamic_0[1][2], 79);
  EXPECT_DOUBLE_EQ(result_dynamic_0[1][3], 86);
  EXPECT_DOUBLE_EQ(result_dynamic_0[2][0], 101);
  EXPECT_DOUBLE_EQ(result_dynamic_0[2][1], 112);
  EXPECT_DOUBLE_EQ(result_dynamic_0[2][2], 123);
  EXPECT_DOUBLE_EQ(result_dynamic_0[2][3], 134);

  dynamic_matrix_0.resize(1, 1);
  Matrix_Dense<2, 3> incorrect;
  EXPECT_DEATH(dynamic_matrix_1*incorrect, "./*");
  EXPECT_DEATH(dynamic_matrix_1*dynamic_matrix_0, "./*");
  EXPECT_DEATH(static_matrix_0*incorrect, "./*");
  EXPECT_DEATH(static_matrix_0*dynamic_matrix_0, "./*");
}

#endif//DISA_DEBUG