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
// File Name: vector_dense.h
// Description: Contains the declaration and definitions for the basic static and dynamic dense vector classes for Disa.
//----------------------------------------------------------------------------------------------------------------------

#ifndef DISA_VECTOR_DENSE_H
#define DISA_VECTOR_DENSE_H

#include "macros.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <numeric>
#include <stdexcept>
#include <stdexcept>
#include <vector>

namespace Disa {

//----------------------------------------------------------------------------------------------------------------------
// Statically Sized Dense Vector Class
//----------------------------------------------------------------------------------------------------------------------

/**
 * @struct Vector_Dense
 * @brief Mathematical Vector, of dimension _size, where every vector element has allocated has memory.
 * @tparam _size The size of the vector, if 0 a specialisation of the class is created.
 *
 * The Vector_Dense struct implements a mathematical vector of real numbers. Disa does not make distinctions row and
 * column vectors, as such this information is not stored.
 *
 * To avoid massive boiler plate the vector inherits from std::array, which implies the vector's dimension is fixed at
 * compile time. To obtain a dynamically allocated dense vector the _size value can be set to 0. In which case
 * std::vector is inherited, see below specialisation.
 */
template<std::size_t _size>
struct Vector_Dense : public std::array<double, _size> {
  typedef Vector_Dense<_size> _vector;     /** @typedef Short hand for this vector type. */
  const static bool is_dynamic = false;    /** @var Indicates the vector is compile time sized. */

  //--------------------------------------------------------------------------------------------------------------------
  // Constructors/Destructors
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Initialise empty vector.
   */
  Vector_Dense() : std::array<double, _size>() {};

  /**
   * @brief Constructor to construct from initializer list, list and vector must be of the same size.
   * @param[in] list The list of doubles to initialised the vector.
   */
  Vector_Dense(const std::initializer_list<double>& list) {
    ASSERT_DEBUG(list.size() == _size, "Initializer list of incorrect size, " + std::to_string(list.size()) + " vs. " +
      std::to_string(_size) + ".");
    auto iter = this->begin();
    FOR_EACH(item, list) *iter++ = item;
  }

