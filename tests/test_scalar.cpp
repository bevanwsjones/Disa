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
// File Name: test_scalar.h
// Description: Unit tests for the header matrix_dense.h
// ---------------------------------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "scalar.h"

using namespace Disa;

// ---------------------------------------------------------------------------------------------------------------------
// Equality Checks
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Macro for is_nearly equal testing since a additional inequality checks need it in addition to another check.
 */
#define NEARLY_EQUAL_TESTS()                                                  \
  //case 1: norm is greater than max                                          \
  Scalar scalar_0 = 0.51*scalar_max;                                          \
  Scalar scalar_1 = scalar_max*(0.51 + default_relative - scalar_epsilon);    \
  EXPECT_TRUE(is_nearly_equal(scalar_0, scalar_1));                           \
  scalar_1 = scalar_max*(0.51 + default_relative);                            \
  EXPECT_FALSE(is_nearly_equal(scalar_0, scalar_1));                          \
                                                                              \
  // case 2: relative - positive                                              \
  scalar_0 = 10.0;                                                            \
  scalar_1 = 10.0*(1.0 - default_relative*scalar_epsilon);                    \
  EXPECT_TRUE(is_nearly_equal(scalar_0, scalar_1));                           \
  scalar_1 += 1.5*(scalar_0 + scalar_1)*default_relative;                     \
  EXPECT_FALSE(is_nearly_equal(scalar_0, scalar_1));                          \
                                                                              \
  // case 2: relative - negative                                              \
  scalar_0 = -10.0;                                                           \
  scalar_1 = -10.0*(1.0 + default_relative*scalar_epsilon);                   \
  EXPECT_TRUE(is_nearly_equal(scalar_0, scalar_1));                           \
  scalar_1 -= 1.5*(scalar_0 + scalar_1)*default_relative;                     \
  EXPECT_FALSE(is_nearly_equal(scalar_0, scalar_1));                          \
                                                                              \
  // case 3: absolute.                                                        \
  scalar_1 = default_absolute;                                                \
  EXPECT_FALSE(is_nearly_equal(0.0, scalar_1));                               \
  EXPECT_TRUE(is_nearly_equal(0.0, scalar_1 - scalar_epsilon));               \

/**
 * @brief wraps NEARLY_EQUAL_TESTS
 */
#define NEARLY_EQUAL_TEST() do {NEARLY_EQUAL_TESTS()} while(0)

TEST(test_scalar, is_nearly_equal) {
  NEARLY_EQUAL_TEST();
}

TEST(test_scalar, is_nearly_greater_equal){
    NEARLY_EQUAL_TEST();

  // Check greater than.
  Scalar scalar_0 = 10.0;
  Scalar scalar_1 = 10.0*(1.0 - default_relative*scalar_epsilon);
  EXPECT_TRUE(is_nearly_greater_equal(scalar_0, scalar_1));
  EXPECT_FALSE(is_nearly_greater_equal(-1.0*scalar_0, scalar_1));
}

TEST(test_scalar, is_nearly_less_equal){
  NEARLY_EQUAL_TEST();

  // Check greater than.
  Scalar scalar_0 = -10.0;
  Scalar scalar_1 = -10.0*(1.0 - default_relative*scalar_epsilon);
  EXPECT_TRUE(is_nearly_less_equal(scalar_0, scalar_1));
  EXPECT_FALSE(is_nearly_less_equal(-1.0*scalar_0, scalar_1));
}