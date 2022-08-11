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
// File Name: matrix_dense.h
// Description: Contains the declaration and definitions for the basic static and dynamic dense matrix classes for Disa.
//----------------------------------------------------------------------------------------------------------------------

#ifndef DISA_MATRIX_DENSE_H
#define DISA_MATRIX_DENSE_H

#include "vector_dense.h"
#include "vector_operators.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <numeric>
#include <stdexcept>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// Statically Sized Dense Vector Class
//----------------------------------------------------------------------------------------------------------------------

namespace Disa {

template<std::size_t _rows, std::size_t _cols>
struct Matrix_Dense : public std::array<Vector_Dense<_cols>, _rows> {
  typedef Matrix_Dense<_rows, _cols> _matrix;    /** @typedef Short hand for this vector type. */
  const static bool is_dynamic = false;          /** @var Indicates the vector is compile time sized. */

  //--------------------------------------------------------------------------------------------------------------------
  // Constructors/Destructors
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Initialise empty matrix.
   */
  Matrix_Dense() : std::array<Vector_Dense<_cols>, _rows>() {};

  /**
   * @brief Constructor to construct from initializer of vectors list, list and matrix must be of the same dimensions.
   * @param[in] list The list of vectors to initialise the matrix.
   */
  Matrix_Dense(const std::initializer_list<Vector_Dense<_cols> >& list) {
    auto iter = this->begin();
    FOR_EACH(item, list) *iter++ = item;
  }

  /**
   * @brief Constructor to construct a matrix from a lambda expression.
   * @param[in] lambda Lambda expression.
   * @param[in] row Desired number of rows of the matrix. Defaulted, allows interoperability with dynamic matrices
   * @param[in] column Desired number of columns of the matrix. Defaulted, allows interoperability with dynamic matrices
   */
  explicit Matrix_Dense(const std::function<double(std::size_t, std::size_t)>& lambda,
                        std::size_t row = _rows, std::size_t column = _cols) {
    DEBUG_ASSERT(row == _rows && column == _cols,
                 std::invalid_argument("Cannot change the number of rows and columns for a static matrix."));
    FOR(i_row, row) { FOR(i_column, column) (*this)[i_row][i_column] = lambda(i_row, i_column); }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Size Functions
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Returns the number of rows in the matrix.
   * @return The number of rows.
   */
  constexpr std::size_t size_row() const noexcept { return _rows; }

  /**
   * @brief Returns the number of columns in the matrix.
   * @return The number of columns.
   */
  constexpr std::size_t size_column() const noexcept { return _cols; }

  /**
   * @brief Returns the number of rows and columns in the matrix.
   * @return Pair containing [rows, columns].
   */
  constexpr std::pair<std::size_t, std::size_t> size() const noexcept {
    return std::make_pair(size_row(), size_column());
  }

};

template<>
struct Matrix_Dense<0, 0> : public std::vector<Vector_Dense<0> > {
  typedef Matrix_Dense<0, 0> _matrix;         /** @typedef Short hand for this vector type. */
  const static bool is_dynamic = true;        /** @var Indicates the vector is compile time sized. */

  //--------------------------------------------------------------------------------------------------------------------
  // Constructors/Destructors
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Initialise empty matrix.
   */
  Matrix_Dense() : std::vector<Vector_Dense<0> >() {};

  /**
   * @brief Constructor to construct a matrix from an initializer list, matrix is resized to list size.
   * @param[in] list The list of doubles to initialise the vector. Each row much be the same size.
   */
  Matrix_Dense(const std::initializer_list<Vector_Dense<0> >& list) {
    resize(list.size());
    auto iter = this->begin();
    FOR_EACH(item, list) {
      DEBUG_ASSERT(item.size() == list.begin()->size(), std::invalid_argument("List dimension varies."));
      *iter++ = item;
    }
  }

  /**
   * @brief Constructor to construct a matrix from a lambda expression.
   * @param[in] lambda Lambda expression.
   * @param[in] row Desired number of rows of the matrix.
   * @param[in] column Desired  number of columns of the matrix.
   */
  explicit Matrix_Dense(const std::function<double(std::size_t, std::size_t)>& lambda, std::size_t row,
                        std::size_t column) {
    resize(row);
    FOR(i_row, row) {
      (*this)[i_row].resize(column);
      FOR(i_column, column) (*this)[i_row][i_column] = lambda(i_row, i_column);
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Size Functions
  //--------------------------------------------------------------------------------------------------------------------

  using std::vector<Vector_Dense<0> >::resize;

  /**
   * @brief Resizes the rows and columns of the matrix.
   * @param[in] rows The number of rows to resize the matrix to.
   * @param[in] columns The number of columns to resize the matrix to.
   */
  void resize(const std::size_t rows, const std::size_t columns) {
    resize(rows);
    FOR_EACH_REF(row, (*this)) row.resize(columns);
  };

  /**
   * @brief Returns the number of rows in the matrix.
   * @return The number of rows.
   */
  std::size_t size_row() const noexcept { return std::distance(cbegin(), cend()); }

  /**
   * @brief Returns the number of columns in the matrix.
   * @return The number of columns.
   */
  std::size_t size_column() const noexcept { return empty() ? 0 : (*this)[0].size(); }

  /**
   * @brief Returns the number of rows and columns in the matrix. If rows are 0, columns are 0.
   * @return Pair containing [rows, columns].
   */
  std::pair<std::size_t, std::size_t> size() const noexcept { return std::make_pair(size_row(), size_column()); }

};

}

#endif //DISA_MATRIX_DENSE_H
