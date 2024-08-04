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
// File Name: solver.hpp
// Description: Contains the base definitions for the solver library.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_SOLVERS_H
#define DISA_SOLVERS_H

#include "direct_lower_upper_factorisation.hpp"
#include "scalar.hpp"
#include "solver_fixed_point.hpp"
#include "solver_utilities.hpp"

#include <memory>

namespace Disa {

// Forward declarations
class Matrix_Sparse;
template<typename, std::size_t>
class Vector_Dense;

/**
 * @brief Solver
 * @brief General Solver Class for all solver types.
 * 
 * This class serves at a 'all in one' solver class for both sparse and dense systems. To achieve 
 */
class Solver {
 public:
  explicit Solver() = default;

  std::variant<std::unique_ptr<Solver_LU<0>>, std::unique_ptr<Solver_LUP<0>>, std::unique_ptr<Solver_Jacobi>,
               std::unique_ptr<Solver_Gauss_Seidel>, std::unique_ptr<Sover_Sor>, std::nullptr_t>
  solver{nullptr};

  Convergence_Data solve(const Matrix_Sparse& a_matrix, Vector_Dense<Scalar, 0>& x_vector,
                         const Vector_Dense<Scalar, 0>& b_vector) {

    switch(solver.index()) {
      case 0:
        ERROR("LU solver does not support sparse matrices.");
      case 1:
        ERROR("LUP solver does not support sparse matrices.");
      case 2:
        return std::get<std::unique_ptr<Solver_Jacobi>>(solver)->solve_system(a_matrix, x_vector, b_vector);
      case 3:
        return std::get<std::unique_ptr<Solver_Gauss_Seidel>>(solver)->solve_system(a_matrix, x_vector, b_vector);
      case 4:
        return std::get<std::unique_ptr<Sover_Sor>>(solver)->solve_system(a_matrix, x_vector, b_vector);
      default:
        ERROR("Unknown or uninitialized solver.");
        exit(1);
    }
  };

  Convergence_Data solve(Vector_Dense<Scalar, 0>& x_vector, const Vector_Dense<Scalar, 0>& b_vector) {

    switch(solver.index()) {
      case 0:
        return std::get<std::unique_ptr<Solver_LU<0>>>(solver)->solve_system(x_vector, b_vector);
      case 1:
        return std::get<std::unique_ptr<Solver_LUP<0>>>(solver)->solve_system(x_vector, b_vector);
      case 2:
        ERROR("Jacobi solver does not support sparse matrices.");
      case 3:
        ERROR("Gauss Seidel solver does not support sparse matrices.");
      case 4:
        ERROR("SOR solver does not support sparse matrices.");
      default:
        ERROR("Unknown or uninitialized solver.");
        exit(1);
    }
  };
};

Solver build_solver(Solver_Config config);

}  // namespace Disa

#endif  //DISA_SOLVERS_H
