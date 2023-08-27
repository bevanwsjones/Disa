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
// File Name: TODO: 
// Description: TODO:
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
 * @class
 * @brief
 *
 */
template<Solver_Type _solver_type, std::size_t _size, bool _pivot>
class Direct_Lower_Upper_Factorisation : public Direct<Direct_Lower_Upper_Factorisation<_solver_type, _size, _pivot>, _size > {

public:

  explicit Direct_Lower_Upper_Factorisation(Solver_Config config) 
  : Direct<Direct_Lower_Upper_Factorisation<_solver_type, _size, _pivot>, _size >(config) {};

  void initialise_solver(Solver_Config config) {};

  /**
   * @brief  basic jacobi
   * @param a_matrix
   * @param x_vector
   * @param b_vector
   * @return
   */
  Convergence_Data solve_system(const Matrix_Dense<_size, _size>& a_matrix, Vector_Dense<_size>& x_vector, 
                                const Vector_Dense<_size>& b_vector) {
  
    Convergence_Data convergence_data = Convergence_Data();

    if(!factorise(a_matrix)) return convergence_data;

    for (std::size_t i_row = 0; i_row < lu_factorised.size_row(); ++i_row) {
          if(_pivot) x_vector[i_row] = b_vector[pivots[i_row]];
    
          for (std::size_t i_column = 0; i_column < i_row; ++i_column)
              x_vector[i_row] -= lu_factorised[i_row][i_column] * x_vector[i_column];
      }

      for (std::size_t i_row = lu_factorised.size_row() - 1; i_row != std::numeric_limits<std::size_t>::max(); --i_row) {
          for (std::size_t i_column = i_row + 1; i_column < lu_factorised.size_column(); ++i_column)
              x_vector[i_row] -= lu_factorised[i_row][i_column] * x_vector[i_column];
          x_vector[i_row] /= lu_factorised[i_row][i_row];
      }
    return convergence_data;
  };

  //------
  Matrix_Dense<_size, _size> lu_factorised;
  std::vector<std::size_t> pivots;      
  //------
  private:
            

  bool factorise(const Matrix_Dense<_size, _size>& a_matrix, Scalar tolerance = scalar_epsilon*10000.0) {
    
    lu_factorised = a_matrix;
    if(_pivot){
      pivots.resize(lu_factorised.size_column() + 1);
      for(std::size_t i_element = 0; i_element < pivots.size(); ++i_element)
        pivots[i_element] = i_element < lu_factorised.size_column() ? i_element : 0;
    }
  
    for(std::size_t i_row = 0; i_row < lu_factorised.size_row(); ++i_row) {
 
      if(_pivot){

        Scalar maxA = 0.0;
        std::size_t imax = i_row;

        for(std::size_t i_row_sweep = i_row; i_row_sweep < lu_factorised.size_column(); ++i_row_sweep) {
          auto absA = std::fabs(lu_factorised[i_row_sweep][i_row]);
          if (absA > maxA) { 
              maxA = absA;
              imax = i_row_sweep;
          }
        }

        if (imax != i_row) {
            std::swap(pivots[i_row], pivots[imax]);
            std::swap(lu_factorised[i_row], lu_factorised[imax]);
            ++pivots.back();
        }
      }
     
      if (lu_factorised[i_row][i_row] < tolerance) return false; //failure, matrix is degenerate

      for (std::size_t i_row_sweep = i_row + 1; i_row_sweep < lu_factorised.size_row(); ++i_row_sweep) {
          lu_factorised[i_row_sweep][i_row] /= lu_factorised[i_row][i_row];

          for (std::size_t i_column_sweep = i_row + 1; i_column_sweep < lu_factorised.size_column(); ++i_column_sweep)
              lu_factorised[i_row_sweep][i_column_sweep] -= lu_factorised[i_row_sweep][i_row] * lu_factorised[i_row][i_column_sweep];
      }
    }   
    return true;
  }

};

template<std::size_t _size>
using Solver_LUP = Direct_Lower_Upper_Factorisation<Solver_Type::lower_upper_factorisation, _size, true>;

template<std::size_t _size>
using Solver_LU = Direct_Lower_Upper_Factorisation<Solver_Type::lower_upper_factorisation, _size, false> ;

}

#endif //DISA_DIRECT_UPPER_LOWER_FACTORISATION_H
