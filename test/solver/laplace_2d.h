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
// File Name: laplace_2d.hpp
// Description: Classes and functions for generating a 2D laplace problem.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_LAPLACE_2D_H
#define DISA_LAPLACE_2D_H

#include "matrix_dense.hpp"
#include "matrix_sparse.hpp"
#include "scalar.hpp"
#include "vector_dense.hpp"

/**
 * @class Laplace_2D
 * @brief `
 */
class Laplace_2D 
{

public:

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
  template<class _matrix_type, class _vector_type>
  void construct_laplace_2d(_matrix_type& a_matrix, _vector_type& b_vector, _vector_type& x_vector) {

    ASSERT_DEBUG(a_matrix.size_row() == a_matrix.size_column(), "Matrix must be square.");   
    ASSERT_DEBUG(a_matrix.size_row() == b_vector.size(), "Matrix-vector size mismatch, b-vector.");
    ASSERT_DEBUG(a_matrix.size_row() == x_vector.size(), "Matrix-vector size mismatch, x-vector.");
    const int number_of_nodes = static_cast<int>(b_vector.size());
    ASSERT_DEBUG(number_of_nodes >= 9, "Must be greater equal than 9.");
    const int x_nodes = sqrt(number_of_nodes);

    a_matrix *= 0.0;
    const Disa::Scalar delta_x = static_cast<Disa::Scalar>(1.0/(x_nodes - 1.0));
    const Disa::Scalar delta_x_squared = 1.0/(delta_x*delta_x);

    // populate
    FOR(i_node, number_of_nodes)
    {
      b_vector[i_node] = source(i_node, x_nodes);
      x_vector[i_node] = analytical_solution(i_node, x_nodes);
      
      if((i_node) % x_nodes == 0 || (i_node + 1) % x_nodes == 0 || i_node < x_nodes || i_node >= (number_of_nodes - x_nodes)) {
        FOR_EACH_REF(column, a_matrix[i_node]) column = 0.0;
        a_matrix[i_node][i_node] = 1.0;
        b_vector[i_node] = analytical_solution(i_node, x_nodes);
      }
      else {
        if((i_node + x_nodes) < number_of_nodes) a_matrix[i_node][i_node + x_nodes] = -1.0*delta_x_squared;
        if(i_node%x_nodes != 0) a_matrix[i_node][i_node - 1] = -1.0*delta_x_squared;
        a_matrix[i_node][i_node] = 4*delta_x_squared;
        if((i_node + 1)%x_nodes != 0) a_matrix[i_node][i_node + 1] = -1.0*delta_x_squared;
        if((i_node - x_nodes) >= 0) a_matrix[i_node][i_node - x_nodes] = -1.0*delta_x_squared;
      }
      
    }
  }

  std::pair<Disa::Scalar, Disa::Scalar> co_ordinate(const std::size_t i_node, const std::size_t size_x) {
  const Disa::Scalar delta_x = 1.0/static_cast<Disa::Scalar>(size_x - 1.0);
  return {static_cast<Disa::Scalar>((i_node % size_x)*delta_x), 
          static_cast<Disa::Scalar>(static_cast<int>(i_node / size_x))*delta_x};
  }


  /**
   * @brief Creates the source term
   * @param i_node 
   * @param size_x 
   * @return 
   */
  Disa::Scalar source(const std::size_t i_node, const std::size_t size_x) {
    // -2 [(1 - 6x^2)y^2(y^2 - 1) + (6y^2 - 1)x^2(1 - x^2)]
    const auto [co_ordinate_x, co_ordinate_y] = co_ordinate(i_node, size_x);
    const Disa::Scalar squared_x = co_ordinate_x*co_ordinate_x;
    const Disa::Scalar squared_y = co_ordinate_y*co_ordinate_y;
    const Disa::Scalar value = -2.0*((1.0 - 6.0*squared_x)*squared_y*(squared_y - 1.0)
                                    + (6.0*squared_y - 1.0)*squared_x*(1.0 - squared_x));
    return value;
  }

  /**
   * @brief 
   * 
   * @param i_node 
   * @param size_x 
   * @return Disa::Scalar 
   */
  Disa::Scalar analytical_solution(const std::size_t i_node, const std::size_t size_x) {
    const auto [co_ordinate_x, co_ordinate_y] = co_ordinate(i_node, size_x);
    const Disa::Scalar squared_x = co_ordinate_x*co_ordinate_x;
    const Disa::Scalar squared_y = co_ordinate_y*co_ordinate_y;
    return (squared_x - squared_x*squared_x)*(squared_y*squared_y - squared_y);
  }

};

#endif //DISA_LAPLACE_2D_H