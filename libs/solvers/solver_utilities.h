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
// File Name: solver_utilities.h
// Description: todo
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_SOLVER_UTILITIES_H
#define DISA_SOLVER_UTILITIES_H

#include "scalar.h"
#include "matrix_sparse.h"
#include "vector_dense.h"
#include "vector_operators.h"

#include <vector>

namespace Disa {

struct Convergence_Data {
  std::size_t iteration{0};

  Scalar b_l2_norm{0};

  Scalar residual{scalar_max};
  Scalar residual_0{scalar_max};
  Scalar residual_relative{0};

  Scalar residual_max{scalar_max};
  Scalar residual_max_0{scalar_max};
  Scalar residual_max_relative{0};
};

inline void reset_convergence_data(Convergence_Data& data){
  data.iteration = 0;

  data.b_l2_norm = scalar_max;

  data.residual = scalar_max;
  data.residual_0 = scalar_max;
  data.residual_relative = scalar_max;

  data.residual_max = scalar_max;
  data.residual_max_0 = scalar_max;
  data.residual_max_relative = scalar_max;
}

template<class _matrix, class _vector>
std::pair<Scalar, Scalar> compute_residual(const _matrix& A, const _vector& x, const _vector& b, _vector& residual) {
  residual = A*x - b;
  Scalar l2_norm = 0.0;
  Scalar linf_norm = 0.0;
  uint size_residual = 0;


  FOR(i_row, A.size_row()) {

    bool is_diricheley_row = true;
    Scalar row_residual = 0;
    Scalar row_residual_squared = 0;

    FOR_ITER(column_iter, A[i_row]) {
      const auto& i_column = column_iter.i_column();
      row_residual += *column_iter*x[i_column] - b[i_column];
      row_residual_squared += std::pow(row_residual, 2);
      if(is_diricheley_row && (i_row != i_column || *column_iter != 1.0)) is_diricheley_row = false;
    }

    if(!is_diricheley_row) {
      l2_norm += row_residual_squared;
      linf_norm += std::max(linf_norm, row_residual);
      ++size_residual;
    }
  }

  return {std::sqrt(l2_norm), linf_norm}; // <-- todo: do properly
}

template<class _matrix, class _vector>
void update_convergence(const _matrix& A, const _vector& x, const _vector& b, Convergence_Data& data) {

  _vector residual;
  std::tie(data.residual, data.residual_max) = compute_residual(A, x, b, residual);

  if(!data.iteration) {
    data.b_l2_norm = lp_norm<2>(b);
    data.residual_0 = data.residual;
    data.residual_max_0 = data.residual_max;
  }
  data.residual_relative = data.residual/data.residual_0;
  data.residual_max_relative = data.residual_max/data.residual_max_0;

  data.residual /= data.b_l2_norm;
  data.residual_max /= data.b_l2_norm;

  ++data.iteration;
}





}


#endif //DISA_SOLVER_UTILITIES_H
