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
// File Name: direct_lower_upper_factorisation.h
// Description: Contains the declaration of the direct lower upper factorisation solver.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_DIRECT_UPPER_LOWER_FACTORISATION_H
#define DISA_DIRECT_UPPER_LOWER_FACTORISATION_H

#include "direct.hpp"
#include "matrix_dense.hpp"
#include "solver_utilities.hpp"
#include "vector_dense.hpp"

#include <algorithm>
#include <cmath>

namespace Disa {

/**
 * @class Direct_Lower_Upper_Factorisation
 * @brief Implements the Lower Upper Factorisation Linear Solver for dense linear systems. 
 * 
 * @tparam _solver_type 
 * @tparam _size Size of the system being solver, if zero assumed dynamically sized system.
 * @tparam _pivot Is the solver allowed to use pivoting when factorising (recommended true).
 */
template<Solver_Type _solver_type, std::size_t _size, bool _pivot>
class Direct_Lower_Upper_Factorisation
    : public Direct<Direct_Lower_Upper_Factorisation<_solver_type, _size, _pivot>, _size> {

 public:
  /**
   * @brief Construct a new Direct_Lower_Upper_Factorisation object
   * @param[in] config The configuration for the LU solver to use.
   */
  explicit Direct_Lower_Upper_Factorisation() = default;

  /**
   * @brief Construct a new Direct_Lower_Upper_Factorisation object
   * @param[in] config The configuration for the LU solver to use.
   */
  explicit Direct_Lower_Upper_Factorisation(Solver_Config config)
      : Direct<Direct_Lower_Upper_Factorisation<_solver_type, _size, _pivot>, _size>(config) {
    initialise_solver(config);
  };

  /**
   * @brief Initialises the solve, copying in the relevant config data.
   * @param[in] config The configuration for the LU solver to use.
   */
  void initialise_solver(Solver_Config config) {
    ASSERT(config.type == _solver_type, "Miss-match between config tpye and LU/LUP selection.");
    ASSERT(config.pivot == _pivot, "Miss-match between config pivoting and LU/LUP selection.");
    factorisation_tolerance = config.factor_tolerance;
  };

  /**
   * @brief Factorises the coefficient matrix, using LU(P) factorisation.
   * @param[in] a_matrix The coefficient matrix to factorise.
   * @return True if the matrix was factorised successfully, else false for degenerate/singular matrices.
   */
  bool factorise(const Matrix_Dense<Scalar, _size, _size>& a_matrix);

  /**
   * @brief Solves the linear system, using the solver's internally stored factorised coefficient matrix. 
   * @param[out] x_vector The solution vector, will be resized and modified, inital value is irrelevant.
   * @param[in] b_vector The constant vector, appropriate to the coefficient matrix which was factorised.
   * @return A Convergence_Data object detailing the solve status.
   * 
   * @warning The factorise function must have been called before this function, and returned true for a correct solver 
   * to occur.
   */
  Convergence_Data solve_system(Vector_Dense<Scalar, _size>& x_vector, const Vector_Dense<Scalar, _size>& b_vector);

  /**
   * @brief Gets the current configuration of the solver.
   * @return A configuration object, containing the current solver configuration.
   */
  constexpr Solver_Config get_config() {
    Solver_Config config;
    config.type = _solver_type;
    config.pivot = _pivot;
    config.factor_tolerance = factorisation_tolerance;
    return config;
  }

 private:
  bool factorised{false};  //<! Has factorisation been completed successfully.
  Scalar factorisation_tolerance{
  default_absolute};  //<! The value below which diagonal entires should be considered zero.
  Matrix_Dense<Scalar, _size, _size>
  lu_factorised;                    //<! The factorised LU coefficient matrix (implicit 1's on diagonal of L).
  std::vector<std::size_t> pivots;  //<! The pivots indicies, is only sized for LUP solver.
};

template<std::size_t _size>
using Solver_LUP = Direct_Lower_Upper_Factorisation<Solver_Type::lower_upper_factorisation, _size, true>;
template<std::size_t _size>
using Solver_LU = Direct_Lower_Upper_Factorisation<Solver_Type::lower_upper_factorisation, _size, false>;

// ---------------------------------------------------------------------------------------------------------------------
// Direct Lower Upper Factorisation Template Definitions
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details 
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
 * The `factorise` function returns `true` upon successful factorization. If the matrix is degenerate/singular, 
 * indicated by an entry on the diagonal below the specified tolerance (`factorisation_tolerance`), the function
 * returns `false`, signifying failure.
 */
template<Solver_Type _solver_type, std::size_t _size, bool _pivot>
bool Direct_Lower_Upper_Factorisation<_solver_type, _size, _pivot>::factorise(
const Matrix_Dense<Scalar, _size, _size>& a_matrix) {

  // Initialise factorisation data.
  factorised = false;
  lu_factorised = a_matrix;
  if(_pivot) {
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
      if(i_max != i_row) {
        std::swap(pivots[i_row], pivots[i_max]);
        std::swap(lu_factorised[i_row], lu_factorised[i_max]);
      }
    }

    // Check degeneracy.
    if(std::abs(lu_factorised[i_row][i_row]) < factorisation_tolerance) return false;

    // Decomposition, actual factorisation to compute L and U.
    for(std::size_t i_row_sweep = i_row + 1; i_row_sweep < lu_factorised.size_row(); ++i_row_sweep) {
      lu_factorised[i_row_sweep][i_row] /= lu_factorised[i_row][i_row];

      for(std::size_t i_column_sweep = i_row + 1; i_column_sweep < lu_factorised.size_column(); ++i_column_sweep)
        lu_factorised[i_row_sweep][i_column_sweep] -=
        lu_factorised[i_row_sweep][i_row] * lu_factorised[i_row][i_column_sweep];
    }
  }

