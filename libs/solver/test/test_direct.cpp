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
// File Name: test_matrix_operators.h
// Description: Unit tests for matrix operations.
// ---------------------------------------------------------------------------------------------------------------------

#include "gtest/gtest.h"

#include "matrix_sparse.h"
#include "matrix_dense.h"
#include "solver.h"

using namespace Disa;
class direct_solvers : public ::testing::Test {

  public: 

  Solver_LU<0>lu_solver;
  Solver_LUP<0>lup_solver;

  /**
   * @brief Sets up simple solvers.
   */ 
  void SetUp() override { 
    Solver_Config data;
    data.type = Solver_Type::lower_upper_factorisation;
    data.pivot = false;
    lu_solver.initialise(data);
    data.pivot = true;
    lup_solver.initialise(data);
  }
};

TEST_F(direct_solvers, lower_upper_factorisation_initialise) {

  Solver_Config data;
  data.type = Solver_Type::lower_upper_factorisation;
  data.factor_tolerance = 5.0;
  data.pivot = false;

  lu_solver.initialise(data);
  auto lu_data = lu_solver.get_config();
  EXPECT_EQ(lu_data.type, Solver_Type::lower_upper_factorisation);
  EXPECT_EQ(lu_data.pivot, false);
  EXPECT_EQ(lu_data.factor_tolerance, data.factor_tolerance);

  data.pivot = true;
  lup_solver.initialise(data);
  auto lup_data = lup_solver.get_config();
  EXPECT_EQ(lup_data.type, Solver_Type::lower_upper_factorisation);
  EXPECT_EQ(lup_data.pivot, true);
  EXPECT_EQ(lup_data.factor_tolerance, data.factor_tolerance);  
}

TEST_F(direct_solvers, lower_upper_factorisation_death_test) {

  // Check pivoting mis-match
  Solver_Config data;
  data.type = Solver_Type::lower_upper_factorisation;
  data.pivot = false;
  EXPECT_DEATH(Solver_LUP<0>lup(data), "./*");
  data.pivot = true;
  EXPECT_DEATH(Solver_LU<0>lup(data), "./*");

  // Check incorrect solver type
  data.type = Solver_Type::unknown; // anything other than lower_upper_factorisation
  data.pivot = false;
  EXPECT_DEATH(Solver_LU<0>lup(data), "./*");
  data.pivot = true;
  EXPECT_DEATH(Solver_LUP<0>lup(data), "./*");

  // incorrect sizes
  Vector_Dense<0> b_vector = {6, 2};
  Vector_Dense<0> x_vector = {0, 0, 0};
  Matrix_Dense<0, 0> matrix = {{2, 7, 6}, {9, 5, 1}, {4, 3, 8}};
  lu_solver.factorise(matrix);
  lup_solver.factorise(matrix);  
  EXPECT_DEATH(lu_solver.solve_system(x_vector, b_vector), "./*");
  EXPECT_DEATH(lup_solver.solve_system(x_vector, b_vector), "./*");
}

TEST_F(direct_solvers, lower_upper_factorisation_not_factorised) {
 
  Vector_Dense<0> b_vector = {6, 2, 7};
  Vector_Dense<0> x_vector = {0, 0, 0};
  Matrix_Dense<0, 0> matrix = {{0, 0, 0}, {9, 5, 1}, {4, 3, 8}};  // singular.
  
  EXPECT_FALSE(lu_solver.factorise(matrix));
  EXPECT_FALSE(lup_solver.factorise(matrix)); 
  
  EXPECT_FALSE(lu_solver.solve_system(x_vector, b_vector).converged); 
  EXPECT_FALSE(lup_solver.solve_system(x_vector, b_vector).converged);  
}

TEST_F(direct_solvers, lower_upper_factorise_solve) {

  // Setup linear system (source https://lampx.tugraz.at/~hadley/num/ch2/2.3a.php)
  Matrix_Dense<0, 0> matrix = {{2, 7, 6}, {9, 5, 1}, {4, 3, 8}};
  Vector_Dense<0> b_vector = {6, 2, 7};
  Vector_Dense<0> x_vector = {0, 0, 0};
  Vector_Dense<0> solution = {0.04166666666666696, 0.16666666666666657, 0.7916666666666667};

  // Test LU
  EXPECT_TRUE(lu_solver.factorise(matrix));
  auto data = lu_solver.solve_system(x_vector, b_vector);
  EXPECT_TRUE(data.converged);
  EXPECT_EQ(data.iteration, 1);
  EXPECT_NEAR(solution[0], x_vector[0], default_absolute);
  EXPECT_NEAR(solution[1], x_vector[1], default_absolute);
  EXPECT_NEAR(solution[2], x_vector[2], default_absolute);

  // Test LUP
  x_vector = {0, 0, 0};
  EXPECT_TRUE(lup_solver.factorise(matrix));
  data = lup_solver.solve_system(x_vector, b_vector);
  EXPECT_TRUE(data.converged);
  EXPECT_EQ(data.iteration, 1);
  EXPECT_NEAR(solution[0], x_vector[0], default_absolute);
  EXPECT_NEAR(solution[1], x_vector[1], default_absolute);
  EXPECT_NEAR(solution[2], x_vector[2], default_absolute);   
}