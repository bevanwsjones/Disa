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
// File Name: direct.h
// Description: Base CRT class for the direct linear solvers. 
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_SOLVER_DIRECT_H
#define DISA_SOLVER_DIRECT_H

#include "matrix_dense.h"
#include "solver_utilities.h"
#include "vector_dense.h"

#include <variant>

namespace Disa {

/**
 * @class Direct
 * @brief Direct Dense Linear Solver Base Class.
 * 
 * @tparam _solver The direct solver type, used to implement CRT. 
 * @tparam _size The linear system size/dimension, if 0 a dynamically sized system is assumed.
 */
template<class _solver, std::size_t _size>
class Direct
{

public:
  /**
   * @brief Construct a new Direct object
   * @param config Configured solver congiguration object, containing parameters for the solver.
   */
  explicit Direct(const Solver_Config config) {
    initialise(config);
  };

  /**
   * @brief Initalises the solver, e.g. allocates memory, sets internal variables etc.
   * @param[in] config Configured solver congiguration object, containing parameters for the solver.
   */
  void initialise(Solver_Config config){
    return static_cast<_solver*>(this)->initialise_solver(config);
  };

  /**
   * @brief Solves the linear algebra system A*x=b.
   * @param[in] coef 
   * @param[out] solution 
   * @param[in] constant 
   * @return const Convergence_Data& 
   */
  const Convergence_Data& solve(const Matrix_Dense<_size, _size>& matrix, Vector_Dense<_size>& x_vector,
                                const Vector_Dense<_size>& b_vector){
    return static_cast<_solver*>(this)->solve_system(matrix, x_vector, b_vector);
  };

};

}

#endif //DISA_SOLVER_DIRECT_H
