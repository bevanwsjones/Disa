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
// File Name: solver_utilities.hpp
// Description: Contains the definitions of the template functions for solver utility functions.
// ---------------------------------------------------------------------------------------------------------------------

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Convergence Tracking
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details Updates the convergence data for the parsed linear system. First the weighted l-norms are computed for the
 * linear system. These residual norms are further modified to normalised to their values computed on the first
 * iteration. The iteration counter is also incremented and the duration of the solver is updated.
 */
template<class _matrix, class _vector>
void Convergence_Data::update(const _matrix& coef, const _vector& solution, const _vector& constant) {

  std::tie(residual, residual_max) = compute_residual(coef, solution, constant);

  if(!iteration) {
    residual_0 = residual;
    residual_max_0 = residual_max;
  }
  residual_normalised = residual/residual_0;
  residual_max_normalised = residual_max/residual_max_0;

  ++iteration;
  duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time);
}

/**
 * @details Computes the size weighed l-norms of the residual vector in a computationally efficient way. This is
 * achieved via fusing the matrix-vector and l_2 norm operations in a single loop. For the linear system
 *
 * A*x = b,
 *
 * the residual vector is defined as
 *
 * r = Ax - b,
 *
 * and the size weighted l_2-norm, and l_inf norm as
 *
 * |r|_{2,n} = (1/n sum_i r_i^2)^{1/2},
 * |r|_{inf} = max_i |r_i|.
 *
 * where
 * A - The coefficient matrix of the linear system.
 * x - The current solution vector of the linear system.
 * b - The constant vector of the linear system.
 * r - The residual vector of the linear system.
 * n - The size (number of rows/columns) of the system.
 */
template<class _matrix, class _vector>
std::pair<Scalar, Scalar> compute_residual(const _matrix& coef, const _vector& solution, const _vector& constant) {

  // Check sizes
  ASSERT_DEBUG(solution.size() != 0 && constant.size() != 0 , "System size is 0.");
  ASSERT_DEBUG(coef.size_row() == solution.size(),
               "Coefficient matrix row size incompatible with solution vector size.");
  ASSERT_DEBUG(coef.size_column() == constant.size(),
               "Coefficient matrix column size incompatible with constant vector size.");

  Scalar l2_norm = 0.0;
  Scalar linf_norm = 0.0;
  for(auto i_row = 0; i_row < coef.size_row(); ++i_row) {
    Scalar matrix_vector_row = 0;
    for(auto column_iter = coef[i_row].cbegin(); column_iter != coef[i_row].cend(); ++column_iter)
      matrix_vector_row += *column_iter*solution[column_iter.i_column()];

    // Compute the l2_norm and l_inf norm.
    const Scalar& row_residual_squared = std::pow(matrix_vector_row - constant[i_row], 2);
    l2_norm += row_residual_squared;
    linf_norm = std::max(linf_norm, row_residual_squared);
  }

  // 'Weight' the l_2 norm and return both norms.
  return {std::sqrt(l2_norm/static_cast<Scalar>(solution.size())), std::sqrt(linf_norm)};
}

}