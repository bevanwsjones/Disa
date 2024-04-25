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
// File Name: Scalar.hpp
// Description: Contains the declaration and definitions of 'macro' like globals which help with floating point
//              operations, when used as mathematical scalar.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_SCALAR_H
#define DISA_SCALAR_H

#include "macros.hpp"

#include <limits>

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Aliasing and Definitions
// ---------------------------------------------------------------------------------------------------------------------

using Scalar = double;
inline constexpr Scalar scalar_max_digits10 = std::numeric_limits<Scalar>::max_digits10;   //!< Alias for scalar max_digits10

inline constexpr Scalar scalar_epsilon = std::numeric_limits<Scalar>::epsilon();     //!< Alias for scalar epsilon
inline constexpr Scalar scalar_infinity = std::numeric_limits<Scalar>::infinity();   //!< Alias for scalar infinity
inline constexpr Scalar scalar_lowest = std::numeric_limits<Scalar>::lowest();       //!< Alias for scalar lowest
inline constexpr Scalar scalar_max = std::numeric_limits<Scalar>::max();             //!< Alias for scalar max
inline constexpr Scalar scalar_min = std::numeric_limits<Scalar>::min();             //!< Alias for scalar min

inline constexpr Scalar default_absolute = static_cast<Scalar>(64)*scalar_epsilon;       //!< Global for default absolute equality check, 'reasonably over 2 orders of epsilon.
inline constexpr Scalar default_relative = static_cast<Scalar>(65536)*scalar_epsilon;    //!< Global for default relative equality check, 'reasonably over 4 orders of epsilon.

// ---------------------------------------------------------------------------------------------------------------------
// Equality Checks
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Checks if two scalars are 'almost' equal to each other, using tolerancing. I.e. definition of an operator=~.
 * @param[in] scalar_0 The first scalar value, s_0, to compare.
 * @param[in] scalar_1 The second scalar value, s_1, to compare.
 * @param[in] tolerance_relative Tolerance value, tol_rel, below which two the normalised scalars are considered equal.
 * @param[in] tolerance_absolute Cutoff tolerance value, tol_abs, below which two the scalars are considered equal.
 * @return True if the scalars are nearly equal to each other, s_0 =~ s_1.
 *
 * @details A number of floating point equality check cases are handled by the implementation used here, most
 * programmers should be familiar with the issues surrounding floating point equality checks, without needing to go into
 * too much depth here. Below is a mathematical description of the list of the cases that are considered here followed
 * by a link to a very useful discussion and the solution for equality checking regarding floating point numbers.
 *
 * Cases:
 * 1. norm > scalar_max             : |s_0 - s_1|/max_scalar < tol_rel
 * 2. eps_abs < norm < scalar_max   : |s_0 - s_1|/norm < tol_rel
 * 3. eps_abs > norm                : |s_0 - s_1| < tol_abs
 * where
 * - norm = |s_0| + |s_1|
 * - scalar_max = std::numeric_limits<Scalar>::max(), maximum floating point representation of scalar values.
 *
 * Note: Solution taken from https://stackoverflow.com/questions/4915462/how-should-i-do-floating-point-comparison
 */
[[nodiscard]] constexpr bool is_nearly_equal(const Scalar& scalar_0, const Scalar& scalar_1,
                                             const Scalar& tolerance_relative = default_relative,
                                             const Scalar& tolerance_absolute = default_absolute) {
  ASSERT_DEBUG(scalar_epsilon <= tolerance_relative,
               "Relative tolerance " + std::to_string(tolerance_relative) + " must be greater than scalar epsilon, "
               + std::to_string(scalar_epsilon) + ".");
  ASSERT_DEBUG(tolerance_relative <= static_cast<Scalar>(1),
               "Relative tolerance " + std::to_string(tolerance_relative) + "  greater than 1, will magnify the norm.");
  if(scalar_0 == scalar_1) return true;
  return std::abs(scalar_0 - scalar_1) <
         std::max(tolerance_absolute, tolerance_relative*std::min(std::abs(scalar_0) + std::abs(scalar_1),
                                                                  scalar_max));
}

/**
 * @brief Checks if a first scalar is greater than or nearly equal to a second, could be though of as operator>=~.
 * @param[in] scalar_0 The first scalar value, s_0, to compare.
 * @param[in] scalar_1 The second scalar value, s_1, to compare.
 * @param[in] tolerance_relative Tolerance value, tol_rel, below which two the normalised scalars are considered equal.
 * @param[in] tolerance_absolute Cutoff tolerance value, tol_abs, below which two the scalars are considered equal.
 * @return True if the s_0 > s_1 or s_0 ~= s_1, i.e. s_0 >=~ s_1.
 */
