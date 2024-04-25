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

#include "matrix_sparse.hpp"
#include "scalar.hpp"
#include "solver_fixed_point.hpp"
#include "vector_dense.hpp"

namespace Disa {

inline void forward_sweep(const Matrix_Sparse& a_matrix,
                   const Vector_Dense<Scalar, 0>& x_vector, Vector_Dense<Scalar, 0>& x_update,
                   const Vector_Dense<Scalar, 0>& b_vector, const Scalar omega = 1) {
  // forward sweep
  FOR(i_row, a_matrix.size_row()) {
    Scalar offs_row_dot = 0;
    FOR_ITER(column_iter, a_matrix[i_row])
      if(column_iter.i_column() != i_row)
        offs_row_dot += *column_iter*x_vector[column_iter.i_column()];
    x_update[i_row] = omega*(b_vector[i_row] - offs_row_dot)/a_matrix[i_row][i_row] + (1.0 - omega)*x_vector[i_row];
  }
}

inline void backward_sweep(const Matrix_Sparse& a_matrix,
                   const Vector_Dense<Scalar, 0>& x_vector, Vector_Dense<Scalar, 0>& x_update,
                   const Vector_Dense<Scalar, 0>& b_vector, const Scalar omega = 1){
  // forward sweep
  for(auto i_row = a_matrix.size_row() - 1; i_row != std::numeric_limits<std::size_t>::max(); --i_row) {
    Scalar offs_row_dot = 0;
    FOR_ITER(column_iter, a_matrix[i_row])
      if(column_iter.i_column() != i_row)
        offs_row_dot += *column_iter*x_vector[column_iter.i_column()];
    x_update[i_row] = omega*(b_vector[i_row] - offs_row_dot)/a_matrix[i_row][i_row] + (1.0 - omega)*x_vector[i_row];
  }
}

template<>
void Solver_Fixed_Point<Solver_Type::jacobi, Solver_Fixed_Point_Jacobi_Data>::initialise_solver(Solver_Config config) {
  //tpdo assert
  data.limits.min_iterations = config.minimum_iterations;
  data.limits.max_iteration = config.maximum_iterations;
  data.limits.tolerance = config.convergence_tolerance;
}

template<>
Convergence_Data
Solver_Fixed_Point<Solver_Type::jacobi, Solver_Fixed_Point_Jacobi_Data>::solve_system(const Matrix_Sparse& a_matrix,
                                                                                      Vector_Dense<Scalar, 0>& x_vector,
                                                                              const Vector_Dense<Scalar, 0>& b_vector) {
  data.working.resize(a_matrix.size_row());
  Convergence_Data convergence_data = Convergence_Data();
  while(!data.limits.is_converged(convergence_data)) {
    forward_sweep(a_matrix, x_vector, data.working, b_vector, 1.0);
    std::swap(x_vector, data.working);
    convergence_data.update(a_matrix, x_vector, b_vector);
  }
  return convergence_data;
}

template<>
void Solver_Fixed_Point<Solver_Type::gauss_seidel, Solver_Fixed_Point_Data>::initialise_solver(Solver_Config config) {
  //tpdo assert
  data.limits.min_iterations = config.minimum_iterations;
  data.limits.max_iteration = config.maximum_iterations;
  data.limits.tolerance = config.convergence_tolerance;
}

template<>
Convergence_Data
Solver_Fixed_Point<Solver_Type::gauss_seidel, Solver_Fixed_Point_Data>::solve_system(const Matrix_Sparse& a_matrix,
                                                                                     Vector_Dense<Scalar, 0>& x_vector,
                                                                                     const Vector_Dense<Scalar, 0>& b_vector) {
  Convergence_Data convergence_data = Convergence_Data();
  while(!data.limits.is_converged(convergence_data)) {
    forward_sweep(a_matrix, x_vector, x_vector, b_vector, 1.0);
    convergence_data.update(a_matrix, x_vector, b_vector);
  }
  return convergence_data;
}

template<>
void Solver_Fixed_Point<Solver_Type::successive_over_relaxation, Solver_Fixed_Point_Sor_Data>::initialise_solver(Solver_Config config) {
  //tpdo assert
  data.limits.min_iterations = config.minimum_iterations;
  data.limits.max_iteration = config.maximum_iterations;
  data.limits.tolerance = config.convergence_tolerance;

  data.relaxation = config.SOR_relaxation;
}

template<>
Convergence_Data Solver_Fixed_Point<Solver_Type::successive_over_relaxation, Solver_Fixed_Point_Sor_Data>::solve_system(
  const Matrix_Sparse& a_matrix, Vector_Dense<Scalar, 0>& x_vector, const Vector_Dense<Scalar, 0>& b_vector) {
  Convergence_Data convergence_data = Convergence_Data();
  while(!data.limits.is_converged(convergence_data)) {
    forward_sweep(a_matrix, x_vector, x_vector, b_vector, 1.5);
    convergence_data.update(a_matrix, x_vector, b_vector);
  }

  return convergence_data;
}

}

