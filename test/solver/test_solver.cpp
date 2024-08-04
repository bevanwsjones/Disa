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
// File Name: test_solver.cpp
// Description: Unit tests for the linear solver.
// ---------------------------------------------------------------------------------------------------------------------

#include "gtest/gtest.h"

#include "matrix_dense.hpp"
#include "matrix_sparse.hpp"
#include "solver.hpp"

using namespace Disa;

// ---------------------------------------------------------------------------------------------------------------------
// Testing Fixture Setup
// ---------------------------------------------------------------------------------------------------------------------

/**
 * todo
 */
class Laplace2DProblem : public ::testing::Test {
 public:
  Matrix_Sparse a_sparse;              //!< Sparse coefficient matrix of the linear system
  Matrix_Sparse a_sparse_0;            //!< Sparse coefficient matrix of the linear system
  Vector_Dense<Scalar, 0> x_vector;    //!< Solution vector of the linear system.
  Vector_Dense<Scalar, 0> b_vector;    //!< Constant vector of the linear system.
  Vector_Dense<Scalar, 0> b_vector_0;  //!< Constant vector of the linear system.
  Matrix_Dense<Scalar, 0, 0> a_dense;  //!< Dense coefficient matrix of the linear system

  /**
   * @brief Calls below setup function with a mesh size of 100 grid points (10 on each axis).
   */
  void SetUp() override { SetUp(10); }

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

    const int size_xy = size_x * size_x;
    a_dense.resize(size_xy, size_xy);
    a_sparse.resize(size_xy, size_xy);
    x_vector.resize(size_xy, 0);
    b_vector.resize(size_xy, std::pow(1.0 / (size_x - 1.0), .0));

    FOR(i_node_0, size_xy) FOR(i_node_1, size_xy) a_dense[i_node_0][i_node_1] = 0.0;

    // populate
    FOR(i_node, size_xy) {
      if((i_node + size_x) < size_xy) {
        a_dense[i_node][i_node + size_x] = -1.0;
        a_sparse[i_node][i_node + size_x] = -1.0;
      }

      if(i_node % size_x != 0) {
        a_dense[i_node][i_node - 1] = -1.0;
        a_sparse[i_node][i_node - 1] = -1.0;
      }

      a_dense[i_node][i_node] = 4;
      a_sparse[i_node][i_node] = 4;

      if((i_node + 1) % size_x != 0) {
        a_dense[i_node][i_node + 1] = -1.0;
        a_sparse[i_node][i_node + 1] = -1.0;
      }

      if((i_node - size_x) >= 0) {
        a_dense[i_node][i_node - size_x] = -1.0;
        a_sparse[i_node][i_node - size_x] = -1.0;
      }
    }
  }

  std::pair<Scalar, Scalar> co_ordinate(const std::size_t i_node, const std::size_t size_x) {
    const Scalar delta_x = 1.0 / static_cast<Scalar>(size_x - 1);
    return {(i_node % size_x) * delta_x, static_cast<Scalar>(static_cast<int>(i_node / size_x)) * delta_x};
  }

  Scalar source(const std::size_t i_node, const std::size_t size_x) {
    // -2 [(1 - 6x^2)y^2(y^2 - 1) + (6y^2 - 1)x^2(1 - x^2)]
    const auto [co_ordinate_x, co_ordinate_y] = co_ordinate(i_node, size_x);
    const Scalar squared_x = co_ordinate_x * co_ordinate_x;
    const Scalar squared_y = co_ordinate_y * co_ordinate_y;
    const Scalar soruce = -2.0 * ((1.0 - 6.0 * squared_x) * squared_y * (squared_y - 1.0) +
                                  (6.0 * squared_y - 1.0) * squared_x * (1.0 - squared_x));
    return soruce;
  }

  Scalar analytical_solution(const std::size_t i_node, const std::size_t size_x) {
    const auto [co_ordinate_x, co_ordinate_y] = co_ordinate(i_node, size_x);
    const Scalar squared_x = co_ordinate_x * co_ordinate_x;
    const Scalar squared_y = co_ordinate_y * co_ordinate_y;
    return (squared_x - squared_x * squared_x) * (squared_y * squared_y - squared_y);
  }

  void construct_2D_laplace_source(const int size_x) {  // populate

    ASSERT_DEBUG(size_x >= 3, "Must be greater than 3.");

    const Scalar k = 1.0;
    const int size_xy = size_x * size_x;
    a_sparse_0.resize(size_xy, size_xy);
    x_vector.resize(size_xy, 0);
    b_vector_0.resize(size_xy, 0);

    const Scalar delta_x = 1.0 / (size_x - 1.0);
    const Scalar delta_x_squared = 1.0 / (delta_x * delta_x);

    FOR(i_node, size_xy) {
      b_vector_0[i_node] = source(i_node, size_x) / k;

      if((i_node) % size_x == 0 || (i_node + 1) % size_x == 0 || i_node < size_x || i_node >= (size_xy - size_x)) {
        FOR_EACH_REF(column, a_sparse_0[i_node]) column = 0.0;
        a_sparse_0[i_node][i_node] = 1.0;
        x_vector[i_node] = analytical_solution(i_node, size_x);
        b_vector_0[i_node] = analytical_solution(i_node, size_x);
      } else {
        if((i_node + size_x) < size_xy) a_sparse_0[i_node][i_node + size_x] = -1.0 * delta_x_squared;
        if(i_node % size_x != 0) a_sparse_0[i_node][i_node - 1] = -1.0 * delta_x_squared;
        a_sparse_0[i_node][i_node] = 4.0 * delta_x_squared;
        if((i_node + 1) % size_x != 0) a_sparse_0[i_node][i_node + 1] = -1.0 * delta_x_squared;
        if((i_node - size_x) >= 0) a_sparse_0[i_node][i_node - size_x] = -1.0 * delta_x_squared;
      }
    }
  }
};

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Type
// ---------------------------------------------------------------------------------------------------------------------

