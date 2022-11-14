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

template<std::size_t _p_value, class _matrix>
constexpr Scalar lp_norm(const _matrix& matrix){
  switch(_p_value){
    case 0:
      return 1;
    case 1:
    case 2:
    default:
      exit(0);
  }
}

template<std::size_t _pq_value, class _matrix>
constexpr Scalar lpq_norm(const _matrix& matrix){

  switch(_pq_value) {
    case 0:
      return std::cbrt(std::accumulate(matrix.begin(), matrix.end(), 0.0, [](Scalar a, auto b) {
        return std::max(a, lp_norm<0>(b));
      }));
    case 1:
      return std::cbrt(std::accumulate(matrix.begin(), matrix.end(), 0.0, [](Scalar a, auto b) {
        return a + lp_norm<1>(b);
      }));
    case 2:
      return std::sqrt(std::accumulate(matrix.begin(), matrix.end(), 0.0, [](Scalar a, auto b) {
        const Scalar row_norm = lp_norm<2>(b);
        return a + row_norm*row_norm;
      }));
    case 3:
      return std::cbrt(std::accumulate(matrix.begin(), matrix.end(), 0.0, [](Scalar a, auto b) {
        const Scalar row_norm = lp_norm<3>(b);
        return a + row_norm*row_norm*row_norm;
      }));
    default:
      return std::pow(std::accumulate(matrix.begin(), matrix.end(), 0.0, [](Scalar a, auto b) {
        return a + std::pow(lp_norm<_pq_value>(b), _pq_value);
      }), 1.0/_pq_value);
  }
}

template<std::size_t _p_value, std::size_t _q_value, class _matrix>
Scalar lpq_norm(const _matrix& matrix) {

  if(_p_value != _q_value) {
    Vector_Dense<0> column_norms(matrix.size_row(), 0.0);
    FOR_EACH(row, matrix) {
      FOR_EACH(column, row){
        std::size_t i_column = std::distance(&*row.begin(), &column);
        switch(_p_value) {
          case 0:
            column_norms[i_column] = std::max(column_norms[i_column], column);
            break;
          case 1:
            column_norms[i_column] += std::abs(column);
            break;
          case 2:
            column_norms[i_column] += column*column;
            break;
          case 3:
            column_norms[i_column] += std::abs(column)*column*column;
            break;
          default:
            column_norms[i_column] += std::pow(std::abs(column), _p_value);
            break;
        }
      }
    }

    if(!static_cast<bool>(_p_value)) {
      const Scalar fraction = 1.0/static_cast<Scalar>(_p_value);
      FOR_EACH_REF(norm, column_norms) norm = std::pow(norm, fraction);
    }

    return lp_norm<_q_value>(column_norms);
  }
  else return lpq_norm<_p_value, _p_value>(matrix);
}

template<class _matrix>
constexpr Scalar frobenius_norm(const _matrix& matrix) { return lpq_norm<2, 2>(matrix); }

}

#endif //DISA_MATRIX_OPERATORS_H
