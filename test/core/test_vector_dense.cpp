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
#include "vector_dense.hpp"

#ifdef DISA_DEBUG

using namespace Disa;

// -------------------------------------------------------------------------------------------------------------------
// Constructors/Destructors
// -------------------------------------------------------------------------------------------------------------------

TEST(test_vector_dense, constructors_initialiser_lists) {
  Vector_Dense<Scalar, 0> dynamic_vector = {1.0, 3.0, 4.0, -8.0};
  EXPECT_EQ(dynamic_vector.size(), 4);
  EXPECT_DOUBLE_EQ(dynamic_vector[0], 1.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[1], 3.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[2], 4.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[3], -8.0);

  Vector_Dense<Scalar, 3> static_vector = {-34.0, 56.0, 23.0};
  EXPECT_DOUBLE_EQ(static_vector[0], -34.0);
  EXPECT_DOUBLE_EQ(static_vector[1], 56.0);
  EXPECT_DOUBLE_EQ(static_vector[2], 23.0);

  EXPECT_DEATH((Vector_Dense<Scalar, 4>({-34.0, 56.0, 23.0})), "./*");
}

TEST(test_vector_dense, constructors_lambda) {
  Vector_Dense<Scalar, 0> dynamic_vector([](const std::size_t index) { return 2.0 * static_cast<Scalar>(index); }, 3);
  EXPECT_EQ(dynamic_vector.size(), 3);
  EXPECT_DOUBLE_EQ(dynamic_vector[0], 0.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[1], 2.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[2], 4.0);

  Vector_Dense<Scalar, 3> static_vector([](const std::size_t index) { return -3.0 * static_cast<Scalar>(index); });
  EXPECT_DOUBLE_EQ(static_vector[0], -0);
  EXPECT_DOUBLE_EQ(static_vector[1], -3);
  EXPECT_DOUBLE_EQ(static_vector[2], -6);
  EXPECT_DEATH((Vector_Dense<Scalar, 3>([](const std::size_t index) { return -3.0 * static_cast<Scalar>(index); }, 2)),
               "./*");
}

// -------------------------------------------------------------------------------------------------------------------
// Assignment Operators
// -------------------------------------------------------------------------------------------------------------------

TEST(test_vector_dense, scalar_multiplication_assignment) {
  Vector_Dense<Scalar, 0> dynamic_vector = {1.0, 2.0, 3.0};
  dynamic_vector *= -3.0;
  EXPECT_DOUBLE_EQ(dynamic_vector[0], -3.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[1], -6.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[2], -9.0);

  Vector_Dense<Scalar, 2> static_vector = {3.0, -5.0};
  static_vector *= 4.0;
  EXPECT_DOUBLE_EQ(static_vector[0], 12.0);
  EXPECT_DOUBLE_EQ(static_vector[1], -20.0);
}

TEST(test_vector_dense, scalar_division_assignment) {
  Vector_Dense<Scalar, 0> dynamic_vector = {3.0, 6.0, -5.0};
  dynamic_vector /= 3.0;
  EXPECT_DOUBLE_EQ(dynamic_vector[0], 1.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[1], 2.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[2], -5.0 / 3.0);

  Vector_Dense<Scalar, 2> static_vector = {3.0, -5.0};
  static_vector /= 4.0;
  EXPECT_DOUBLE_EQ(static_vector[0], 3.0 / 4.0);
  EXPECT_DOUBLE_EQ(static_vector[1], -5.0 / 4.0);
}

