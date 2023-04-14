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
// File Name: iterative_solvers.h
// Description: Contains the class declarations for the iterative solvers, Jacobi, Gauss-Seidel, SOR <- todo : update
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_SOLVER_ITERATIVE_H
#define DISA_SOLVER_ITERATIVE_H

#include "solver_type.h"
#include "solver_utilities.h"
#include "vector_dense.h"

#include <variant>

namespace Disa {

// Forward declarations
class Matrix_Sparse;

/**
 * @brief
 */
template<class _solver>
class Solver_Iterative
{

public:
  explicit Solver_Iterative(const Solver_Config solver_config) : config(solver_config) {} ;
  void setup(){};

  const Convergence_Data& solve(const Matrix_Sparse& matrix, Vector_Dense<0>& x_vector,
                                const Vector_Dense<0>& b_vector){
    return static_cast<_solver*>(this)->solve_system(matrix, x_vector, b_vector);
  };

protected:
  Solver_Config config;
  Convergence_Data convergence_data;
};


}

#endif //DISA_SOLVER_ITERATIVE_H
