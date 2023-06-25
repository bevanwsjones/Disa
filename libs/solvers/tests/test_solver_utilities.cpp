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
// File Name:
// Description:
// ---------------------------------------------------------------------------------------------------------------------

#include "gtest/gtest.h"

#include "solver_utilities.h"

using namespace Disa;

TEST(solver_utilites, update_convergence) {
  EXPECT_TRUE(false);
}

TEST(solver_utilites, is_converged) {
  EXPECT_TRUE(false);
}

TEST(solver_utilites, compute_residual) {

  Matrix_Sparse matrix_sparse(5, 5);
  matrix_sparse[0][0] = 0.5;
  matrix_sparse[1][1] = 0.5;
  matrix_sparse[2][2] = 0.5;
  matrix_sparse[3][3] = 0.5;
  matrix_sparse[4][4] = 0.5;
  Vector_Dense<0> solution;
  solution.resize(5, 2.0);

  Vector_Dense<0> constant = {3.0, -1.0, 3.0, -1.0, 3.0};
  auto [weighted_l2_norm, l_inf_norm] = compute_residual(matrix_sparse, solution, constant);
  EXPECT_DOUBLE_EQ(weighted_l2_norm, 2.0);
  EXPECT_DOUBLE_EQ(l_inf_norm, 2.0);

  constant = Vector_Dense<0>({2.0, 0.0, 3.0, 0.0, 2.0});
  std::tie(weighted_l2_norm, l_inf_norm) = compute_residual(matrix_sparse, solution, constant);
  EXPECT_DOUBLE_EQ(weighted_l2_norm, std::sqrt(8.0/5.0));
  EXPECT_DOUBLE_EQ(l_inf_norm, 2.0);

  EXPECT_DEATH(compute_residual(Matrix_Sparse(5, 2), solution, constant), ".*");
  EXPECT_DEATH(compute_residual(Matrix_Sparse(1, 5), solution, constant), ".*");
  EXPECT_DEATH(compute_residual(Matrix_Sparse(1, 1), Vector_Dense<0>{}, constant), ".*");
  EXPECT_DEATH(compute_residual(Matrix_Sparse(1, 1), solution, Vector_Dense<0>{}), ".*");
}