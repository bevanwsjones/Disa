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

#ifndef DISA_ITERATIVE_SOLVERS_H
#define DISA_ITERATIVE_SOLVERS_H

#include "solver.h"
#include "vector_dense.h"

namespace Disa{

// Forward declarations
class Matrix_Sparse;

/**
 * @class
 * @brief
 *
 */
class Iterative_Solver : public Solver {

public:
  explicit Iterative_Solver(const Matrix_Sparse& a_matrix, SolverConfig config) : Solver(a_matrix, config) {

  };

  /**
   * @brief
   */
  void setup() override;

  /**
   * @brief  basic jacobi
   * @param x_vector
   * @param b_vector
   * @return
   */
  Convergence_Data solve(Vector_Dense<0>& x_vector, const Vector_Dense<0>& b_vector) override;

private:
  Vector_Dense<0> x_working;
};

}

#endif //DISA_ITERATIVE_SOLVERS_H
