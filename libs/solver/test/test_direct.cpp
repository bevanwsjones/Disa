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

#include "laplace_2d.h"

using namespace Disa;

// ---------------------------------------------------------------------------------------------------------------------
// Testing Fixture Setup
// ---------------------------------------------------------------------------------------------------------------------


TEST(direct, direct_lower_upper_factorisation_laplace_2d) {

  // Vector_Dense<0> b_vector;
  // Vector_Dense<0> x_analytical;
  // Vector_Dense<0> x_vector;
  // Matrix_Dense<0, 0> a_matrix;

  // b_vector.resize(25);
  // x_analytical.resize(25);
  // x_vector.resize(25);
  // a_matrix.resize(25, 25);
  // std::cout<<"\n solve: ";
  // Solver_Config data;
  // data.type = Solver_Type::lower_upper_factorisation;
  // data.pivot = true;  
  //   std::cout<<"\n solve: ";
  // Solver_LUP<0> lup(data);
  //   std::cout<<"\n solve: ";
  // Laplace_2D builder; 
  //   std::cout<<"\n solve: ";
  // builder.construct_laplace_2d(a_matrix, x_analytical, b_vector);

  // std::cout<<"\n solve: ";
  // lup.factorise(a_matrix);
  // lup.solve_system(x_vector, b_vector);

  // std::cout<<"\n vector: ";
  // FOR(i_row, a_matrix.size_row()) std::cout<<std::setw(2)<<x_vector[i_row]<<", ";
  
  // std::cout<<"\n original: ";
  
  // FOR(i_row, a_matrix.size_row()) { 
  //   std::cout<<"\n";
  //   FOR(i_coloumn, a_matrix.size_column())
  //     std::cout<<std::setw(4)<<a_matrix[i_row][i_coloumn]<<", ";
  // }
  // //   std::cout<<"\n factored: ";


  // // FOR(i_row, matrix.size_row()){
  // //   std::cout<<"\n";
  // //   FOR(i_coloumn, matrix.size_column())
  // //     {}std::cout<<std::setw(4)<<lup.lu_factorised[i_row][i_coloumn]<<", ";
  // // }

  // std::cout<<"\n";
  // FOR(i_node, x_vector.size()) {
  //   std::cout<<x_vector[i_node]<<" | "<<x_analytical[i_node]<<"\n";
  // }
  
}

TEST(direct, direct_lower_upper_factorisation_death_test) {
  // construction fails for lU/LUP

  // incorrect sizes/

  // no factorised.

}


TEST(direct, direct_lower_upper_factorisation) {

  // Setup Solvers
  Solver_Config data;
  data.type = Solver_Type::lower_upper_factorisation;
  data.pivot = false;
  Solver_LU<0> lu(data);
  data.pivot = true;
  Solver_LUP<0> lup(data);

  // Setup linear system (source https://lampx.tugraz.at/~hadley/num/ch2/2.3a.php)
  Matrix_Dense<0, 0> matrix = {{2, 7, 6}, {9, 5, 1}, {4, 3, 8}};
  Vector_Dense<0> b_vector = {6, 2, 7};
  Vector_Dense<0> x_vector = {0, 0, 0};
  Vector_Dense<0> solution = {0.04166666666666696, 0.16666666666666657, 0.7916666666666667};

  // Test LU
  lu.factorise(matrix);
  lu.solve_system(x_vector, b_vector);
  EXPECT_NEAR(solution[0], x_vector[0], default_absolute);
  EXPECT_NEAR(solution[1], x_vector[1], default_absolute);
  EXPECT_NEAR(solution[2], x_vector[2], default_absolute);

  // Test LUP
  x_vector = {0, 0, 0};
  lup.factorise(matrix);
  lup.solve_system(x_vector, b_vector);
  EXPECT_NEAR(solution[0], x_vector[0], default_absolute);
  EXPECT_NEAR(solution[1], x_vector[1], default_absolute);
  EXPECT_NEAR(solution[2], x_vector[2], default_absolute);   
}