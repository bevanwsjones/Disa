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

#include "iterative_solvers.h"

using namespace Disa;

// ---------------------------------------------------------------------------------------------------------------------
// Testing Fixture Setup
// ---------------------------------------------------------------------------------------------------------------------

/**
 * todo
 */
class Laplace2DProblem : public ::testing::Test
{
public:
  Matrix_Sparse a_matrix;      //!< Sparse coefficient matrix of the linear system
  Vector_Dense<0> x_vector;    //!< Solution vector of the linear system.
  Vector_Dense<0> b_vector;    //!< Constant vector of the linear system.

  /**
   * @brief Calls below setup function with a mesh size of 100 grid points (10 on each axis).
   */
  void SetUp() override {
    SetUp(10);
  }

  /**
   * @brief Constructs a linear system to solve a 2D Laplace problem on a unit equi-spaced domain.
   * @param[in] size_x The number of nodes in one of the cardinal x-direction.
   *
   * @details Consider an equi-spaced structured domain of size 1 + 2dx and the Laplace equation:
   *
   * d^2phi/dx^2 + d^2phi/dy^2 = 1,
   *
   * where dx = 1/(n_x - 1), and with zero as a dirichlet boundary condition. The discrete equation for a generic (non-
   * boundary) node/vertex reads:
   *                 A                         x         =    b
   * |              ...              | |       .       |   | dx^2 |
   * |              ...              | | phi_{i - n_x} |   | dx^2 |
   * |              ...              | |       .       |   | dx^2 |
   * |              ...              | | phi_{i - 1}   |   | dx^2 |
   * | ... -1 ... -1 4 -1 ... -1 ... | | phi_{i}       | = | dx^2 |.
   * |              ...              | | phi_{i + 1}   |   | dx^2 |
   * |              ...              | |       .       |   | dx^2 |
   * |              ...              | | phi_{i + n_x} |   | dx^2 |
   * |              ...              | |       .       |   | dx^2 |
   *
   * Finally for this problem a zero initial condition is set.
   */
  void SetUp(const int size_x) {
    ASSERT_DEBUG(size_x >= 3, "Must be greater than 3.");

    const int size_xy = size_x*size_x;
    a_matrix.resize(size_xy, size_xy);
    x_vector.resize(size_xy, 0);
    b_vector.resize(size_xy, std::pow(1.0/(size_x - 1.0), 2.0));

    // populate
    FOR(node_index, size_xy)
    {
      if((node_index + size_x) < size_xy) a_matrix[node_index][node_index + size_x] = -1.0;
      if(node_index%size_x != 0) a_matrix[node_index][node_index - 1] = -1.0;
      a_matrix[node_index][node_index] = 4;
      if((node_index + 1)%size_x != 0) a_matrix[node_index][node_index + 1] = -1.0;
      if((node_index - size_x) >= 0) a_matrix[node_index][node_index - size_x] = -1.0;
    }
  }
};

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Type
// ---------------------------------------------------------------------------------------------------------------------

TEST_F(Laplace2DProblem, iterative_solver_test) {

  Scalar tolerance = 10;
  uint max_iterations = 10;

  Iterative_Solver solver(a_matrix, {2000, 1.0e-5});
  solver.setup();
  ConvergenceData result = solver.solve(x_vector, b_vector);

  std::cout<<"\n"<<result.iteration<<"\t"<<result.residual;

  // Solve and check solution.
  EXPECT_LE(result.residual, tolerance);
  EXPECT_GE(result.iteration, 1); // make sure it actual did a solve.
  EXPECT_LE(result.iteration, max_iterations);//todo more than 1 iteration
  EXPECT_LE(lp_norm<2>(a_matrix*x_vector - b_vector)/lp_norm<2>(b_vector), tolerance); // safty check - just incase the residual calc gets knocked
}