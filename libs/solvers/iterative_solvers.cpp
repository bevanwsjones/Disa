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
// File Name: iterative_solvers.cpp
// Description: Contains the class definitions for the iterative solvers, Jacobi, Gauss-Seidel, SOR <- todo : update
// ---------------------------------------------------------------------------------------------------------------------

#include "iterative_solvers.h"

#include "matrix_sparse.h"
#include "vector_operators.h"

namespace Disa {

/**
 * @details
 */
void Iterative_Solver::setup() {
  x_working.resize(matrix.size_row());
}

/**
 * @details basic jacobi
 */
ConvergenceData Iterative_Solver::solve(Vector_Dense<0>& x_vector, const Vector_Dense<0>& b_vector) {

  ConvergenceData data;

  while(data.iteration < config.maximum_iterations && data.residual > config.convergence_tolerance) {

    FOR(i_row, matrix.size_row()) {
      Scalar offs_row_dot = 0;
      FOR_ITER(column_iter, matrix[i_row]){
        if(column_iter.i_column() != i_row) offs_row_dot += *column_iter*x_vector[column_iter.i_column()];
      }
      x_working[i_row] = (b_vector[i_row] - offs_row_dot)/matrix[i_row][i_row];
    }
    data.iteration++;
    data.residual = lp_norm<2>(matrix*x_vector - b_vector)/lp_norm<2>(b_vector);
    std::swap(x_vector, x_working);
  }

  return data;
}

}