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
//  File Name:
//  Description:
// ----------------------------------------------------------------------------------------------------------------------


namespace Disa {


template<>
const Convergence_Data& Solver_Fixed_Point<Solver_Type::jacobi>::solve_system(const Matrix_Sparse& a_matrix,
                                                                       Vector_Dense<0>& x_vector,
                                                                       const Vector_Dense<0>& b_vector) {
  x_working.resize(a_matrix.size_row());
  reset_convergence_data(convergence_data);
  auto& update_vector = x_working;

  while(convergence_data.iteration < config.maximum_iterations
        && convergence_data.residual > config.convergence_tolerance) {

    // forward sweep
    FOR(i_row, a_matrix.size_row()) {
      Scalar offs_row_dot = 0;
      FOR_ITER(column_iter, a_matrix[i_row])if(column_iter.i_column() != i_row)
          offs_row_dot += *column_iter*x_vector[column_iter.i_column()];
      update_vector[i_row] = (b_vector[i_row] - offs_row_dot)/a_matrix[i_row][i_row];
    }

    update_convergence(a_matrix, x_vector, b_vector, convergence_data);
    std::swap(x_vector, x_working);
  }

  return convergence_data;
}

template<>
const Convergence_Data& Solver_Fixed_Point<Solver_Type::gauss_seidel>::solve_system(const Matrix_Sparse& a_matrix,
                                                                                    Vector_Dense<0>& x_vector,
                                                                                    const Vector_Dense<0>& b_vector) {

  reset_convergence_data(convergence_data);
  auto& update_vector =  x_vector ;

  while(convergence_data.iteration < config.maximum_iterations
        && convergence_data.residual > config.convergence_tolerance) {

    // forward sweep
    FOR(i_row, a_matrix.size_row()) {
      Scalar offs_row_dot = 0;
      FOR_ITER(column_iter, a_matrix[i_row])if(column_iter.i_column() != i_row)
          offs_row_dot += *column_iter*x_vector[column_iter.i_column()];
      update_vector[i_row] = (b_vector[i_row] - offs_row_dot)/a_matrix[i_row][i_row];
    }

    update_convergence(a_matrix, x_vector, b_vector, convergence_data);
  }

  return convergence_data;
}


template<>
const Convergence_Data& Solver_Fixed_Point<Solver_Type::successive_over_relaxation>::solve_system(
  const Matrix_Sparse& a_matrix, Vector_Dense<0>& x_vector, const Vector_Dense<0>& b_vector) {

  ERROR("not implemented");
  exit(0);

  return convergence_data;
}

}
