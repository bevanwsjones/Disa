//----------------------------------------------------------------------------------------------------------------------
// MIT License
// Copyright (c) 2022 bevanwsjones
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
// File Name: Vector.h
// Description: Contains the declaration and definitions for the basic static and dynamic vector class for Disa.
//----------------------------------------------------------------------------------------------------------------------

//TODO: fill in above
#ifndef DISA_VECTOR_H
#define DISA_VECTOR_H

#include "Macros.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <numeric>
#include <vector>

namespace Disa {

template<std::size_t _size>
struct Vector : public std::array<double, _size> {
  typedef Vector<_size> _vector;
  const static bool dynamic = false;  /**< Indicates the vector is compile time sized. */

  /**
   * @brief Constructor to construct from initializer list, list and vector must be of the same size.
   * @param[in] list The list of doubles to initialised the vector.
   */
  Vector(const std::initializer_list<double> &list) {
    assert_check_debug(list.size() == _size, "Initializer list of incorrect size, " + std::to_string(list.size())
                                             + " vs. " + std::to_string(_size) + ".", std::source_location::current());
    auto iter = this->begin();
    FOR_EACH(item, list) *iter++ = item;
  }

  /**
   * @brief Constructor to construct a vector from a lambda.
   * @tparam _lambda Template lambda function double(std::size_t).
   * @param[in] lambda Lambda expression.
   */
  template<class _lambda>
  explicit Vector(_lambda lambda) {
    FOR(i_element, this->size()) (*this)[i_element] = lambda(i_element);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Arithmetic Operators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Multiplies the vector by a scalar, a' = a*b, where a is the vectors and b is a scalar.
   * @param scalar Scalar value, b, to multiply the vector by.
   * @return Updated vector (a).
   */
  constexpr _vector &operator*(const double &scalar) {
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
  constexpr _vector &operator/(const double &scalar) {
    FOR_EACH_REF(element, *this) element /= scalar;
    return *this;
  }

};//Vector

template<>
struct Vector<0> : public std::vector<double> {
  typedef Vector<0> _vector;
  const static bool dynamic = true;    /**< Indicates the vector is runtime resizable. */

  /**
   * @brief Constructor to construct from initializer list, vector is resized to list size.
   * @param[in] list The list of doubles to initialised the vector.
   */
  Vector(const std::initializer_list<double> &list) {
    resize(list.size());
    auto iter = begin();
    FOR_EACH(item, list) *iter++ = item;
  }

  /**
   * @brief Constructor to construct a vector from a lambda.
   * @tparam _lambda Template lambda function double(std::size_t).
   * @param[in] size Desired size of the vector.
   * @param[in] lambda Lambda expression.
   */
  template<class _lambda>
  explicit Vector(std::size_t size, _lambda lambda) : std::vector<double>(size) {
    FOR(i_element, this->size()) (*this)[i_element] = lambda(i_element);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Arithmetic Operators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Multiplies the vector by a scalar, a' = a*b, where a is the vectors and b is a scalar.
   * @param scalar Scalar value, b, to multiply the vector by.
   * @return Updated vector (a).
   */
  _vector &operator*(const double &scalar) {
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
  _vector &operator/(const double &scalar) {
    FOR_EACH_REF(element, *this) element /= scalar;
    return *this;
  }
};//Vector


template<class _vector0, class _vector1>
constexpr typename std::conditional<!_vector0::dynamic, _vector0, _vector1>::type
operator+(const _vector0 &vector0, const _vector1 &vector1) {
  typedef typename std::conditional<!_vector0::dynamic, _vector0, _vector1>::type _return;
  return _return([&](const std::size_t ii) { return vector0[ii] + vector1[ii]; });
}

Vector<0> operator+(const Vector<0> &vector0, const Vector<0> &vector1) {
  return Vector<0>(vector0.size(), [&](const std::size_t ii) { return vector0[ii] + vector1[ii]; });
}

}//Disa

#endif //DISA_VECTOR_H