TEST(test_vector_dense, vector_addition_assignment) {
  Vector_Dense<Scalar, 0> dynamic_vector_0 = {1.0, 2.0, 3.0};
  Vector_Dense<Scalar, 0> dynamic_vector_1 = {-1.0, -2.0, -3.0};
  Vector_Dense<Scalar, 3> static_vector_0 = {4.0, 5.0, 6.0};
  Vector_Dense<Scalar, 3> static_vector_1 = {-4.0, -5.0, -6.0};

  // assignment of dynamic vectors
  dynamic_vector_0 += dynamic_vector_1;
  static_vector_0 += dynamic_vector_1;
  FOR_EACH(element, dynamic_vector_0) EXPECT_DOUBLE_EQ(element, 0.0);
  FOR_EACH(element, static_vector_0) EXPECT_DOUBLE_EQ(element, 3.0);

  // assignment of static vectors
  dynamic_vector_0 = {1.0, 2.0, 3.0};
  static_vector_0 = {4.0, 5.0, 6.0};
  dynamic_vector_0 += static_vector_1;
  static_vector_0 += static_vector_1;
  FOR_EACH(element, dynamic_vector_0) EXPECT_DOUBLE_EQ(element, -3.0);
  FOR_EACH(element, static_vector_0) EXPECT_DOUBLE_EQ(element, 0.0);

  Vector_Dense<Scalar, 0> dynamic_vector_2;
  Vector_Dense<Scalar, 2> static_vector_2;
  EXPECT_DEATH(dynamic_vector_0 += dynamic_vector_2, "./*");
  EXPECT_DEATH(static_vector_0 += dynamic_vector_2, "./*");
  EXPECT_DEATH(dynamic_vector_0 += static_vector_2, "./*");
  EXPECT_DEATH(static_vector_0 += static_vector_2, "./*");
}

TEST(test_vector_dense, vector_subtraction_assignment) {
  Vector_Dense<Scalar, 0> dynamic_vector_0 = {-1.0, -2.0, -3.0};
  Vector_Dense<Scalar, 0> dynamic_vector_1 = {-1.0, -2.0, -3.0};
  Vector_Dense<Scalar, 3> static_vector_0 = {-4.0, -5.0, -6.0};
  Vector_Dense<Scalar, 3> static_vector_1 = {-4.0, -5.0, -6.0};

  // assignment of dynamic vectors
  dynamic_vector_0 -= dynamic_vector_1;
  static_vector_0 -= dynamic_vector_1;
  FOR_EACH(element, dynamic_vector_0) EXPECT_DOUBLE_EQ(element, 0.0);
  FOR_EACH(element, static_vector_0) EXPECT_DOUBLE_EQ(element, -3.0);

  // assignment of static vectors
  dynamic_vector_0 = {-1.0, -2.0, -3.0};
  static_vector_0 = {-4.0, -5.0, -6.0};
  dynamic_vector_0 -= static_vector_1;
  static_vector_0 -= static_vector_1;
  FOR_EACH(element, dynamic_vector_0) EXPECT_DOUBLE_EQ(element, 3.0);
  FOR_EACH(element, static_vector_0) EXPECT_DOUBLE_EQ(element, 0.0);

  Vector_Dense<Scalar, 0> dynamic_vector_2;
  Vector_Dense<Scalar, 2> static_vector_2;
  EXPECT_DEATH(dynamic_vector_0 -= dynamic_vector_2, "./*");
  EXPECT_DEATH(static_vector_0 -= dynamic_vector_2, "./*");
  EXPECT_DEATH(dynamic_vector_0 -= static_vector_2, "./*");
  EXPECT_DEATH(static_vector_0 -= static_vector_2, "./*");
}

// ---------------------------------------------------------------------------------------------------------------------
// Arithmetic Operators
// ---------------------------------------------------------------------------------------------------------------------

TEST(test_vector_dense, scalar_multiplication) {
  Vector_Dense<Scalar, 0> dynamic_vector = {1.0, 2.0, 3.0};
  dynamic_vector = -3.0 * dynamic_vector;
  EXPECT_DOUBLE_EQ(dynamic_vector[0], -3.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[1], -6.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[2], -9.0);

  Vector_Dense<Scalar, 2> static_vector = {3.0, -5.0};
  static_vector = 4.0 * static_vector;
  EXPECT_DOUBLE_EQ(static_vector[0], 12.0);
  EXPECT_DOUBLE_EQ(static_vector[1], -20.0);
}

