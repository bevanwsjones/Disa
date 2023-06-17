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
#include "vector_operators.h"

#ifdef DISA_DEBUG

using namespace Disa;

TEST(test_vector_operators, lp_norm) {
  Vector_Dense<0> dynamic_vector = {1.0, -2.0, 3.0, -4.0};
  Vector_Dense<4> static_vector = {1.0, 2.0, -3.0, 4.0};

  EXPECT_DOUBLE_EQ(lp_norm<0>(dynamic_vector), 4.0);
  EXPECT_DOUBLE_EQ(lp_norm<0>(static_vector), 4.0);
  EXPECT_DOUBLE_EQ(lp_norm<1>(dynamic_vector), 10.0);
  EXPECT_DOUBLE_EQ(lp_norm<1>(static_vector), 10.0);
  EXPECT_DOUBLE_EQ(lp_norm<2>(dynamic_vector), std::sqrt(1.0 + 4.0 + 9.0 + 16.0));
  EXPECT_DOUBLE_EQ(lp_norm<2>(static_vector), std::sqrt(1.0 + 4.0 + 9.0 + 16.0));
  EXPECT_DOUBLE_EQ(lp_norm<3>(dynamic_vector), std::cbrt(1.0 + 8.0 + 27.0 + 64.0));
  EXPECT_DOUBLE_EQ(lp_norm<3>(static_vector), std::cbrt(1.0 + 8.0 + 27.0 + 64.0));
  EXPECT_DOUBLE_EQ(lp_norm<4>(dynamic_vector), std::pow(1.0 + 16.0 + 81.0 + 256.0, 0.25));
  EXPECT_DOUBLE_EQ(lp_norm<4>(static_vector), std::pow(1.0 + 16.0 + 81.0 + 256.0, 0.25));
}

TEST(test_vector_operators, mean) {
  Vector_Dense<0> dynamic_vector = {1.0, -2.0, 3.0, 4.0};
  Vector_Dense<4> static_vector = {1.0, -2.0, 3.0, -4.0};
  EXPECT_DOUBLE_EQ(mean(dynamic_vector), 6.0/4.0);
  EXPECT_DOUBLE_EQ(mean(static_vector), -2.0/4.0);

  Vector_Dense<0> zero_size;
  EXPECT_DEATH(mean(zero_size), "./*");
}

TEST(test_vector_operators, dot_product) {
  Vector_Dense<0> dynamic_vector_0 = {1.0, 0.0, 0.0};
  Vector_Dense<0> dynamic_vector_1 = {0.0, 1.0, 0.0};
  Vector_Dense<3> static_vector_0 = {1.0, 0.0, 0.0};
  Vector_Dense<3> static_vector_1 = {0.0, 1.0, 0.0};
  EXPECT_DOUBLE_EQ(dot_product(dynamic_vector_0, dynamic_vector_1), 0.0);
  EXPECT_DOUBLE_EQ(dot_product(dynamic_vector_0, static_vector_0), 1.0);
  EXPECT_DOUBLE_EQ(dot_product(static_vector_0, static_vector_1), 0.0);

  dynamic_vector_0 = {1.0, 2.0, 5.0};
  dynamic_vector_1 = {-2.0, -1.0, 6.0};
  static_vector_0 = {4.0, -3.0, 7.0};
  static_vector_1 = {1.0, -2.0, 2.0};
  EXPECT_DOUBLE_EQ(dot_product(dynamic_vector_0, dynamic_vector_1), 26.0);
  EXPECT_DOUBLE_EQ(dot_product(dynamic_vector_0, static_vector_0), 33.0);
  EXPECT_DOUBLE_EQ(dot_product(static_vector_0, static_vector_1), 24.0);

  EXPECT_DEATH(dot_product(dynamic_vector_0, Vector_Dense<0>({1.0, 2.0})), "./*");
  EXPECT_DEATH(dot_product(static_vector_0, Vector_Dense<0>({1.0, 2.0})), "./*");
}

