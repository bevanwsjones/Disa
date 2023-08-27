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
// File Name: direct_lower_upper_factorisation.hpp
// Description: TODO:
// ---------------------------------------------------------------------------------------------------------------------

namespace Disa {

/**
 * @details 
 * This function solves a linear system of equations using direct lower-upper factorization.
 * It takes an input dense matrix `a_matrix`, a dense vector `b_vector` representing the right-hand side,
 * and solves for the solution vector `x_vector`. The function performs LU factorization on the input matrix,
 * and then applies forward and backward substitution to compute the solution. 
 */
template<Solver_Type _solver_type, std::size_t _size, bool _pivot>
Convergence_Data Direct_Lower_Upper_Factorisation<_solver_type, _size, _pivot>::solve_system(
  const Matrix_Dense<_size, _size>& a_matrix, Vector_Dense<_size>& x_vector, const Vector_Dense<_size>& b_vector) {

  Convergence_Data convergence_data = Convergence_Data();

  if(!factorise(a_matrix)) return convergence_data;

  for (std::size_t i_row = 0; i_row < lu_factorised.size_row(); ++i_row) {
        if(_pivot) x_vector[i_row] = b_vector[pivots[i_row]];
  
        for (std::size_t i_column = 0; i_column < i_row; ++i_column)
            x_vector[i_row] -= lu_factorised[i_row][i_column] * x_vector[i_column];
    }

    for (std::size_t i_row = lu_factorised.size_row() - 1; i_row != std::numeric_limits<std::size_t>::max(); --i_row) {
        for (std::size_t i_column = i_row + 1; i_column < lu_factorised.size_column(); ++i_column)
            x_vector[i_row] -= lu_factorised[i_row][i_column] * x_vector[i_column];
        x_vector[i_row] /= lu_factorised[i_row][i_row];
    }
  return convergence_data;
}
  
/**
 * @details TODO: - how is LU stored - i.e. implicit 1 on diagonal? 
 * This function performs LU (Lower-Upper) factorization on a given dense matrix, resulting in a decomposition
 * of the matrix into a product of lower and upper triangular matrices. The LU factorization is utilized to
 * solve linear systems of equations more efficiently.
 * 
 * The LU factorization process involves the following steps:
 * 
 * 1. Initialization: The input matrix `a_matrix` is copied into the `lu_factorised` matrix. If `_pivot` is `true`, the
 *    pivots and permutation vector are initialized.
 * 2. Partial Pivoting (if enabled): If pivoting is used, the function identifies the maximum element in the current
 *    column, ensuring numerical stability and better conditioning.
 * 3. Row Reduction: For each row in the matrix, the factorization process eliminates the entries below the diagonal to
 *    create the lower triangular matrix.
 * 4. Matrix Decomposition: The factorized matrix is formed with the lower triangular part holding the multipliers used 
 *    in the row reduction, and the upper triangular part containing the modified row values.
 * 
 * The `factorise` function returns `true` upon successful factorization. If the matrix is degenerate,
 * indicated by an entry on the diagonal below a specified tolerance (`factorisation_tolerance`), the function
 * returns `false`, signifying failure.
 */
template<Solver_Type _solver_type, std::size_t _size, bool _pivot>
bool Direct_Lower_Upper_Factorisation<_solver_type, _size, _pivot>::factorise(
  const Matrix_Dense<_size, _size>& a_matrix, Scalar tolerance) {
  
  // Initlaise factorisation data.
  lu_factorised = a_matrix;
  if(_pivot){
    pivots.resize(lu_factorised.size_column());
    std::iota(pivots.begin(), pivots.end(), 0);
  }

  // Factorise A into L and U.
  for(std::size_t i_row = 0; i_row < lu_factorised.size_row(); ++i_row) {

    // Pivoting
    if(_pivot) {
      
      // Find largest remaining column value to pivot on.
      Scalar max = 0.0;
      std::size_t i_max = i_row;
      for(std::size_t i_row_sweep = i_row; i_row_sweep < lu_factorised.size_column(); ++i_row_sweep) {
        const Scalar absA = std::abs(lu_factorised[i_row_sweep][i_row]);
        if(is_nearly_greater(absA, max)) { 
            max = absA;
            i_max = i_row_sweep;
        }
      } 

      // Pivot if larger value found.
      if (i_max != i_row) {
          std::swap(pivots[i_row], pivots[i_max]);
          std::swap(lu_factorised[i_row], lu_factorised[i_max]);
          ++pivots.back();
      }
    }
    
    // Check degeneracy.
    if(lu_factorised[i_row][i_row] < factorisation_tolerance) return false; 

    // Decomposition, actual factorisation to compute L and U.
    for(std::size_t i_row_sweep = i_row + 1; i_row_sweep < lu_factorised.size_row(); ++i_row_sweep) {
        lu_factorised[i_row_sweep][i_row] /= lu_factorised[i_row][i_row];

        for (std::size_t i_column_sweep = i_row + 1; i_column_sweep < lu_factorised.size_column(); ++i_column_sweep)
            lu_factorised[i_row_sweep][i_column_sweep] -= lu_factorised[i_row_sweep][i_row] * lu_factorised[i_row][i_column_sweep];
    }
  }   
  return true;
}
  
} // namespace Disa
