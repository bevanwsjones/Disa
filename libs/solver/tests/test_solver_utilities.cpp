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

  auto start_time = std::chrono::steady_clock::now();
  Convergence_Data data;
  // Test that which must be correct after instantiation.
  EXPECT_EQ(data.iteration, 0);
  EXPECT_GT(data.start_time, start_time);

  // construct system
  Matrix_Sparse matrix_sparse(5, 5);
  matrix_sparse[0][0] = 0.5;
  matrix_sparse[1][1] = 0.5;
  matrix_sparse[2][2] = 0.5;
  matrix_sparse[3][3] = 0.5;
  matrix_sparse[4][4] = 0.5;
  Vector_Dense<Scalar, 0> solution;
  solution.resize(5, 2.0);
  Vector_Dense<Scalar, 0> constant = {3.0, -1.0, 3.0, -1.0, 3.0};
  auto [weighted_l2_norm_0, l_inf_norm_0] = compute_residual(matrix_sparse, solution, constant);


  // Test that which must be correct after instantiation.
  data.update(matrix_sparse, solution, constant);
  EXPECT_EQ(data.iteration, 1);
  EXPECT_LE(data.duration,
            std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time));
  EXPECT_EQ(data.residual, weighted_l2_norm_0);
  EXPECT_EQ(data.residual_max, l_inf_norm_0);
  EXPECT_EQ(data.residual_0, weighted_l2_norm_0);
  EXPECT_EQ(data.residual_max_0, l_inf_norm_0);
  EXPECT_EQ(data.residual_normalised, 1.0);
  EXPECT_EQ(data.residual_max_normalised, 1.0);

  // Second iteration, check  that which must be correct after instantiation.
  constant = Vector_Dense<Scalar, 0>({2.0, 0.0, 3.0, 0.0, 2.0});
  auto [weighted_l2_norm, l_inf_norm] = compute_residual(matrix_sparse, solution, constant);

  data.update(matrix_sparse, solution, constant);
  EXPECT_EQ(data.iteration, 2);
  EXPECT_LE(data.duration,
            std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time));
  EXPECT_EQ(data.residual, weighted_l2_norm);
  EXPECT_EQ(data.residual_max, l_inf_norm);
  EXPECT_EQ(data.residual_0, weighted_l2_norm_0);
  EXPECT_EQ(data.residual_max_0, l_inf_norm_0);
  EXPECT_EQ(data.residual_normalised, weighted_l2_norm/weighted_l2_norm_0);
  EXPECT_EQ(data.residual_max_normalised, l_inf_norm/l_inf_norm_0);
}

TEST(solver_utilites, is_converged) {

  // Setup criteria object.
  Convergence_Criteria criteria;
  criteria.min_iterations = 10;
  criteria.max_iteration = 100;
  criteria.tolerance = 1.0e-6;

  // Required minimum iterations not met.
  Convergence_Data data;
  data.iteration = 5;
  data.residual_normalised = criteria.tolerance*1.0e-1;
  data.residual_max_normalised = criteria.tolerance*1.0e-1;
  EXPECT_FALSE(criteria.is_converged(data));

  // Max iteration reached (force true converged), but residual is still too high.
  data.iteration = 101;
  data.residual_normalised = criteria.tolerance*1.0e+1;
  data.residual_max_normalised = criteria.tolerance*1.0e+2;
  EXPECT_TRUE(criteria.is_converged(data));

  // Residual meets convergence tolerance
  data.iteration = 50;
  data.residual_normalised = criteria.tolerance*1.0e-1;
  data.residual_max_normalised = criteria.tolerance*2.0; // demonstrate max residual higher than tolerance
  EXPECT_TRUE(criteria.is_converged(data));
}

TEST(solver_utilites, compute_residual) {

  Matrix_Sparse matrix_sparse(5, 5);

  matrix_sparse[0][0] = 0.5;
  matrix_sparse[1][1] = 0.5;
  matrix_sparse[2][2] = 0.5;
  matrix_sparse[3][3] = 0.5;
  matrix_sparse[4][4] = 0.5;
  Vector_Dense<Scalar, 0> solution;
  solution.resize(5, 2.0);

  Vector_Dense<Scalar, 0> constant = {3.0, -1.0, 3.0, -1.0, 3.0};
  auto [weighted_l2_norm, l_inf_norm] = compute_residual(matrix_sparse, solution, constant);
  EXPECT_DOUBLE_EQ(weighted_l2_norm, 2.0);
  EXPECT_DOUBLE_EQ(l_inf_norm, 2.0);

  constant = Vector_Dense<Scalar, 0>({2.0, 0.0, 3.0, 0.0, 2.0});
  std::tie(weighted_l2_norm, l_inf_norm) = compute_residual(matrix_sparse, solution, constant);
  EXPECT_DOUBLE_EQ(weighted_l2_norm, std::sqrt(8.0/5.0));
  EXPECT_DOUBLE_EQ(l_inf_norm, 2.0);

  EXPECT_DEATH(compute_residual(Matrix_Sparse(5, 2), solution, constant), ".*");
  EXPECT_DEATH(compute_residual(Matrix_Sparse(1, 5), solution, constant), ".*");
  EXPECT_DEATH(compute_residual(Matrix_Sparse(1, 1), Vector_Dense<Scalar, 0>{}, constant), ".*");
  EXPECT_DEATH(compute_residual(Matrix_Sparse(1, 1), solution, Vector_Dense<Scalar, 0>{}), ".*");
}