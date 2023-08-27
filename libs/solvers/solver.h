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
// File Name: solver.h
// Description: Contains the base definitions for the solver library.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_SOLVERS_H
#define DISA_SOLVERS_H

#include "solver_fixed_point.h"

#include "scalar.h"
#include "solver_utilities.h"

#include "memory"

namespace Disa{

// Forward declarations
class Matrix_Sparse;
template<std::size_t> class Vector_Dense;

class Solver {
public:

  explicit Solver() = default;

  std::variant<std::unique_ptr<Solver_Jacobi>,
               std::unique_ptr<Solver_Gauss_Seidel>,
               std::unique_ptr<Sover_Sor>,
               std::nullptr_t> solver{nullptr};

  Convergence_Data solve(const Matrix_Sparse& a_matrix, Vector_Dense<0>& x_vector,
                                const Vector_Dense<0>& b_vector) {

    switch(solver.index()) {
      case 0: return std::get<std::unique_ptr<Solver_Jacobi> >(solver)->solve_system(a_matrix, x_vector, b_vector);
      case 1: return std::get<std::unique_ptr<Solver_Gauss_Seidel> >(solver)->solve_system(a_matrix, x_vector, b_vector);
      case 2: return std::get<std::unique_ptr<Sover_Sor> >(solver)->solve_system(a_matrix, x_vector, b_vector);
      default:
        ERROR("Unknown or uninitialized solver.");
        exit(1);
    }
  };

};

inline Solver build_solver(Solver_Config config){
  Solver solver;
  switch(config.type) {
    case Solver_Type::jacobi:
      solver.solver = std::make_unique<Solver_Jacobi>(config);
      break;
    case Solver_Type::gauss_seidel:
      solver.solver = std::make_unique<Solver_Gauss_Seidel>(config);
      break;
    case Solver_Type::successive_over_relaxation:
      solver.solver = std::make_unique<Sover_Sor>(config);
      break;
    default:
      ERROR("Undefined.");
      exit(0);
  }
  return solver;
}



}

#endif //DISA_SOLVERS_H
