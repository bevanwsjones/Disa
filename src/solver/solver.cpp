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
// File Name: solver.cpp
// Description: Contains the base definitions for the solver library.
// ---------------------------------------------------------------------------------------------------------------------

#include "solver.hpp"

namespace Disa {
    
Solver build_solver(Solver_Config config) {
  Solver solver;
  switch(config.type) {
    case Solver_Type::lower_upper_factorisation:
      if(config.pivot) solver.solver = std::make_unique<Solver_LUP<0> >(config);
      else solver.solver = std::make_unique<Solver_LU<0> >(config);
      break;
    case Solver_Type::jacobi:
      solver.solver = std::make_unique<Solver_Jacobi>(config);
      break;
    case Solver_Type::gauss_seidel:
      solver.solver = std::make_unique<Solver_Gauss_Seidel>(config);
      break;
    case Solver_Type::successive_over_relaxation:
      solver.solver = std::make_unique<Sover_Sor>(config);
      break;
    default:
      ERROR("Undefined.");
      exit(0);
  }
  return solver;
}


} // namespace Disa