TEST(test_vector_operators, unit) {
  Vector_Dense<0> dynamic_vector = {1, -2, 3};
  Vector_Dense<3> static_vector = {-1, 2, -3};
  dynamic_vector = unit(dynamic_vector);
  static_vector = unit(static_vector);

  EXPECT_DOUBLE_EQ(dynamic_vector[0], 1.0/std::sqrt(14.0));
  EXPECT_DOUBLE_EQ(dynamic_vector[1], -2.0/std::sqrt(14.0));
  EXPECT_DOUBLE_EQ(dynamic_vector[2], 3.0/std::sqrt(14.0));
  EXPECT_DOUBLE_EQ(static_vector[0], -1.0/std::sqrt(14.0));
  EXPECT_DOUBLE_EQ(static_vector[1], 2.0/std::sqrt(14.0));
  EXPECT_DOUBLE_EQ(static_vector[2], -3.0/std::sqrt(14.0));

  dynamic_vector *= 0.0;
  static_vector *= 0.0;
  dynamic_vector[1] = std::numeric_limits<double>::min();
  static_vector[2] = std::numeric_limits<double>::min();
  dynamic_vector = unit(dynamic_vector);
  static_vector = unit(static_vector);

  EXPECT_DOUBLE_EQ(dynamic_vector[0], 0.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[1], 0.0);
  EXPECT_DOUBLE_EQ(dynamic_vector[2], 0.0);
  EXPECT_DOUBLE_EQ(static_vector[0], 0.0);
  EXPECT_DOUBLE_EQ(static_vector[1], 0.0);
  EXPECT_DOUBLE_EQ(static_vector[2], 0.0);
}

TEST(test_vector_operators, angle) {
  Vector_Dense<0> dynamic_vector_0 = {1.0, 0.0, 0.0};
  Vector_Dense<0> dynamic_vector_1 = {0.0, 1.0, 0.0};
  Vector_Dense<3> static_vector_0 = {1.0, 0.0, 0.0};
  Vector_Dense<3> static_vector_1 = {0.0, 0.0, 1.0};

  double angle_radian = angle<0, 0, true>(dynamic_vector_0, dynamic_vector_1);
  double angle_degree = angle<0, 0, false>(dynamic_vector_0, dynamic_vector_1);
  EXPECT_DOUBLE_EQ(angle_radian, 0.5*std::numbers::pi);
  EXPECT_DOUBLE_EQ(angle_degree, 90.0);

  angle_radian = angle<3, 3, true>(static_vector_0, static_vector_1);
  angle_degree = angle<3, 3, false>(static_vector_0, static_vector_1);
  EXPECT_DOUBLE_EQ(angle_radian, 0.5*std::numbers::pi);
  EXPECT_DOUBLE_EQ(angle_degree, 90.0);

  angle_radian = angle<0, 3, true>(dynamic_vector_0, static_vector_1);
  angle_degree = angle<3, 0, false>(static_vector_1, dynamic_vector_1);
  EXPECT_DOUBLE_EQ(angle_radian, 0.5*std::numbers::pi);
  EXPECT_DOUBLE_EQ(angle_degree, 90.0);

  EXPECT_DEATH(cross_product(Vector_Dense<0>(), Vector_Dense<3>()), "./*");
  EXPECT_DEATH(cross_product(Vector_Dense<4>(), Vector_Dense<4>()), "./*");
}

