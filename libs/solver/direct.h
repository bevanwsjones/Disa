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
   * @brief Creates a default Direct Solver.
   */
  explicit Direct() = default;

  /**
   * @brief Destroy the Direct Solver.
   */
   ~Direct() = default;

  /**
   * @brief Construct a new Direct object
   * @param config Configured solver configuration object, containing parameters for the solver.
   */
  explicit Direct(const Solver_Config config) {
    initialise(config);
  };

  /**
   * @brief Initialises the solver, e.g. allocates memory, sets internal variables etc.
   * @param[in] config Configured solver configuration object, containing parameters for the solver.
   */
  void initialise(Solver_Config config){
    return static_cast<_solver*>(this)->initialise_solver(config);
  };

  /**
   * @brief Factorises the coefficient matrix. 
   * @param[in] a_matrix The coefficient matrix to factorise.
   * @return True if the matrix was factorised successfully, else false.
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
  const Convergence_Data& solve(Vector_Dense<_size>& x_vector, const Vector_Dense<_size>& b_vector){
    return static_cast<_solver*>(this)->solve_system(x_vector, b_vector);
  };

  /**
   * @brief Get the configuration of the solver.
   * @return A configuration object with the solvers state.
   */
  const Solver_Config get_config() {
    return static_cast<_solver*>(this)->get_config();
  }

};

}

#endif //DISA_SOLVER_DIRECT_H
