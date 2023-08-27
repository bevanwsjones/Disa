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
// File Name: solver_utilities.h
// Description: Contains the declarations of common solver related objects and functions.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_SOLVER_UTILITIES_H
#define DISA_SOLVER_UTILITIES_H

#include "scalar.h"
#include "matrix_sparse.h"
#include "vector_dense.h"
#include "vector_operators.h"

#include <chrono>

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Solver Configuration
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @enum Solver_Type
 * @brief Enumerated list of all linear solvers in Disa.
 */
enum class Solver_Type {
  lower_upper_factorisation,                       //!< TODO:
  jacobi,                       //!< The Jacobi fixed point iterative solver.
  gauss_seidel,                 //!< The Gauss Seidel fixed point iterative solver.
  successive_over_relaxation,   //!< The Successive Over Relaxation fixed point iterative solver.
  unknown                       //!< Uninitialised/Unknown solver.
};

/**
 * @struct Solver_Config
 * @brief Contains all possible configurations for all solvers in Disa.
 */
struct Solver_Config {

  // General Configuration
  Solver_Type type {Solver_Type::unknown};   //!< The solver to construct.

  // ---------------------------------------------------------------------------------------------------------------------
  // Direct Solver Options
  // ---------------------------------------------------------------------------------------------------------------------

  bool pivot{true};

  // ---------------------------------------------------------------------------------------------------------------------
  // Iterative Solver Options
  // ---------------------------------------------------------------------------------------------------------------------

  // Convergence Configurations 
  std::size_t minimum_iterations {0};        //!< The minimum 'force' number of iterations during for a solve.
  std::size_t maximum_iterations {0};        //!< The maximum allowable iterations during a solve.
  Scalar convergence_tolerance {0};          //!< The convergence tolerance below which a solve is considered converged.

  // Iterative
  Scalar SOR_relaxation {1.5};               //!< The relaxation factor for a  Successive Over Relaxation solver.
};

// ---------------------------------------------------------------------------------------------------------------------
// Convergence Tracking
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @stuct Convergence_Data
 * @brief Contains data to track the convergence progress of a solver.
 *
 * @details
 * Typically returned by a solver after it has solved the linear system, and can be used to asses the performance of the
 * solve. To this end the struct contains details, including the duration, the number of iterations, various scalar
 * residuals of the solve. Solvers will typically update the struct by calling the update() method, which represents
 * completion of an iteration.
 *
 * @note
 * While the library remains under construction, the exact computation of the residual may vary. As such the details are
 * deliberately excluded here and one should consult the update() method and its call stack.
 */
struct Convergence_Data {

  std::chrono::microseconds duration {0};                                               //!< The duration of the solve.
  std::chrono::steady_clock::time_point start_time {std::chrono::steady_clock::now()};  //!< The time at which the object was created.

  std::size_t iteration {0};                      //!< The number of iterations performed by the solver

  Scalar residual {scalar_max};                   //!< The un-normalised weighted l2 norm of the residual vector.
  Scalar residual_0 {scalar_max};                 //!< The initial l2 norm of the residual vector of the system (before the first solve).
  Scalar residual_normalised {scalar_max};        //!< The l2 norm of the residual vector normalised to the l2 norm of the initial residual vector.

  Scalar residual_max {scalar_max};               //!< The un-normalised linf norm of the residual vector.
  Scalar residual_max_0 {scalar_max};             //!< The initial l2 norm of the residual vector of the system (before the first solve).
  Scalar residual_max_normalised {scalar_max};    //!< The linf norm of the residual vector normalised to the l2 norm of the initial residual vector.

  /**
   * @brief Updates the convergence state of a linear system (Ax = b) by computing various residual norms and data.
   * @tparam _matrix Matrix type, dense/sparse/dynamic/static/etc, of A.
   * @tparam _vector Vector type, dynamic/static, of x and b.
   * @param[in] coef The (sparse) coefficient matrix of the linear system, A.
   * @param[in] solution The solution vector of the linear system, x.
   * @param[in] constant The constant vector of the linear system, b.
   */
  template<class _matrix, class _vector>
  void update(const _matrix& coef, const _vector& solution, const _vector& constant);
};

/**
 * @struct Convergence_Criteria
 * @brief Contains the criteria values against which convergence status can be assessed.
 *
 * @details
 * This struct holds the values need to assess convergence of a solve. It is typically instances are created
 * in the solvers themselves, and initialised on construction of the solver. The method is_converged is used to
 * determine if a solve has converged. The criteria to satisfy convergence are as follows:
 * 1. i > i_min
 * 2. i < i_max
 * 3. |r|_{2, n} < epsilon
 * 4. |r|_inf < 10.0*epsilon
 *
 * where
 * i is the number of iterations
 * i_min is the minimum number of iterations.
 * i_max is the maximum number of iterations.
 * r is the residual vector of the linear system.
 * epsilon is a user defined scalar tolerance.
 * n ia the cardinality of the linear system (number of rows).
 */
struct Convergence_Criteria {

  std::size_t min_iterations {0};                                         //!< The minimum number of iterations.
  std::size_t max_iteration {std::numeric_limits<std::size_t>::max()};    //!< The maximum allowable number of iterations.
  Scalar tolerance {scalar_max};                                          //!< The convergence tolerance.

  /**
   * @brief Checks the parsed convergence data against the criteria (see struct comment for the criteria).
   * @param[in] data The convergence data (state) of the system.
   * @return True if converged, else false.
   */
  [[nodiscard]] inline bool is_converged(const Convergence_Data& data) const {
    if(data.iteration < min_iterations) return false;
    if(data.iteration > max_iteration) return true;
    if(data.residual_normalised > tolerance) return false;
    if(data.residual_max_normalised > 10.0*tolerance) return false;
    return true;
  }
};

/**
 * @brief Efficiently computes various scalar residual's of a linear system, e.g. |r|= |Ax - b|
 * @tparam _matrix Matrix type, dense/sparse/dynamic/static/etc, of A.
 * @tparam _vector Vector type, dynamic/static, of x and b.
 * @param[in] coef The (sparse) coefficient matrix of the linear system, A.
 * @param[in] solution The solution vector of the linear system, x.
 * @param[in] constant The constant vector of the linear system, b.
 * @return The l2_norm (accounting for the system size, n) and l_inf norms of the residual vector, r.
 */
template<class _matrix, class _vector>
std::pair<Scalar, Scalar> compute_residual(const _matrix& coef, const _vector& solution, const _vector& constant);

}

#include "solver_utilities.hpp"

#endif //DISA_SOLVER_UTILITIES_H
