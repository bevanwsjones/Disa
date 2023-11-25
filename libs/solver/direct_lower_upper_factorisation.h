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
// File Name: direct_lower_upper_factorisation.h
// Description: Contains the declaration of the direct lower upper factorisation solver.
// ---------------------------------------------------------------------------------------------------------------------


#ifndef DISA_DIRECT_UPPER_LOWER_FACTORISATION_H
#define DISA_DIRECT_UPPER_LOWER_FACTORISATION_H

#include "direct.h"
#include "matrix_dense.h"
#include "solver_utilities.h"
#include "vector_dense.h"

#include <algorithm>
#include <cmath>

namespace Disa {

/**
 * @class Direct_Lower_Upper_Factorisation
 * @brief Implements the Lower Upper Factorisation Linear Solver for dense linear systems. 
 * 
 * @tparam _solver_type 
 * @tparam _size Size of the system being solver, if zero assumed dynamically sized system.
 * @tparam _pivot Is the solver allowed to use pivoting when factorising (recommended true).
 */
template<Solver_Type _solver_type, std::size_t _size, bool _pivot>
class Direct_Lower_Upper_Factorisation 
: public Direct<Direct_Lower_Upper_Factorisation<_solver_type, _size, _pivot>, _size > {

public:

  /**
   * @brief Construct a new Direct_Lower_Upper_Factorisation object
   * @param[in] config The configuration for the LU solver to use.
   */
  explicit Direct_Lower_Upper_Factorisation(Solver_Config config) 
  : Direct<Direct_Lower_Upper_Factorisation<_solver_type, _size, _pivot>, _size >(config) {
    initialise_solver(Solver_Config config);
  };

  /**
   * @brief Initialises the solve, copying in the relevant config data.
   * @param[in] config The configuration for the LU solver to use.
   */
  void initialise_solver(Solver_Config config) {
    factorisation_tolerance = config.factor_tolerance;
  };

  /**
   * @brief Factorises the coefficient matrix, using LU(P) factorisation.
   * @param[in] a_matrix The coefficient matrix to factorise.
   * @return True if the matrix was factorised successfully, else false for singular matrices.
   */
  bool factorise(const Matrix_Dense<_size, _size>& a_matrix);

  /**
   * @brief Solves the linear system, using the solver's internally stored factorised coefficient matrix. 
   * @param[out] x_vector The solution vector, will be resized and modified, inital value is irrelevant.
   * @param[in] b_vector The constant vector, appropriate to the coefficient matrix which was factorised.
   * @return A Convergence_Data object detailing the solve status.
   * 
   * @warning The factorise function must have been called before this function, and returned true for a correct solver 
   * to occur.
   */
  Convergence_Data solve_system(Vector_Dense<_size>& x_vector, const Vector_Dense<_size>& b_vector);

  private:
  bool factorised{false};
  Scalar factorisation_tolerance;             //<! The value below which diagonal entires should be considered zero.
  Matrix_Dense<_size, _size> lu_factorised;   //<! The factorised LU coefficient matrix (implicit 1's on diagonal of L).
  std::vector<std::size_t> pivots;            //<! The pivots indicies, is only sized for LUP solver.
};

template<std::size_t _size>
using Solver_LUP = Direct_Lower_Upper_Factorisation<Solver_Type::lower_upper_factorisation, _size, true>;
template<std::size_t _size>
using Solver_LU = Direct_Lower_Upper_Factorisation<Solver_Type::lower_upper_factorisation, _size, false> ;

}

#include "direct_lower_upper_factorisation.hpp"

#endif //DISA_DIRECT_UPPER_LOWER_FACTORISATION_H