  factorised = true;
  return true;
}

/**
 * @details 
 * This function solves a linear system of equations using direct lower-upper factorization. It takes an input dense 
 * vector `b_vector` representing the right-hand side of constants, and solves for the solution vector `x_vector`. A 
 * forward and backward sweep are performed with the LU factorised matrix to compute the solution. The x vector will be 
 * resized appropriately to solve, while the b-vector must be sized appropriately to the lu-factored matrix. Should no 
 * factorisation have been completed the solver returns an 'un-converged state'.
 */
template<Solver_Type _solver_type, std::size_t _size, bool _pivot>
Convergence_Data Direct_Lower_Upper_Factorisation<_solver_type, _size, _pivot>::solve_system(
Vector_Dense<Scalar, _size>& x_vector, const Vector_Dense<Scalar, _size>& b_vector) {

  ASSERT_DEBUG(b_vector.size() == lu_factorised.size_row(), "Constant vector not of the correct size.");

  Convergence_Data convergence_data = Convergence_Data();
  if(!factorised) return convergence_data;

  ++convergence_data.iteration;
  x_vector.resize(b_vector.size());
  for(std::size_t i_row = 0; i_row < lu_factorised.size_row(); ++i_row) {
    x_vector[i_row] = b_vector[_pivot ? pivots[i_row] : i_row];

    for(std::size_t i_column = 0; i_column < i_row; ++i_column)
      x_vector[i_row] -= lu_factorised[i_row][i_column] * x_vector[i_column];
  }

  for(std::size_t i_row = lu_factorised.size_row() - 1; i_row != std::numeric_limits<std::size_t>::max(); --i_row) {
    for(std::size_t i_column = i_row + 1; i_column < lu_factorised.size_column(); ++i_column)
      x_vector[i_row] -= lu_factorised[i_row][i_column] * x_vector[i_column];
    x_vector[i_row] /= lu_factorised[i_row][i_row];
  }

  convergence_data.converged = true;
  return convergence_data;
}

}  // namespace Disa

#endif  //DISA_DIRECT_UPPER_LOWER_FACTORISATION_H
