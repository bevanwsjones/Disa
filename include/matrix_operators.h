// ----------------------------------------------------------------------------------------------------------------------
//  MIT License
//  Copyright (c) 2022 Bevan W.S. Jones
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
//  documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
//  permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
//  Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
//  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----------------------------------------------------------------------------------------------------------------------
//  File Name: matrix_operators.h
// Description: Contains functions which can check matrix types (square, symetric, etc), and perform matrix mathematical
//              operations e.g. taking the trace of a matrix.
// ----------------------------------------------------------------------------------------------------------------------

#ifndef DISA_MATRIX_OPERATORS_H
#define DISA_MATRIX_OPERATORS_H

#include "macros.h"
#include "matrix_dense.h"
#include "matrix_sparse.h"
#include "scalar.h"

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Type
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Determines if the nxm matrix A is square, i.e. n = m.
 * @tparam _matrix The matrix type, dense/sparse, dynamic/static.
 * @param[in] matrix The matrix to test.
 * @return True if n = m, else false.
 */
template<class _matrix>
constexpr bool is_square(const _matrix& matrix) {
  return matrix.size_row() == matrix.size_column();
}

/**
 * @brief Determines if the nxn matrix A is symmetric, i.e. a_i,j = a_j,i
 * @tparam _matrix The matrix type, dense/sparse, dynamic/static.
 * @param[in] matrix The matrix to test.
 * @return True if all a_i,j = a_j,i, else false.
 */
template<class _matrix>
constexpr bool is_symmetric(const _matrix& matrix) {
  if(!is_square(matrix)) return false;
    FOR(i_row, matrix.size_row())
      FOR(i_column, i_row + 1, matrix.size_column()) {
        if(matrix[i_row][i_column] != matrix[i_column][i_row]) return false;
    }
  return true;
}

/**
 * @brief Determines if the nxn matrix A is symmetric, i.e. a_i,j = a_j,i
 * @param[in] matrix The matrix to test.
 * @return True if all a_i,j = a_j,i, else false.
 */
template<>
constexpr bool is_symmetric(const Matrix_Sparse& matrix) {
  if(!is_square(matrix)) return false;
  FOR(i_row, matrix.size_row()) {
    for(auto iter = matrix.lower_bound(i_row, i_row + 1); iter != matrix[i_row].end(); ++iter) {
      if(!matrix.contains(i_row, iter.i_column()) || matrix[iter.i_column()][i_row] != *iter) return false;
    }
  }
  return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Operations
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Computes the trance of a matrix an n x m A, such that trace(A) = sum_i^min(n, m) a_i,i
 * @tparam _matrix The matrix type, dense/sparse, dynamic/static.
 * @param[in] matrix The matrix to perform the trace over.
 * @return The scalar trace(A).
 */
template<class _matrix>
Scalar trace(const _matrix& matrix) {
  Scalar sum = 0.0;
  FOR(i_diagonal, std::min(matrix.size_row(), matrix.size_column())) sum += matrix[i_diagonal][i_diagonal];
  return sum;
};

/**
 * @brief Computes the trance of a matrix an n x m A, such that trace(A) = sum_i^min(n, m) a_i,i
 * @param[in] matrix The sparse matrix to perform the trace over.
 * @return The scalar trace(A).
 */
template<>
Scalar trace(const Matrix_Sparse& matrix) {
  Scalar sum = 0.0;
  FOR(i_diagonal, std::min(matrix.size_row(), matrix.size_column()))
    if(matrix.contains(i_diagonal, i_diagonal))
      sum += matrix[i_diagonal][i_diagonal];
  return sum;
};

template<class _matrix, std::size_t _p_value, std::size_t _q_value>
constexpr Scalar lpq_norm(const _matrix& matrix){
  if(_p_value == _q_value && _p_value != 0) {
    std::accumulate(matrix.begin(), matrix.end(), 0.0 , [](const auto& row){
      return std::accumulate(row.begin(), row.end(), 0.0, [](Scalar a, Scalar b) { return a + std::abs(b); }); });
  }
  else {

  }

  return 0.0;
}

template<class _matrix>
constexpr Scalar frobenius_norm(const _matrix& matrix) { return lpq_norm<2, 2>(matrix); }

}

#endif //DISA_MATRIX_OPERATORS_H