TEST(test_vector_dense, scalar_division) {
  Vector_Dense<Scalar, 0> dynamic_vector = {3.0, 6.0, -5.0};
  dynamic_vector = dynamic_vector / 3.0;
  EXPECT_DOUBLE_EQ(dynamic_vector[0], 1.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[1], 2.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[2], -5.0 / 3.0);

  Vector_Dense<Scalar, 2> static_vector = {3.0, -5.0};
  static_vector = static_vector / 4.0;
  EXPECT_DOUBLE_EQ(static_vector[0], 3.0 / 4.0);
  EXPECT_DOUBLE_EQ(static_vector[1], -5.0 / 4.0);
}

TEST(test_vector_dense, vector_addition) {
  Vector_Dense<Scalar, 0> dynamic_vector_0 = {1.0, 2.0, 3.0};
  Vector_Dense<Scalar, 0> dynamic_vector_1 = {-1.0, -2.0, -3.0};
  Vector_Dense<Scalar, 3> static_vector_0 = {4.0, 5.0, 6.0};
  Vector_Dense<Scalar, 3> static_vector_1 = {-4.0, -5.0, -6.0};

  Vector_Dense<Scalar, 0> result_0 = dynamic_vector_0 + dynamic_vector_1;
  Vector_Dense<Scalar, 3> result_1 = static_vector_0 + dynamic_vector_1;
  Vector_Dense<Scalar, 3> result_2 = dynamic_vector_0 + static_vector_1;
  Vector_Dense<Scalar, 3> result_3 = static_vector_0 + static_vector_1;
  FOR_EACH(element, result_0) EXPECT_DOUBLE_EQ(element, 0.0);
  FOR_EACH(element, result_1) EXPECT_DOUBLE_EQ(element, 3.0);
  FOR_EACH(element, result_2) EXPECT_DOUBLE_EQ(element, -3.0);
  FOR_EACH(element, result_3) EXPECT_DOUBLE_EQ(element, 0.0);

  Vector_Dense<Scalar, 0> dynamic_vector_2;
  Vector_Dense<Scalar, 2> static_vector_2;
  EXPECT_DEATH(dynamic_vector_0 + dynamic_vector_2, "./*");
  EXPECT_DEATH(static_vector_0 + dynamic_vector_2, "./*");
  EXPECT_DEATH(dynamic_vector_0 + static_vector_2, "./*");
  EXPECT_DEATH(static_vector_0 + static_vector_2, "./*");
}

TEST(test_vector_dense, vector_subtraction) {
  Vector_Dense<Scalar, 0> dynamic_vector_0 = {-1.0, -2.0, -3.0};
  Vector_Dense<Scalar, 0> dynamic_vector_1 = {-1.0, -2.0, -3.0};
  Vector_Dense<Scalar, 3> static_vector_0 = {-4.0, -5.0, -6.0};
  Vector_Dense<Scalar, 3> static_vector_1 = {-4.0, -5.0, -6.0};

  Vector_Dense<Scalar, 0> result_0 = dynamic_vector_0 - dynamic_vector_1;
  Vector_Dense<Scalar, 3> result_1 = static_vector_0 - dynamic_vector_1;
  Vector_Dense<Scalar, 3> result_2 = dynamic_vector_0 - static_vector_1;
  Vector_Dense<Scalar, 3> result_3 = static_vector_0 - static_vector_1;
  FOR_EACH(element, result_0) EXPECT_DOUBLE_EQ(element, 0.0);
  FOR_EACH(element, result_1) EXPECT_DOUBLE_EQ(element, -3.0);
  FOR_EACH(element, result_2) EXPECT_DOUBLE_EQ(element, 3.0);
  FOR_EACH(element, result_3) EXPECT_DOUBLE_EQ(element, 0.0);

  Vector_Dense<Scalar, 0> dynamic_vector_2;
  Vector_Dense<Scalar, 2> static_vector_2;
  EXPECT_DEATH(dynamic_vector_0 - dynamic_vector_2, "./*");
  EXPECT_DEATH(static_vector_0 - dynamic_vector_2, "./*");
  EXPECT_DEATH(dynamic_vector_0 - static_vector_2, "./*");
  EXPECT_DEATH(static_vector_0 - static_vector_2, "./*");
}

#endif  //DISA_DEBUG
