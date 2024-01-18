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
// File Name: vector_operator.h
// Description: Contains the declaration and definition of mathematical vector operations. File will only define
//              operations which either reduce or maintain vector dimensionality. I.e. An outer product, for example,
//              would be defined in matrix_operators.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_VECTOR_OPERATORS_H
#define DISA_VECTOR_OPERATORS_H

#include "macros.h"
#include "scalar.h"
#include "vector_dense.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>
#include <random>

namespace Disa {

/**
 * @brief Computes the $L_p$-norm of a parsed vector, \f$L_p = (\sum_i |a_i|^p)^1/p\f$.
 * @tparam _p_value the p value for the norm, if 0 the l_infinity norm is computed.
 * @tparam _size Size of the vector if static, else 0.
 * @param vector The vector for which the norm is being computed.
 * @return The computed L_p-norm.
 */
template<unsigned int _p_value, typename _data, std::size_t _size>
constexpr Scalar lp_norm(const Vector_Dense<_data, _size>& vector) {
  switch(_p_value) {
    case 0:
      return std::abs(*std::max_element(vector.begin(), vector.end(),
                                        [](Scalar a, Scalar b) { return std::abs(a) < std::abs(b); }));
    case 1:
      return std::accumulate(vector.begin(), vector.end(), 0.0, [](Scalar a, Scalar b) { return a + std::abs(b); });
    case 2:
      return std::sqrt(std::accumulate(vector.begin(), vector.end(), 0.0, [](Scalar a, Scalar b) { return a + b*b; }));
    case 3:
      return std::cbrt(std::accumulate(vector.begin(), vector.end(), 0.0, [](Scalar a, Scalar b) {
        const Scalar abs_b = std::abs(b);
        return a + abs_b*abs_b*abs_b;
      }));
    default:
      return std::pow(std::accumulate(vector.begin(), vector.end(), 0.0, [](Scalar a, Scalar b) {
        return a + std::pow(_p_value%2 == 0 ? b : std::abs(b), _p_value);
      }), 1.0/_p_value);
  }
}

/**
 * @brief Computes the arithmetic mean of the vector's elements.
 * @tparam _size Size of the vector if static, else 0.
 * @param[in] vector The vector to compute the mean value.
 * @return The arithmetic mean value of the vector.
 */
template<typename _data, std::size_t _size>
constexpr Scalar mean(const Vector_Dense<_data, _size>& vector) {
  ASSERT_DEBUG(_size != 0 || !vector.empty(), "Dynamic vector is empty.");
  return std::accumulate(vector.begin(), vector.end(), 0.0)/static_cast<Scalar>(vector.size());
}

/**
 * @brief Computes the dot(inner) product between two vectors.
 * @tparam _size_0 Size of the first vector if static, else 0.
 * @tparam _size_1 Size of the second vector if static, else 0.
 * @param vector_0 The first vector to be dotted.
 * @param vector_1 The second vector to be dotted.
 * @return The scalar result.
 */
template<typename _data, std::size_t _size_0, std::size_t _size_1>
constexpr Scalar dot_product(const Vector_Dense<_data, _size_0>& vector_0,
                             const Vector_Dense<_data, _size_1>& vector_1) {
  ASSERT_DEBUG(vector_0.size() == vector_1.size(),
               "Incompatible vector sizes, " + std::to_string(vector_0.size()) + " vs. " +
               std::to_string(vector_1.size()) + ".");
  return std::inner_product(vector_0.begin(), vector_0.end(), vector_1.begin(), 0.0);
}

/**
 * @brief Computes a new vector with the same direction, but unit length. Zero vectors are returned as zero vectors.
 * @tparam _size Size of the vector if static, else 0.
 * @param[in] vector The vector to be normalised.
 * @return A unit vector if the vector has size, else the zero vector.
 */
template<typename _data, std::size_t _size>
constexpr Vector_Dense<_data, _size> unit(Vector_Dense<_data, _size> vector) {
  const Scalar inverse_l_2 = 1.0/lp_norm<2>(vector);
  vector *= std::isinf(inverse_l_2) ? 0.0 : inverse_l_2; // properly zero, zero vectors.
  return vector;
}

/**
 * @brief Computes the (smaller/included) angle between two vectors, computed theta = arccos (a.b/|a||b|).
 * @tparam _is_radians Must the returned angle be computed in radians or degrees.
 * @tparam _size_0 Size of the first vector if static, else 0.
 * @tparam _size_1 Size of the second vector if static, else 0.
 * @param vector_0 The first vector, a.
 * @param vector_1 The second vector, b.
 * @return The angle between the vectors.
 */
template<bool _is_radians, typename _data, std::size_t _size_0, std::size_t _size_1>
constexpr _data angle(const Vector_Dense<_data, _size_0>& vector_0, const Vector_Dense<_data, _size_1>& vector_1) {
  ASSERT_DEBUG(vector_0.size() == vector_1.size(),
               "Incompatible vector sizes, " + std::to_string(vector_0.size()) + " vs. " +
               std::to_string(vector_1.size()) + ".");
  ASSERT_DEBUG(vector_0.size() == 2 || vector_0.size() == 3,
               "Incompatible vector size, " + std::to_string(vector_0.size()) + ", must be 2 or 3.");
  return std::acos(std::clamp(dot_product(unit(vector_0), unit(vector_1)), -1.0, 1.0))/
         (_is_radians ? 1.0 : std::numbers::pi/180.0);
}

/**
 * @brief Computes the cross product between two vectors, c = a x b.
 * @tparam _size_0 Size of the first vector if static, else 0.
 * @tparam _size_1 Size of the second vector if static, else 0.
 * @param[in] vector_0 The first vector, a, of the cross product.
 * @param[in] vector_1 The second vector, a, of the cross product.
 * @return The vector orthogonal (to a and b) vector c.
 */
template<typename _data, std::size_t _size_0, std::size_t _size_1>
constexpr typename StaticPromoter<Vector_Dense<_data, _size_0>, Vector_Dense<_data, _size_1>>::type
cross_product(const Vector_Dense<_data, _size_0>& vector_0, const Vector_Dense<_data, _size_1>& vector_1) {
  ASSERT_DEBUG(vector_0.size() == vector_1.size(),
               "Incompatible vector sizes, " + std::to_string(vector_0.size()) + " vs. " +
               std::to_string(vector_1.size()) + ".");
  ASSERT_DEBUG(vector_0.size() == 2 || vector_0.size() == 3,
               "Incompatible vector size, " + std::to_string(vector_0.size()) + ", must be 2 or 3.");
  typedef typename StaticPromoter<Vector_Dense<_data, _size_0>, Vector_Dense<_data, _size_1>>::type _return_vector;
  if (vector_0.size() == 2) return {0.0, 0.0, vector_0[0]*vector_1[1] - vector_0[1]*vector_1[0]};
  else
    return {vector_0[1]*vector_1[2] - vector_0[2]*vector_1[1],
            vector_0[2]*vector_1[0] - vector_0[0]*vector_1[2],
            vector_0[0]*vector_1[1] - vector_0[1]*vector_1[0]};

}

/**
 * @brief Projects a vector, a, onto a second vector, b, i.e. the component of a in the directory of b.
 * @tparam _size_0 Size of the first vector if static, else 0.
 * @tparam _size_1 Size of the second vector if static, else 0.
 * @param[in] vector_0 The vector to project.
 * @param[in] vector_1 The direction of the projection, must be a unit vector.
 * @return The projected vector.
 */
template<typename _data, std::size_t _size_0, std::size_t _size_1>
constexpr typename StaticPromoter<Vector_Dense<_data, _size_0>, Vector_Dense<_data, _size_1>>::type
projection_tangent(const Vector_Dense<_data, _size_0>& vector_0, const Vector_Dense<_data, _size_1>& vector_1) {
  ASSERT_DEBUG(lp_norm<2>(vector_1), "Second vector is not a unit vector.");
  ASSERT_DEBUG(vector_0.size() == vector_1.size(),
               "Incompatible vector sizes, " + std::to_string(vector_0.size()) + " vs. " +
               std::to_string(vector_1.size()) + ".");
  typedef typename StaticPromoter<Vector_Dense<_data, _size_0>, Vector_Dense<_data, _size_1>>::type _return_vector;
  return dot_product(vector_0, vector_1)*
         _return_vector([&](const std::size_t i_element) { return vector_1[i_element]; }, vector_1.size());
}

/**
 * @brief Projects a vector, a, such that the projection is orthogonal to a second vector, b.
 * @tparam _size_0 Size of the first vector if static, else 0.
 * @tparam _size_1 Size of the second vector if static, else 0.
 * @param[in] vector_0 The vector to project.
 * @param[in] vector_1 The 'orthogonal' direction of the projection, must be a unit vector.
 * @return The projected vector.
 */
template<typename _data, std::size_t _size_0, std::size_t _size_1>
constexpr typename StaticPromoter<Vector_Dense<_data, _size_0>, Vector_Dense<_data, _size_1>>::type
projection_orthogonal(const Vector_Dense<_data, _size_0>& vector_0, const Vector_Dense<_data, _size_1>& vector_1) {
  ASSERT_DEBUG(vector_0.size() == vector_1.size(),
               "Incompatible vector sizes, " + std::to_string(vector_0.size()) + " vs. " +
               std::to_string(vector_1.size()) + ".");
  typedef typename StaticPromoter<Vector_Dense<_data, _size_0>, Vector_Dense<_data, _size_1>>::type _return_vector;
  _return_vector vector([&](const std::size_t index) { return vector_0[index]; }, vector_1.size());
  return vector - projection_tangent(vector_0, vector_1);
}

}//Disa

#endif //DISA_VECTOR_OPERATORS_H