TEST(test_vector_operators, cross_product) {
  Vector_Dense<0> dynamic_vector_0 = {1.0, 0.0, 0.0};
  Vector_Dense<0> dynamic_vector_1 = {0.0, 0.0, 1.0};
  Vector_Dense<3> static_vector_0 = {0.0, 1.0, 0.0};
  Vector_Dense<3> static_vector_1 = {0.0, 0.0, 1.0};

  Vector_Dense<0> dynamic_result = cross_product(dynamic_vector_0, dynamic_vector_1);
  Vector_Dense<3> static_result = cross_product(static_vector_0, static_vector_1);
  EXPECT_DOUBLE_EQ(dynamic_result[0], 0.0);
  EXPECT_DOUBLE_EQ(dynamic_result[1], -1.0);
  EXPECT_DOUBLE_EQ(dynamic_result[2], 0.0);
  EXPECT_DOUBLE_EQ(static_result[0], 1.0);
  EXPECT_DOUBLE_EQ(static_result[1], 0.0);
  EXPECT_DOUBLE_EQ(static_result[2], 0.0);

  dynamic_result = cross_product(dynamic_vector_1, dynamic_vector_0);
  static_result = cross_product(static_vector_1, static_vector_0);
  EXPECT_DOUBLE_EQ(dynamic_result[0], 0.0);
  EXPECT_DOUBLE_EQ(dynamic_result[1], 1.0);
  EXPECT_DOUBLE_EQ(dynamic_result[2], 0.0);
  EXPECT_DOUBLE_EQ(static_result[0], -1.0);
  EXPECT_DOUBLE_EQ(static_result[1], 0.0);
  EXPECT_DOUBLE_EQ(static_result[2], 0.0);

  static_result = cross_product(dynamic_vector_0, static_vector_0);
  EXPECT_DOUBLE_EQ(static_result[0], 0.0);
  EXPECT_DOUBLE_EQ(static_result[1], 0.0);
  EXPECT_DOUBLE_EQ(static_result[2], 1.0);

  Vector_Dense<0> dynamics_bad_size = {1.0, 2.0};
  EXPECT_DEATH(cross_product(dynamic_vector_0, dynamics_bad_size), "./*");
  EXPECT_DEATH(cross_product(static_vector_0, dynamics_bad_size), "./*");
  EXPECT_DEATH(cross_product(Vector_Dense<4>(), Vector_Dense<4>()), "./*");
}

TEST(test_vector_operators, tangent_projection) {
  Vector_Dense<0> dynamic_vector = {4.0, -2.0, 3.0};
  Vector_Dense<0> dynamic_unit = {1.0, 0.0, 0.0};
  Vector_Dense<3> static_vector = {-1.0, -2.0, -3.0};
  Vector_Dense<3> static_unit = {0.0, 1.0/std::sqrt(2.0), 1.0/std::sqrt(2.0)};

  Vector_Dense<0> dynamic_result = projection_tangent(dynamic_vector, dynamic_unit);
  Vector_Dense<3> static_result = projection_tangent(static_vector, static_unit);
  EXPECT_DOUBLE_EQ(dynamic_result[0], 4.0);
  EXPECT_DOUBLE_EQ(dynamic_result[1], 0.0);
  EXPECT_DOUBLE_EQ(dynamic_result[2], 0.0);
  EXPECT_DOUBLE_EQ(static_result[0], 0.0);
  EXPECT_DOUBLE_EQ(static_result[1], -2.5);
  EXPECT_DOUBLE_EQ(static_result[2], -2.5);

  Vector_Dense<0> dynamics_bad_size = {1.0, 2.0};
  Vector_Dense<2> static_bad_size = {1.0, 2.0};
  EXPECT_DEATH(projection_tangent(dynamic_vector, dynamics_bad_size), "./*");
  EXPECT_DEATH(projection_tangent(static_vector, static_bad_size), "./*");
}

TEST(test_vector_operators, orthogonal_projection) {
  Vector_Dense<0> dynamic_vector = {4.0, -2.0, 3.0};
  Vector_Dense<0> dynamic_unit = {1.0, 0.0, 0.0};
  Vector_Dense<3> static_vector = {-1.0, -2.0, -3.0};
  Vector_Dense<3> static_unit = {0.0, 1.0/std::sqrt(2.0), 1.0/std::sqrt(2.0)};

  Vector_Dense<0> dynamic_result = projection_orthogonal(dynamic_vector, dynamic_unit);
  Vector_Dense<3> static_result = projection_orthogonal(static_vector, static_unit);
  EXPECT_DOUBLE_EQ(dynamic_result[0], 0.0);
  EXPECT_DOUBLE_EQ(dynamic_result[1], -2.0);
  EXPECT_DOUBLE_EQ(dynamic_result[2], 3.0);
  EXPECT_DOUBLE_EQ(static_result[0], -1.0);
  EXPECT_NEAR(static_result[1], 0.5, std::numeric_limits<double>::epsilon()*10.0);
  EXPECT_DOUBLE_EQ(static_result[2], -0.5);

  EXPECT_DEATH(projection_orthogonal(dynamic_vector, Vector_Dense<0>({1.0, 2.0})), "./*");
  EXPECT_DEATH(projection_orthogonal(static_vector, Vector_Dense<2>({1.0, 2.0})), "./*");
}

#endif//DISA_DEBUG