// ----------------------------------------------------------------------------------------------------------------------
//  MIT License
//  Copyright (c) 2022 Bevan W.S. Jones
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
//  documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
//  permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
//  Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
//  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----------------------------------------------------------------------------------------------------------------------
//  File Name: solver_fixed_point.hpp
//  Description:
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_SOLVER_FIXED_POINT_H
#define DISA_SOLVER_FIXED_POINT_H

#include "solver_iterative.hpp"

namespace Disa {

struct Solver_Fixed_Point_Data : public Solver_Data{
};

struct Solver_Fixed_Point_Jacobi_Data : public Solver_Data{
  Vector_Dense<Scalar, 0> working;
};

struct Solver_Fixed_Point_Sor_Data : public Solver_Data{
  Scalar relaxation{1.5};
};

/**
 * @class
 * @brief
 *
 */
template<Solver_Type _solver_type, class _solver_data>
class Solver_Fixed_Point : public Solver_Iterative<Solver_Fixed_Point<_solver_type, _solver_data>, _solver_data> {

public:

  explicit Solver_Fixed_Point(Solver_Config config) : Solver_Iterative<Solver_Fixed_Point<_solver_type, _solver_data>, _solver_data >(config) {};

  void initialise_solver(Solver_Config config);

  /**
   * @brief  basic jacobi
   * @param a_matrix
   * @param x_vector
   * @param b_vector
   * @return
   */
  Convergence_Data solve_system(const Matrix_Sparse& a_matrix, Vector_Dense<Scalar, 0>& x_vector,
                                       const Vector_Dense<Scalar, 0>& b_vector);
};

typedef Solver_Fixed_Point<Solver_Type::jacobi, Solver_Fixed_Point_Jacobi_Data> Solver_Jacobi;
typedef Solver_Fixed_Point<Solver_Type::gauss_seidel, Solver_Fixed_Point_Data> Solver_Gauss_Seidel;
typedef Solver_Fixed_Point<Solver_Type::successive_over_relaxation, Solver_Fixed_Point_Sor_Data> Sover_Sor;

}

#endif //DISA_SOLVER_FIXED_POINT_H