[[nodiscard]] constexpr bool is_nearly_greater_equal(const Scalar& scalar_0, const Scalar& scalar_1,
                                                     const Scalar& tolerance_relative = default_relative,
                                                     const Scalar& tolerance_absolute = default_absolute) {
  return scalar_0 > scalar_1 || is_nearly_equal(scalar_0, scalar_1, tolerance_relative, tolerance_absolute);
}

/**
 * @brief Checks if a first scalar is less than or nearly equal to a second, could be though of as operator<=~.
 * @param[in] scalar_0 The first scalar value, s_0, to compare.
 * @param[in] scalar_1 The second scalar value, s_1, to compare.
 * @param[in] tolerance_relative Tolerance value, tol_rel, below which two the normalised scalars are considered equal.
 * @param[in] tolerance_absolute Cutoff tolerance value, tol_abs, below which two the scalars are considered equal.
 * @return True if the s_0 < s_1 or s_0 ~= s_1, i.e. s_0 <=~ s_1.
 */
[[nodiscard]] constexpr bool is_nearly_less_equal(const Scalar& scalar_0, const Scalar& scalar_1,
                                                  const Scalar& tolerance_relative = default_relative,
                                                  const Scalar& tolerance_absolute = default_absolute) {
  return scalar_0 < scalar_1 || is_nearly_equal(scalar_0, scalar_1, tolerance_relative, tolerance_absolute);
}

/**
 * @brief Checks if a first scalar is not less than or nearly equal to a second, could be though of as operator>~.
 * @param[in] scalar_0 The first scalar value, s_0, to compare.
 * @param[in] scalar_1 The second scalar value, s_1, to compare.
 * @param[in] tolerance_relative Tolerance value, tol_rel, below which two the normalised scalars are considered equal.
 * @param[in] tolerance_absolute Cutoff tolerance value, tol_abs, below which two the scalars are considered equal.
 * @return True if the s_0 > s_1 and s_0 !~= s_1, i.e. s_0 !<=~ s_1.
 *
 * @warning
 * This function does not give a strictly greater than operation, it is designed to give consistent/continuous results
 * between with is_nearly_less_equal. i.e. If is_nearly_less_equal(s_0, s_1) returns true this function will return
 * false, and via versa if is_nearly_less_equal(s_0, s_1) returns false. This occurs even if the first scalar, s_0, is
 * actually greater than the second, s_1. For this reason there is also no test for it, as is_nearly_less_equal is
 * tested.
 */
[[nodiscard]] constexpr bool is_nearly_greater(const Scalar& scalar_0, const Scalar& scalar_1,
                                              const Scalar& tolerance_relative = default_relative,
                                              const Scalar& tolerance_absolute = default_absolute) {
  return !is_nearly_less_equal(scalar_0, scalar_1, tolerance_relative, tolerance_absolute);
}

/**
 * @brief Checks if a first scalar is not greater than or nearly equal to a second, could be though of as operator<~.
 * @param[in] scalar_0 The first scalar value, s_0, to compare.
 * @param[in] scalar_1 The second scalar value, s_1, to compare.
 * @param[in] tolerance_relative Tolerance value, tol_rel, below which two the normalised scalars are considered equal.
 * @param[in] tolerance_absolute Cutoff tolerance value, tol_abs, below which two the scalars are considered equal.
 * @return True if the s_0 < s_1 and s_0 !~= s_1, i.e. s_0 !>=~ s_1.
 *
 * @warning
 * This function does not give a strictly less than operation, it is designed to give consistent/continuous results
 * between with is_nearly_greater_equal. i.e. If is_nearly_greater_equal(s_0, s_1) returns true this function will
 * return false, and via versa if is_nearly_greater_equal(s_0, s_1) returns false. This occurs even if the first scalar,
 * s_0, is actually less than the second, s_1. For this reason there is also no test for it, as is_nearly_greater_equal
 * is tested.
 */
[[nodiscard]] constexpr bool is_nearly_less(const Scalar& scalar_0, const Scalar& scalar_1,
                                        const Scalar& tolerance_relative = default_relative,
                                        const Scalar& tolerance_absolute = default_absolute) {
  return !is_nearly_greater_equal(scalar_0, scalar_1, tolerance_relative, tolerance_absolute);
}

}

#endif //DISA_SCALAR_H
