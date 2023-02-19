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
// File Name: matrix_dense.h
// Description: Contains the declaration and definitions for the basic static and dynamic dense matrix classes for Disa.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_MATRIX_DENSE_H
#define DISA_MATRIX_DENSE_H

#include "macros.h"
#include "scalar.h"
#include "vector_dense.h"
#include "vector_operators.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <numeric>
#include <stdexcept>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------
// Statically Sized Dense Matrix Class
// ---------------------------------------------------------------------------------------------------------------------

namespace Disa {

/**
 * @struct Matrix_Dense
 * @brief Mathematical matrix, of dimension _row x _col, where every matrix element has allocated has memory.
 * @tparam _row The number of rows in the matrix, if 0 a specialisation of the class is created.
 * @tparam _col The number of columns in the matrix, if 0 a specialisation of the class is created.
 *
 * @details
 * The Matrix_Dense struct implements a mathematical matrix of nxm real numbers.
 *
 * To avoid massive boiler plate the matrix inherits from std::array, which implies the matrix's dimension is fixed at
 * compile time. To obtain a dynamically allocated dense matrix the _row and _col value can be set to 0.  In which case
 * std::matrix is inherited, see below specialisation. Note that semi-static matrices are not supported, e.g. _row = 1,
 * _col = 0.
 */
template<std::size_t _row, std::size_t _col>
struct Matrix_Dense : public std::array<Vector_Dense<_col>, _row> {
  using _matrix = Matrix_Dense<_row, _col>;  //!< Short hand for this matrix type.
  const static bool is_dynamic = false;      //!< Indicates the matrix is compile time sized.
  static_assert(_row != 0 && _col != 0, "Semi-static matrices are not supported");

  // -------------------------------------------------------------------------------------------------------------------
  // Constructors/Destructors
  // -------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Initialise empty matrix.
   */
  Matrix_Dense() : std::array<Vector_Dense<_col>, _row>() {};

  /**
   * @brief Constructor to construct from initializer of vectors list, list and matrix must be of the same dimensions.
   * @param[in] list The list of vectors to initialise the matrix.
   */
  Matrix_Dense(const std::initializer_list<Vector_Dense<_col> >& list) {
    auto iter = this->begin();
    FOR_EACH(item, list) *iter++ = item;
  }

  /**
   * @brief Constructor to construct a matrix from a lambda expression.
   * @param[in] lambda Lambda expression.
   * @param[in] row Desired number of rows of the matrix. Defaulted, allows interoperability with dynamic matrices
   * @param[in] column Desired number of columns of the matrix. Defaulted, allows interoperability with dynamic matrices
   */
  explicit Matrix_Dense(const std::function<Scalar(std::size_t, std::size_t)>& lambda,
                        std::size_t row = _row, std::size_t column = _col) {
    ASSERT_DEBUG(row == _row && column == _col, "Cannot change the number of rows and columns for a static matrix.");
    FOR(i_row, row) { FOR(i_column, column) (*this)[i_row][i_column] = lambda(i_row, i_column); }
  }

  // -------------------------------------------------------------------------------------------------------------------
  // Size Functions
  // -------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Returns the number of rows in the matrix.
   * @return The number of rows.
   */
  [[nodiscard]] constexpr std::size_t size_row() const noexcept { return _row; }

  /**
   * @brief Returns the number of columns in the matrix.
   * @return The number of columns.
   */
  [[nodiscard]] constexpr std::size_t size_column() const noexcept { return _col; }

  /**
   * @brief Returns the number of rows and columns in the matrix.
   * @return Pair containing [rows, columns].
   */
  constexpr std::pair<std::size_t, std::size_t> size() const noexcept {
    return std::make_pair(size_row(), size_column());
  }

