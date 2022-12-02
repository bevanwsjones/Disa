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
// File Name: Solver.cpp
// Description:
// ---------------------------------------------------------------------------------------------------------------------

#include "solver.h"

namespace Disa {

ConvergenceData Iterative_Solver::solve(Vector_Dense<0>& x_vector, const Vector_Dense<0>& b_vector) {

  ConvergenceData data;

  Vector_Dense<0> x_np1;
  x_np1.resize(x_vector.size());

  FOR(iteration, 1000) {
    FOR(i_row, matrix.size_row()) {
      Scalar offs_row_dot = 0;
      FOR_ITER(column_iter, matrix[i_row]){
        if(column_iter.i_column() != i_row) offs_row_dot += *column_iter*x_vector[column_iter.i_column()];
      }
      x_np1[i_row] = (b_vector[i_row] - offs_row_dot)/matrix[i_row][i_row];
    }
    data.iteration = iteration;

    std::cout<<"\niteration: "<<data.iteration<<"\tresidual: "<<lp_norm<2>(matrix*x_vector - b_vector)/lp_norm<2>(b_vector);

    std::swap(x_vector, x_np1);
  }


  return data;
}

}