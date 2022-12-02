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
// Description:
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_SOLVERS_H
#define DISA_SOLVERS_H

#include "scalar.h"

#include "vector_operators.h"
#include "matrix_sparse.h"
#include "vector_dense.h"

namespace Disa{

struct ConvergenceData{
  std::size_t iteration{0};
  Scalar residual{0};
};

class Solver{

public:

  virtual void setup(Matrix_Sparse& a_matrix) = 0;
  virtual ConvergenceData solve(Vector_Dense<0>& x_vector, const Vector_Dense<0>& b_vector) = 0;

};

class Iterative_Solver : public Solver{

public:
  explicit Iterative_Solver(Matrix_Sparse& a_matrix) : matrix(a_matrix) {};

  void setup(Matrix_Sparse& _a_matrix) override {
  }

  ConvergenceData solve(Vector_Dense<0>& x_vector, const Vector_Dense<0>& b_vector) override;

  private:
  Matrix_Sparse& matrix;
};

}

#endif //DISA_SOLVERS_H