  /**
   * @brief Constructor to construct a vector from a lambda.
   * @tparam _lambda Template lambda function double(std::size_t).
   * @param[in] lambda Lambda expression.
   * @param[in] size Desired size of the vector. Added for interoperability with dynamic vectors.
   */
  explicit Vector_Dense(const std::function<double(const std::size_t)>& lambda, std::size_t size = _size) {
    ASSERT_DEBUG(size == _size, "Cannot change the size for a static vector.");
    FOR(i_element, this->size()) (*this)[i_element] = lambda(i_element);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Assignment Operators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Multiplies the vector by a scalar, a' = a*b, where a is the vectors and b is a scalar.
   * @param scalar Scalar value, b, to multiply the vector by.
   * @return Updated vector (a').
   */
  constexpr _vector& operator*=(const double& scalar) {
    FOR_EACH_REF(element, *this) element *= scalar;
    return *this;
  }

  /**
   * @brief Divides the vector by a scalar, a' = a/b, where a is the vectors and b is a scalar.
   * @param scalar Scalar value, b, to multiply the vector by.
   * @return Updated vector (a').
   *
   * Note: Division by zero is left to the user to handle.
   */
  constexpr _vector& operator/=(const double& scalar) {
    FOR_EACH_REF(element, *this) element /= scalar;
    return *this;
  }

  /**
   * @brief Addition of a second vector, a' = a + b, where a and b are vectors.
   * @tparam _size_other Vector size of b, dynamic/static.
   * @param vector The second vector, b, to add.
   * @return Updated vector (a').
   */
  template<std::size_t _size_other>
  constexpr _vector& operator+=(const Vector_Dense<_size_other>& vector) {
    ASSERT_DEBUG(_size == vector.size(),
                 "Incompatible vector sizes, " + std::to_string(_size) + " vs. " + std::to_string(vector.size()) + ".");
    FOR(index, _size) (*this)[index] += vector[index];
    return *this;
  }

  /**
   * @brief Subtraction by a second vector, a' = a - b, where a and b are vectors.
   * @tparam _size_other Vector size of b, dynamic/static.
   * @param vector The second vector, b, to subtract.
   * @return Updated vector (a').
   */
  template<std::size_t _size_other>
  constexpr _vector& operator-=(const Vector_Dense<_size_other>& vector) {
    ASSERT_DEBUG(_size == vector.size(),
                 "Incompatible vector sizes, " + std::to_string(_size) + " vs. " + std::to_string(vector.size()) + ".");
    FOR(index, _size) (*this)[index] -= vector[index];
    return *this;
  }
};//Vector_Dense

//----------------------------------------------------------------------------------------------------------------------
// Dynamically Sized Dense Vector Class
//----------------------------------------------------------------------------------------------------------------------

/**
 * @struct Vector_Dense<0>
 * @brief Mathematical Vector, of dimension _size, where every vector element has allocated has memory.
 *
 * The Vector_Dense struct the implements a mathematical vector of real numbers. Disa does not make distinctions row and
 * column vectors, as such this information is not stored.
 *
 * To avoid massive boiler plate the vector inherits from std::array, which implies the vector's dimension is fixed at
 * compile time. To obtain a dynamically allocated dense vector the _size value can be set to 0. In which case the
 * std::vector is inherited, see below specialisation.
 */
template<>
struct Vector_Dense<0> : public std::vector<double> {
  typedef Vector_Dense<0> _vector;      /** @typedef Short hand for this vector type. */
  const static bool is_dynamic = true;  /** @var Indicates the vector is runtime resizable. */

  //--------------------------------------------------------------------------------------------------------------------
  // Constructors/Destructors
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Initialise empty vector.
   */
  Vector_Dense() : std::vector<double>() {};

  /**
   * @brief Constructor to construct from initializer list, vector is resized to list size.
   * @param[in] list The list of doubles to initialised the vector.
   */
  Vector_Dense(const std::initializer_list<double>& list) {
    resize(list.size());
    auto iter = begin();
    FOR_EACH(item, list) *iter++ = item;
  }

  /**
   * @brief Constructor to construct a vector from a lambda.
   * @tparam _lambda Template lambda function double(std::size_t).
   * @param[in] lambda Lambda expression.
   * @param[in] size Desired size of the vector.
   */
  explicit Vector_Dense(const std::function<double(std::size_t)>& lambda, std::size_t size) : std::vector<double>(
    size) {
    FOR(i_element, this->size()) (*this)[i_element] = lambda(i_element);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Assignment Operators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Multiplies the vector by a scalar, a' = a*b, where a is the vectors and b is a scalar.
   * @param scalar Scalar value, b, to multiply the vector by.
   * @return Updated vector (a).
   */
  _vector& operator*=(const double& scalar) {
    FOR_EACH_REF(element, *this) element *= scalar;
    return *this;
  }

  /**
   * @brief Divides the vector by a scalar, a' = a/b, where a is the vectors and b is a scalar.
   * @param scalar Scalar value, b, to multiply the vector by.
   * @return Updated vector (a).
   *
   * Note: Division by zero is left to the user to handle.
   */
  _vector& operator/=(const double& scalar) {
    FOR_EACH_REF(element, *this) element /= scalar;
    return *this;
  }

  /**
   * @brief Addition of a second vector, a' = a + b, where a and b are vectors.
      * @tparam _size_other Vector size of b, dynamic/static.
   * @param vector The second vector, b, to add.
   * @return Updated vector (a').
   */
  template<std::size_t _size_other>
  constexpr _vector& operator+=(const Vector_Dense<_size_other>& vector) {
    ASSERT_DEBUG(size() == vector.size(),
                 "Incompatible vector sizes, " + std::to_string(size()) + " vs. " + std::to_string(vector.size()) +
                 ".");
    FOR(index, size()) (*this)[index] += vector[index];
    return *this;
  }

  /**
   * @brief Subtraction by a second vector, a' = a - b, where a and b are vectors.
   * @tparam _size_other Vector size of b, dynamic/static.
   * @param vector The second vector, b, to subtract.
   * @return Updated vector (a').
   */
  template<std::size_t _size_other>
  constexpr _vector& operator-=(const Vector_Dense<_size_other>& vector) {
    ASSERT_DEBUG(size() == vector.size(),
                 "Incompatible vector sizes, " + std::to_string(size()) + " vs. " + std::to_string(vector.size()) +
                 ".");
    FOR(index, size()) (*this)[index] -= vector[index];
    return *this;
  }

};//Vector_Dense

//----------------------------------------------------------------------------------------------------------------------
// Template Meta Programming
//----------------------------------------------------------------------------------------------------------------------

/**
 * @brief Chooses, between two vectors the static vector type if possible.
 * @tparam _vector0 The first vector type.
 * @tparam _vector1 The second vector type.
 */
template<class _vector0, class _vector1>
struct StaticPromoter {
  typedef typename std::conditional<!_vector0::is_dynamic, _vector0,
                                    _vector1>::type type;    /** @var Static vector type if either _vector_0 or _vector_1 is static else dynamic. */
};

//----------------------------------------------------------------------------------------------------------------------
// Arithmetic Operators
//----------------------------------------------------------------------------------------------------------------------

/**
 * @brief Multiplies a vector by a scalar, c = b*a, where a, and c are vectors and b is a scalar.
 * @tparam _vector Vector type, dynamic/static.
 * @param scalar Scalar value, b, to multiply the vector by.
 * @param vector Vector, a, to be multiplied.
 * @return New vector (c).
 */
template<std::size_t _size>
constexpr Vector_Dense<_size> operator*(const double& scalar, Vector_Dense<_size> vector) {
  return vector *= scalar;
}

/**
 * @brief Divides a vector by a scalar, c = b*a, where a, and c are vectors and b is a scalar.
 * @tparam _vector Vector type, dynamic/static.
 * @param scalar Scalar value, b, to multiply the vector by.
 * @param vector Vector, a, to be multiplied.
 * @return New vector (c).
 */
template<std::size_t _size>
constexpr Vector_Dense<_size> operator/(Vector_Dense<_size> vector, const double& scalar) {
  return vector /= scalar;
}

/**
 * @brief Adds two vectors together, c = a + b, where a, b, and c are vectors.
 * @tparam _vector0 Vector type, dynamic/static, of a.
 * @tparam _vector1 Vector type, dynamic/static, of b.
 * @param vector0 The first vector of the addition, a.
 * @param vector1 The second vector of the addition, b.
 * @return Newly constructed vector c.
 */
template<std::size_t _size_0, std::size_t _size_1>
typename StaticPromoter<Vector_Dense<_size_0>, Vector_Dense<_size_1> >::type
constexpr operator+(const Vector_Dense<_size_0>& vector0, const Vector_Dense<_size_1>& vector1) {
  ASSERT_DEBUG(vector0.size() == vector1.size(),
               "Incompatible vector sizes, " + std::to_string(vector0.size()) + " vs. " +
               std::to_string(vector1.size()) + ".");
  typedef typename StaticPromoter<Vector_Dense<_size_0>, Vector_Dense<_size_1> >::type _return_vector;
  return _return_vector([&](const std::size_t ii) { return vector0[ii] + vector1[ii]; }, vector0.size());
}

/**
 * @brief Subtracts two vectors, c = a - b, where a, b, and c are vectors.
 * @tparam _vector0 Vector type, dynamic/static, of a.
 * @tparam _vector1 Vector type, dynamic/static, of b.
 * @param vector0 The vector being subtracted from, a.
 * @param vector1 The subtracting vector, b.
 * @return Newly constructed vector c.
 */
template<std::size_t _size_0, std::size_t _size_1>
typename StaticPromoter<Vector_Dense<_size_0>, Vector_Dense<_size_1> >::type
operator-(const Vector_Dense<_size_0>& vector0, const Vector_Dense<_size_1>& vector1) {
  ASSERT_DEBUG(vector0.size() == vector1.size(),
               "Incompatible vector sizes, " + std::to_string(vector0.size()) + " vs. " +
               std::to_string(vector1.size()) + ".");
  typedef typename StaticPromoter<Vector_Dense<_size_0>, Vector_Dense<_size_1> >::type _return_vector;
  return _return_vector([&](const std::size_t ii) { return vector0[ii] - vector1[ii]; }, vector0.size());
}

}//Disa

#endif //DISA_VECTOR_DENSE_H
