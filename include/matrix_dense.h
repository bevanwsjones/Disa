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
    ASSERT_DEBUG(row == _rows && column == _cols, "Cannot change the number of rows and columns for a static matrix.");
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

  //--------------------------------------------------------------------------------------------------------------------
  // Assignment Operators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Multiplies the matrix by a scalar, A' = A*b, where A is the matrix and b is the scalar
   * @param[in] scalar Scalar value, b, to multiply the matrix by.
   * @return Updated matrix (A').
   */
  constexpr _matrix& operator*=(const double& scalar) {
    FOR_EACH_REF(element, *this) element *= scalar;
    return *this;
  }

  /**
   * @brief Divides the matrix by a scalar, A' = A/b, where A is the matrix and b is a scalar.
   * @param[in] scalar Scalar value, b, to multiply the vector by.
   * @return Updated matrix (A').
   *
   * Note: Division by zero is left to the user to handle.
   */
  constexpr _matrix& operator/=(const double& scalar) {
    FOR_EACH_REF(element, *this) element /= scalar;
    return *this;
  }

  /**
   * @brief Addition of a second matrix, A' = A + B, where A and B are matrices.
   * @tparam _rows_other The number of rows of the B matrix, dynamic/static.
   * @tparam _cols_other The number of column of the B matrix, dynamic/static.
   * @param[in] matrix The second matrix, B, to add.
   * @return Updated matrix (A').
   */
  template<std::size_t _rows_other, std::size_t _cols_other>
  constexpr _matrix& operator+=(const Matrix_Dense<_rows_other, _cols_other>& matrix) {
    ASSERT_DEBUG(size() == matrix.size(),
                 "Incompatible matrix dimensions, " + std::to_string(size_row()) + "," + std::to_string(size_column()) +
                 " vs. " + std::to_string(matrix.size_row()) + "," + std::to_string(matrix.size_column()) + ".");
    FOR(row, _rows) (*this)[row] += matrix[row];
    return *this;
  }

  /**
   * @brief Subtraction by a second matrix, A' = A - B, where A and B are matrices.
   * @tparam _rows_other The number of rows of the B matrix, dynamic/static.
   * @tparam _cols_other The number of column of the B matrix, dynamic/static.
   * @param[in] matrix The second matrix, B, to subtract.
   * @return Updated matrix (A').
   */
  template<std::size_t _rows_other, std::size_t _cols_other>
  constexpr _matrix& operator-=(const Matrix_Dense<_rows_other, _cols_other>& matrix) {
    ASSERT_DEBUG(size() == matrix.size(),
                 "Incompatible matrix dimensions, " + std::to_string(size_row()) + "," + std::to_string(size_column()) +
                 " vs. " + std::to_string(matrix.size_row()) + "," + std::to_string(matrix.size_column()) + ".");
    FOR(row, _rows) (*this)[row] -= matrix[row];
    return *this;
  }

  /**
   * @brief Multiplies the matrix by another matrix, A' = A*B, where A and B are matrices.
   * @tparam _rows_other The number of rows of the B matrix, dynamic/static.
   * @tparam _cols_other The number of column of the B matrix, dynamic/static.
   * @param[in] matrix The second matrix, B, to add.
   * @return Updated matrix (A').
   *
   * Note: Due to the static nature of the size of rows and columns it is only possible to multiply by square matrices.
   */
  template<std::size_t _rows_other, std::size_t _cols_other>
  constexpr _matrix& operator*=(const Matrix_Dense<_rows_other, _cols_other>& matrix) {
    // For static containers the matrices must be square.
    ASSERT_DEBUG(size() == matrix.size(),
                 "Incompatible matrix dimensions, " + std::to_string(size_row()) + "," + std::to_string(size_column()) +
                 " vs. " + std::to_string(matrix.size_row()) + "," + std::to_string(matrix.size_column()) + ".");
    _matrix copy;
    std::swap(*this, copy);
    FOR(i_row, size_row()) {
      FOR(i_row_other, matrix.size_row()) {
        FOR(i_column, size_column()) {
          if (i_row_other == 0) (*this)[i_row][i_column] = 0.0;
          (*this)[i_row][i_column] += copy[i_row][i_row_other]*matrix[i_row_other][i_column];
        }
      }
    }
    return *this;
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
      ASSERT_DEBUG(item.size() == list.begin()->size(), "List dimension varies.");
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

  //--------------------------------------------------------------------------------------------------------------------
  // Assignment Operators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Multiplies the matrix by a scalar, A' = A*b, where A is the matrix and b is the scalar
   * @param[in] scalar Scalar value, b, to multiply the matrix by.
   * @return Updated matrix (A').
   */
  _matrix& operator*=(const double& scalar) {
    FOR_EACH_REF(element, *this) element *= scalar;
    return *this;
  }

  /**
   * @brief Divides the matrix by a scalar, A' = A/b, where A is the matrix and b is a scalar.
   * @param[in] scalar Scalar value, b, to multiply the vector by.
   * @return Updated vector (A').
   *
   * Note: Division by zero is left to the user to handle.
   */
  _matrix& operator/=(const double& scalar) {
    FOR_EACH_REF(element, *this) element /= scalar;
    return *this;
  }

  /**
   * @brief Addition of a second matrix, A' = A + B, where A and B are matrices.
   * @tparam _rows_other The number of rows of the B matrix, dynamic/static.
   * @tparam _cols_other The number of column of the B matrix, dynamic/static.
   * @param[in] matrix The second matrix, B, to add.
   * @return Updated matrix (A').
   */
  template<std::size_t _rows_other, std::size_t _cols_other>
  constexpr _matrix& operator+=(const Matrix_Dense<_rows_other, _cols_other>& matrix) {
    ASSERT_DEBUG(size() == matrix.size(),
                 "Incompatible matrix dimensions, " + std::to_string(size_row()) + "," + std::to_string(size_column()) +
                 " vs. " + std::to_string(matrix.size_row()) + "," + std::to_string(matrix.size_column()) + ".");
    FOR(row, size_row()) (*this)[row] += matrix[row];
    return *this;
  }

  /**
   * @brief Subtraction by a second matrix, A' = A - B, where A and B are matrices.
   * @tparam _rows_other The number of rows of the B matrix, dynamic/static.
   * @tparam _cols_other The number of column of the B matrix, dynamic/static.
   * @param[in] matrix The second matrix, B, to subtract.
   * @return Updated matrix (A').
   */
  template<std::size_t _rows_other, std::size_t _cols_other>
  constexpr _matrix& operator-=(const Matrix_Dense<_rows_other, _cols_other>& matrix) {
    ASSERT_DEBUG(size() == matrix.size(),
                 "Incompatible matrix dimensions, " + std::to_string(size_row()) + "," + std::to_string(size_column()) +
                 " vs. " + std::to_string(matrix.size_row()) + "," + std::to_string(matrix.size_column()) + ".");
    FOR(row, size_row()) (*this)[row] -= matrix[row];
    return *this;
  }

  /**
   * @brief Multiplies the matrix by another matrix, A' = A*B, where A and B are matrices.
   * @tparam _rows_other The number of rows of the B matrix, dynamic/static.
   * @tparam _cols_other The number of column of the B matrix, dynamic/static.
   * @param[in] matrix The second matrix, B, to add.
   * @return Updated matrix (A').
   *
   * Note: Number of rows and columns will change if either matrix is not square.
   */
  template<std::size_t _rows_other, std::size_t _cols_other>
  _matrix& operator*=(const Matrix_Dense<_rows_other, _cols_other>& matrix) {
    // For static containers the matrices must be square.
    ASSERT_DEBUG(size_column() == matrix.size_row(),
                 "Incompatible matrix dimensions, " + std::to_string(size_row()) + "," + std::to_string(size_column()) +
                 " vs. " + std::to_string(matrix.size_row()) + "," + std::to_string(matrix.size_column()) + ".");
    _matrix copy;
    copy.resize(size_row(), matrix.size_column());
    std::swap(*this, copy);
    FOR(i_row, size_row()) {
      FOR(i_row_other, matrix.size_row()) {
        FOR(i_column, size_column()) {
          if (i_row_other == 0) (*this)[i_row][i_column] = 0.0;
          (*this)[i_row][i_column] += copy[i_row][i_row_other]*matrix[i_row_other][i_column];
        }
      }
    }
    return *this;
  }
};

template<std::size_t _rows, std::size_t _cols>
constexpr Matrix_Dense<_rows, _cols> operator*(const double& scalar, Matrix_Dense<_rows, _cols> matrix) {
  return matrix *= scalar;
}

template<std::size_t _rows, std::size_t _cols>
constexpr Vector_Dense<_rows>& operator*(const Matrix_Dense<_rows, _cols>& matrix, const Vector_Dense<_rows>& vector) {
  return Vector_Dense<_rows>([&](const std::size_t i_row) { return dot_product(matrix[i_row], vector[i_row]); });
}

template<std::size_t _rows, std::size_t _cols>
constexpr Matrix_Dense<_rows, _cols>
operator*(const Matrix_Dense<_rows, _cols>& matrix_0, const Matrix_Dense<_rows, _cols>& matrix_1) {
  Matrix_Dense<_rows, _cols> matrix([&](const std::size_t i_row) {
    Vector_Dense<_rows> row(0.0);
    FOR(i_column, matrix_0.column()) {
      FOR(i_row_1, matrix_1.row()) {
        row[i_column] += matrix_0[i_row][i_column]*matrix_1[i_row_1][i_column];
      }
      return row;
    }
  });
}


}

#endif //DISA_MATRIX_DENSE_H