  // -------------------------------------------------------------------------------------------------------------------
  // Assignment Operators
  // -------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Multiplies the matrix by a scalar, A' = A*b, where A is the matrix and b is the scalar
   * @param[in] scalar Scalar value, b, to multiply the matrix by.
   * @return Updated matrix (A').
   */
  constexpr _matrix& operator*=(const Scalar& scalar) {
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
  constexpr _matrix& operator/=(const Scalar& scalar) {
    FOR_EACH_REF(element, *this) element /= scalar;
    return *this;
  }

  /**
   * @brief Addition of a second matrix, A' = A + B, where A and B are matrices.
   * @tparam _row_other The number of rows of the B matrix, dynamic/static.
   * @tparam _col_other The number of column of the B matrix, dynamic/static.
   * @param[in] matrix The second matrix, B, to add.
   * @return Updated matrix (A').
   */
  template<std::size_t _row_other, std::size_t _col_other>
  constexpr _matrix& operator+=(const Matrix_Dense<_row_other, _col_other>& matrix) {
    ASSERT_DEBUG(size() == matrix.size(),
                 "Incompatible matrix dimensions, " + std::to_string(size_row()) + "," + std::to_string(size_column()) +
                 " vs. " + std::to_string(matrix.size_row()) + "," + std::to_string(matrix.size_column()) + ".");
    FOR(row, _row) (*this)[row] += matrix[row];
    return *this;
  }

  /**
   * @brief Subtraction by a second matrix, A' = A - B, where A and B are matrices.
   * @tparam _row_other The number of rows of the B matrix, dynamic/static.
   * @tparam _col_other The number of column of the B matrix, dynamic/static.
   * @param[in] matrix The second matrix, B, to subtract.
   * @return Updated matrix (A').
   */
  template<std::size_t _row_other, std::size_t _col_other>
  constexpr _matrix& operator-=(const Matrix_Dense<_row_other, _col_other>& matrix) {
    ASSERT_DEBUG(size() == matrix.size(),
                 "Incompatible matrix dimensions, " + std::to_string(size_row()) + "," + std::to_string(size_column()) +
                 " vs. " + std::to_string(matrix.size_row()) + "," + std::to_string(matrix.size_column()) + ".");
    FOR(row, _row) (*this)[row] -= matrix[row];
    return *this;
  }

  /**
   * @brief Multiplies the matrix by another matrix, A' = A*B, where A and B are matrices.
   * @tparam _row_other The number of rows of the B matrix, dynamic/static.
   * @tparam _col_other The number of column of the B matrix, dynamic/static.
   * @param[in] matrix The second matrix, B, to multiply by.
   * @return Updated matrix (A').
   *
   * Note: Due to the static nature of the size of rows and columns it is only possible to multiply by square matrices.
   */
  template<std::size_t _row_other, std::size_t _col_other>
  constexpr _matrix& operator*=(const Matrix_Dense<_row_other, _col_other>& matrix) {
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

/**
 * @struct Matrix_Dense<0, 0>
 * @brief Mathematical matrix, of dimension _row x _col, where every matrix element has allocated has memory.
 *
 * The Matrix_Dense struct implements a mathematical matrix of nxm real numbers.
 *
 * To avoid massive boiler plate the matrix inherits from std::array, which implies the matrix's dimension is fixed at
 * compile time. To obtain a dynamically allocated dense matrix the _row and _col value can be set to 0.  In which case
 * std::vector is inherited, see below specialisation. Note that semi-static matrices are not supported, e.g. _row = 1,
 * _col = 0.
 */
template<>
struct Matrix_Dense<0, 0> : public std::vector<Vector_Dense<0> > {
  using _matrix = Matrix_Dense<0, 0>;         //!< Short hand for this matrix type.
  const static bool is_dynamic = true;        //!< Indicates the matrix is compile time sized.

  // -------------------------------------------------------------------------------------------------------------------
  // Constructors/Destructors
  // -------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Initialise empty matrix.
   */
  Matrix_Dense() : std::vector<Vector_Dense<0> >() {};

  /**
   * @brief Constructor to construct a matrix from an initializer list, matrix is resized to list size.
   * @param[in] list The list of Scalars to initialise the matrix. Each row much be the same size.
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
  explicit Matrix_Dense(const std::function<Scalar(std::size_t, std::size_t)>& lambda, std::size_t row,
                        std::size_t column) {
    resize(row);
    FOR(i_row, row) {
      (*this)[i_row].resize(column);
      FOR(i_column, column) (*this)[i_row][i_column] = lambda(i_row, i_column);
    }
  }

  // -------------------------------------------------------------------------------------------------------------------
  // Size Functions
  // -------------------------------------------------------------------------------------------------------------------

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
  [[nodiscard]] std::size_t size_row() const noexcept { return std::distance(cbegin(), cend()); }

  /**
   * @brief Returns the number of columns in the matrix.
   * @return The number of columns.
   */
  [[nodiscard]] std::size_t size_column() const noexcept { return empty() ? 0 : (*this)[0].size(); }

  /**
   * @brief Returns the number of rows and columns in the matrix. If rows are 0, columns are 0.
   * @return Pair containing [rows, columns].
   */
  [[nodiscard]] std::pair<std::size_t, std::size_t> size() const noexcept {
    return std::make_pair(size_row(), size_column());
  }

  // -------------------------------------------------------------------------------------------------------------------
  // Assignment Operators
  // -------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Multiplies the matrix by a scalar, A' = A*b, where A is the matrix and b is the scalar
   * @param[in] scalar Scalar value, b, to multiply the matrix by.
   * @return Updated matrix (A').
   */
  _matrix& operator*=(const Scalar& scalar) {
    FOR_EACH_REF(element, *this) element *= scalar;
    return *this;
  }

  /**
   * @brief Divides the matrix by a scalar, A' = A/b, where A is the matrix and b is a scalar.
   * @param[in] scalar Scalar value, b, to multiply the vector by.
   * @return Updated vector (A').
   *
   * @note Division by zero is left to the user to handle.
   */
  _matrix& operator/=(const Scalar& scalar) {
    FOR_EACH_REF(element, *this) element /= scalar;
    return *this;
  }

  /**
   * @brief Addition of a second matrix, A' = A + B, where A and B are matrices.
   * @tparam _row_other The number of rows of the B matrix, dynamic/static.
   * @tparam _col_other The number of column of the B matrix, dynamic/static.
   * @param[in] matrix The second matrix, B, to add.
   * @return Updated matrix (A').
   */
  template<std::size_t _row_other, std::size_t _col_other>
  constexpr _matrix& operator+=(const Matrix_Dense<_row_other, _col_other>& matrix) {
    ASSERT_DEBUG(size() == matrix.size(),
                 "Incompatible matrix dimensions, " + std::to_string(size_row()) + "," + std::to_string(size_column()) +
                 " vs. " + std::to_string(matrix.size_row()) + "," + std::to_string(matrix.size_column()) + ".");
    FOR(row, size_row()) (*this)[row] += matrix[row];
    return *this;
  }

  /**
   * @brief Subtraction by a second matrix, A' = A - B, where A and B are matrices.
   * @tparam _row_other The number of rows of the B matrix, dynamic/static.
   * @tparam _col_other The number of column of the B matrix, dynamic/static.
   * @param[in] matrix The second matrix, B, to subtract.
   * @return Updated matrix (A').
   */
  template<std::size_t _row_other, std::size_t _col_other>
  constexpr _matrix& operator-=(const Matrix_Dense<_row_other, _col_other>& matrix) {
    ASSERT_DEBUG(size() == matrix.size(),
                 "Incompatible matrix dimensions, " + std::to_string(size_row()) + "," + std::to_string(size_column()) +
                 " vs. " + std::to_string(matrix.size_row()) + "," + std::to_string(matrix.size_column()) + ".");
    FOR(row, size_row()) (*this)[row] -= matrix[row];
    return *this;
  }

  /**
   * @brief Multiplies this matrix by another matrix, A' = A*B, where A and B are matrices.
   * @tparam _row_other The number of rows of the B matrix, dynamic/static.
   * @tparam _col_other The number of column of the B matrix, dynamic/static.
   * @param[in] matrix The second matrix, B, to add.
   * @return Updated matrix (A').
   *
   * @note Number of rows and columns will change if either matrix is not square.
   */
  template<std::size_t _row_other, std::size_t _col_other>
  _matrix& operator*=(const Matrix_Dense<_row_other, _col_other>& matrix) {
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
          if(i_row_other == 0) (*this)[i_row][i_column] = 0.0;
          (*this)[i_row][i_column] += copy[i_row][i_row_other]*matrix[i_row_other][i_column];
        }
      }
    }
    return *this;
  }
};

// ---------------------------------------------------------------------------------------------------------------------
// Template Meta Programming
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Used for matrix multiplication, where the static nature of matrices must decay to dynamic.
 * @tparam _row_0 The number of rows of the A matrix, dynamic/static.
 * @tparam _col_0 The number of column of the A matrix, dynamic/static.
 * @tparam _row_1 The number of rows of the B matrix, dynamic/static.
 * @tparam _col_1 The number of column of the B matrix, dynamic/static.
 */
template<std::size_t _row_0, std::size_t _colu_0, std::size_t _row_1, std::size_t _colu_1>
struct Matrix_Static_Demoter {
  const static bool is_dynamic = _row_0 == 0 || _colu_1 == 0;
  const static std::size_t row_new = is_dynamic ? 0 : _row_0;
  const static std::size_t colu_new = is_dynamic ? 0 : _colu_1;
  typedef Matrix_Dense<row_new,
                       colu_new> type;    //!< Static Matrix type if either _row_0/_colu_0 or _row_1/_colu_1 is static else dynamic. */
};

// ---------------------------------------------------------------------------------------------------------------------
// Arithmetic Operators
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Multiplies a matrix by a scalar, C = b*A, where A, and C are matrices and b is a scalar
 * @tparam _row The number of rows of the A matrix, dynamic/static.
 * @tparam _col The number of column of the A matrix, dynamic/static.
 * @param[in] scalar The scalar value, b, to multiply the matrix by.
 * @param[in] matrix The matrix, A, to be multiplied.
 * @return New matrix, C.
 */
template<std::size_t _row, std::size_t _col>
constexpr Matrix_Dense<_row, _col> operator*(const Scalar& scalar, Matrix_Dense<_row, _col> matrix) {
  return matrix *= scalar;
}

/**
 * @brief Divides a matrix by a scalar, C = A/b, where A, and C are matrices and b is a scalar
 * @tparam _row The number of rows of the A matrix, dynamic/static.
 * @tparam _col The number of column of the A matrix, dynamic/static.
 * @param[in] matrix The matrix, A, to be divided.
 * @param[in] scalar The scalar value, b, to divide the matrix by.
 * @return New matrix, C.
 */
template<std::size_t _row, std::size_t _col>
constexpr Matrix_Dense<_row, _col> operator/(Matrix_Dense<_row, _col> matrix, const Scalar& scalar) {
  return matrix /= scalar;
}

/**
 * @brief Multiplies a matrix and vector, c = A*b, where A is a matrix and c and b are vectors.
 * @tparam _row The number of rows of the A matrix, dynamic/static.
 * @tparam _col The number of column of the A matrix, dynamic/static.
 * @tparam _size The number of column of the A matrix, dynamic/static.
 * @param[in] matrix The matrix, A, to be multiplied.
 * @param[in] vector The vector, b, to multiply the matrix by.
 * @return New vector, c.
 */
template<std::size_t _row, std::size_t _col, std::size_t _size>
typename StaticPromoter<Vector_Dense<_row>, Vector_Dense<_size> >::type
constexpr operator*(const Matrix_Dense<_row, _col>& matrix, const Vector_Dense<_size>& vector) {
  ASSERT_DEBUG(matrix.size_column() == vector.size(),
               "Incompatible vector-matrix dimensions, " + std::to_string(matrix.size_row()) + "," +
               std::to_string(matrix.size_column()) + " vs. " + std::to_string(vector.size()));
  typedef typename StaticPromoter<Vector_Dense<_row>, Vector_Dense<_size> >::type _return_vector;
  return _return_vector([&](const std::size_t i_row) { return dot_product(matrix[i_row], vector); },
                        matrix.size_column());
}

/**
 * @brief Adds two matrices together, C = A + B, where A, B, and C are matrices
 * @tparam _row_0 The number of rows of the A matrix, dynamic/static.
 * @tparam _col_0 The number of column of the A matrix, dynamic/static.
 * @tparam _row_1 The number of rows of the B matrix, dynamic/static.
 * @tparam _col_1 The number of column of the B matrix, dynamic/static.
 * @param[in] matrix_0 The first matrix, A, to add.
 * @param[in] matrix_1 The second matrix, B, to add.
 * @return New matrix, C.
 */
template<std::size_t _row_0, std::size_t _col_0, std::size_t _row_1, std::size_t _col_1>
typename StaticPromoter<Matrix_Dense<_row_0, _col_0>, Matrix_Dense<_row_1, _col_1> >::type
constexpr operator+(const Matrix_Dense<_row_0, _col_0>& matrix_0, const Matrix_Dense<_row_1, _col_1>& matrix_1) {
  ASSERT_DEBUG(matrix_0.size() == matrix_1.size(),
               "Incompatible matrix dimensions, " + std::to_string(matrix_0.size_row()) + "," +
               std::to_string(matrix_0.size_column()) + " vs. " + std::to_string(matrix_1.size_row()) + "," +
               std::to_string(matrix_1.size_column()) + ".");
  typedef typename StaticPromoter<Matrix_Dense<_row_0, _col_0>, Matrix_Dense<_row_1, _col_1> >::type _return_matrix;
  return _return_matrix(
    [&](std::size_t i_row, std::size_t i_column) { return matrix_0[i_row][i_column] + matrix_1[i_row][i_column]; },
    matrix_0.size_row(), matrix_0.size_column());
}

/**
 * @brief Subtracts one matrix from another, C = A - B, where A, B, and C are matrices
 * @tparam _row_0 The number of rows of the A matrix, dynamic/static.
 * @tparam _col_0 The number of column of the A matrix, dynamic/static.
 * @tparam _row_1 The number of rows of the B matrix, dynamic/static.
 * @tparam _col_1 The number of column of the B matrix, dynamic/static.
 * @param[in] matrix_0 The matrix, A, to be subtracted from.
 * @param[in] matrix_1 The matrix, B, to subtract by.
 * @return New matrix, C.
 */
template<std::size_t _row_0, std::size_t _col_0, std::size_t _row_1, std::size_t _col_1>
typename StaticPromoter<Matrix_Dense<_row_0, _col_0>, Matrix_Dense<_row_1, _col_1> >::type
constexpr operator-(const Matrix_Dense<_row_0, _col_0>& matrix_0, const Matrix_Dense<_row_1, _col_1>& matrix_1) {
  ASSERT_DEBUG(matrix_0.size() == matrix_1.size(),
               "Incompatible matrix dimensions, " + std::to_string(matrix_0.size_row()) + "," +
               std::to_string(matrix_0.size_column()) + " vs. " + std::to_string(matrix_1.size_row()) + "," +
               std::to_string(matrix_1.size_column()) + ".");
  typedef typename StaticPromoter<Matrix_Dense<_row_0, _col_0>, Matrix_Dense<_row_1, _col_1> >::type _return_matrix;
  return _return_matrix(
    [&](std::size_t i_row, std::size_t i_column) { return matrix_0[i_row][i_column] - matrix_1[i_row][i_column]; },
    matrix_0.size_row(), matrix_0.size_column());
}

/**
 * @brief Multiples two matrices together, C = A*B, where A, B, and C are matrices
 * @tparam _row_0 The number of rows of the A matrix, dynamic/static.
 * @tparam _col_0 The number of column of the A matrix, dynamic/static.
 * @tparam _row_1 The number of rows of the B matrix, dynamic/static.
 * @tparam _col_1 The number of column of the B matrix, dynamic/static.
 * @param[in] matrix_0 The left matrix, A, to multiply.
 * @param[in] matrix_1 The right matrix, B, to multiply.
 * @return New matrix, C.
 */
template<std::size_t _row_0, std::size_t _col_0, std::size_t _row_1, std::size_t _col_1>
typename Matrix_Static_Demoter<_row_0, _col_0, _row_1, _col_1>::type
constexpr operator*(const Matrix_Dense<_row_0, _col_0>& matrix_0, const Matrix_Dense<_row_1, _col_1>& matrix_1) {
  ASSERT_DEBUG(matrix_0.size_column() == matrix_1.size_row(),
               "Incompatible matrix dimensions, " + std::to_string(matrix_0.size_row()) + "," +
               std::to_string(matrix_0.size_column()) + " vs. " + std::to_string(matrix_1.size_row()) + "," +
               std::to_string(matrix_1.size_column()) + ".");
  typename Matrix_Static_Demoter<_row_0, _col_0, _row_1, _col_1>::type
    _return_matrix([&](const std::size_t i_row, const std::size_t i_column) {
    return matrix_0[i_row][i_column];
  }, matrix_0.size_row(), matrix_0.size_column());
  return _return_matrix *= matrix_1;
}

}

#endif //DISA_MATRIX_DENSE_H
