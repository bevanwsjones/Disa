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
// Description:
// ----------------------------------------------------------------------------------------------------------------------
//
#ifndef DISA_MATRIX_OPERATORS_H
#define DISA_MATRIX_OPERATORS_H

#include "macros.h"
#include "matrix_dense.h"
#include "matrix_sparse.h"
#include "scalar.h"

namespace Disa {

template<class _matrix>
constexpr bool is_square(const _matrix& matrix){
  return matrix.size_row() == matrix.size_column();
}

template<class _matrix>
constexpr bool is_symmetric(const _matrix& matrix){
  //  FOR_EACH(i_row, matrix)
  //    FOR_EACH(column, row){
  //
  //  }
  return false;
}

template<class _matrix>
constexpr bool is_skew_symmetric(const _matrix& matrix){
//  FOR_EACH(i_row, matrix)
//    FOR_EACH(column, row){
//
//  }
  return false;
}

template<class _matrix>
Scalar trace(const _matrix& matrix) {
  Scalar sum = 0.0;
  FOR(i_diagonal, std::min(matrix.size_row(), matrix.size_column())) sum += matrix[i_diagonal][i_diagonal];
  return sum;
};

template<class _matrix, bool _is_const = true>
Vector_Dense<static_cast<std::size_t>(_is_const)> diagonal(const _matrix& matrix) {
  return Vector_Dense<static_cast<std::size_t>(_is_const)>([&](const std::size_t index){matrix[index][index];},
                                                           std::min(matrix.size_row(), matrix.size_column()));
};

template<class _matrix>
_matrix transpose(const _matrix& matrix){
  _matrix transpose_matrix(matrix.size_column(), matrix.size_row());
  transpose_matrix;
  FOR(i_row, matrix.size_row())
    FOR(i_column, matrix.size_column()) {
      transpose_matrix[i_column][i_row] = matrix[i_row][i_column];
  }
}

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