TEST_F(Laplace2DProblem, heat) {

  // Solver_Config data;
  // data.type = Solver_Type::jacobi;

  // data.maximum_iterations = 1000;
  // data.convergence_tolerance = 1.0e-11;

  // int size = 40;
  // const int size_xy = size*size;
  // construct_2D_laplace_source(size);
  // FOR(i_node, x_vector.size())  x_vector[i_node] = analytical_solution(i_node, size);

  // // writeout
  // FOR(i_row, size_xy) {
  //   //std::cout <<"\n|";
  //   //FOR(i_column, size_xy) {
  //   //  if(a_matrix_0.contains(i_row, i_column)) std::cout<<std::setw(7)<<a_matrix_0[i_row][i_column];
  //   //  else std::cout<<"      .";
  //   //}
  //   //std::cout <<"|";
  //   //std::cout<<std::setw(7)<<x_vector[i_row];
  //   //if(i_row%(i_row/2) == 0) std::cout<<" = ";
  //   //std::cout<<"   ";
  //   //std::cout<<std::setw(7)<<b_vector_0[i_row];
  // }

  // //std::cout<<"\n";
  // Vector_Dense<0> residual_0 = a_sparse_0*x_vector - b_vector_0;
  // std::cout<<"\n"<<lp_norm<2>(residual_0);
  // //FOR(i_node, x_vector.size()) {
  // //  std::cout<<" "<<residual_0[i_node];
  // //}

  // Solver solver = build_solver(data);
  // Convergence_Data result = solver.solve(a_sparse_0, x_vector, b_vector_0);
  // std::cout<<"\nSOR: "<<result.iteration<<"\t"<<result.residual;

  // std::cout<<"\n";
  // std::vector<Scalar> residual(size_xy, std::numeric_limits<Scalar>::max());
  // residual_0 = a_sparse_0*x_vector - b_vector_0;
  // FOR(i_node, x_vector.size()) {
  //   residual[i_node] = x_vector[i_node] - analytical_solution(i_node, size);
  //   std::cout<<x_vector[i_node]<<" | "<<analytical_solution(i_node, size)<<"\n";
  // }
  // std::cout<<"\n";
}

TEST_F(Laplace2DProblem, iterative_solver_test) {

  Solver_Config data;
  data.type = Solver_Type::jacobi;
  data.maximum_iterations = 1000;
  data.convergence_tolerance = 1.0e-5;
  Solver solver = build_solver(data);

  std::fill(x_vector.begin(), x_vector.end(), 10.0);
  Convergence_Data result = solver.solve(a_sparse, x_vector, b_vector);
  std::cout << "\nJ:  " << result.iteration << "\t" << result.residual_normalised << "\t" << result.duration.count()
            << "us";
  std::cout << "\n";

  data.type = Solver_Type::gauss_seidel;
  solver = build_solver(data);
  std::fill(x_vector.begin(), x_vector.end(), 10.0);
  result = solver.solve(a_sparse, x_vector, b_vector);
  std::cout << "\nGS:  " << result.iteration << "\t" << result.residual_normalised << "\t" << result.duration.count()
            << "us";
  std::cout << "\n";

  data.type = Solver_Type::successive_over_relaxation;
  data.SOR_relaxation = 1.5;
  solver = build_solver(data);
  std::fill(x_vector.begin(), x_vector.end(), 10.0);
  result = solver.solve(a_sparse, x_vector, b_vector);
  std::cout << "\nSOR: " << result.iteration << "\t" << result.residual_normalised << "\t" << result.duration.count()
            << "us";
  std::cout << "\n";
}
