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

#include "scalar.h"
#include "solver_utilities.h"

namespace Disa{

// Forward declarations
class Matrix_Sparse;
template<std::size_t> class Vector_Dense;

/**
 * @struct Options
 * @brief
 */
struct SolverConfig {
  std::size_t maximum_iterations{0}; //!<
  Scalar convergence_tolerance{0};       //!<
};

/**
 * @brief
 */
class Solver {

public:
  explicit Solver(const Matrix_Sparse& a_matrix, const SolverConfig solver_config) : matrix(a_matrix), config(solver_config) {} ;
  virtual void setup() = 0;
  virtual Convergence_Data solve(Vector_Dense<0>& x_vector, const Vector_Dense<0>& b_vector) = 0;

protected:
  const Matrix_Sparse& matrix;
  SolverConfig config;
};

}

#endif //DISA_SOLVERS_H
