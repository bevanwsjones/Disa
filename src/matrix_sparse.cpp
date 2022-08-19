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
//  File Name: matrix_sparse.cpp
//  Description: Implements the definitions for out-of-line declarations for member and non-member functions relating
//               to sparse matrices.
// ----------------------------------------------------------------------------------------------------------------------

#include "matrix_sparse.h"

namespace Disa {

//----------------------------------------------------------------------------------------------------------------------
// Matrix Sparse
//----------------------------------------------------------------------------------------------------------------------
// Constructors/Destructors
//----------------------------------------------------------------------------------------------------------------------

/**
 * @details A number of checks are performed to ensure that the parsed initializer lists are consistent. The checks
 * include:
 *
 * 1. The first value in the non-zero offset vector is 0.
 * 2. Making sure the number of non-zeros matches the last offset value in the parsed non-zero vector.
 * 3. The non-zero offsets or sorted in an ascending manner.
 * 4. The column index size matches the value size.
 * 5. For each row of column indexes the indexes are unique and within the parsed column size.
 */
Matrix_Sparse::Matrix_Sparse(std::initializer_list<std::size_t> non_zero, std::initializer_list<std::size_t> index,
                             std::initializer_list<double> value, std::size_t column) : row_non_zero(non_zero),
                                                                                        column_index(index),
                                                                                        entry(value),
                                                                                        column_size(column) {
  ASSERT(row_non_zero.front() == 0, "First value must be zero, but is " + std::to_string(row_non_zero.front()) + ".");
  ASSERT(row_non_zero.back() == column_index.size(), "Number of non-zeros does not match number of column non zeros");
  ASSERT(std::is_sorted(row_non_zero.begin(), row_non_zero.end()), "Inconsistent non-zeros list, but be ascending.");
  ASSERT(column_index.size() == entry.size(),
         "Mis-match in column and entry size, " + std::to_string(column_index.size()) + " vs. " +
         std::to_string(entry.size()));

  std::vector<double> new_value;
  std::vector<std::size_t> org_index;
  FOR(row, row_non_zero.size() - 1) {
    const std::size_t size_non_zero = row_non_zero[row + 1] - row_non_zero[row];
    org_index.resize(size_non_zero);
    new_value.resize(size_non_zero);
    std::iota(org_index.begin(), org_index.end(), 0);
    std::sort(org_index.begin(), org_index.end(), [&](const std::size_t index_0, const std::size_t index_1) {
      return column_index[row_non_zero[row] + index_0] < column_index[row_non_zero[row] + index_1];
    });

    FOR(i_non_zero, size_non_zero) {
      new_value[i_non_zero] = entry[row_non_zero[row] + org_index[i_non_zero]];
      org_index[i_non_zero] = column_index[row_non_zero[row] + org_index[i_non_zero]];
      ASSERT(org_index[i_non_zero] < column_size,
             "Column index, " + std::to_string(org_index[i_non_zero]) + ", in row " + std::to_string(row) +
             " not in range" + range_column() + ".");
    }
    ASSERT(std::adjacent_find(org_index.begin(), org_index.end()) == org_index.end(),
           "Duplicate column index, " + std::to_string(column_size) + "in row " + std::to_string(row) + ".");
    std::swap_ranges(new_value.begin(), new_value.end(),
                     std::next(entry.begin(), static_cast<size_t_signed>(row_non_zero[row])));
    std::swap_ranges(org_index.begin(), org_index.end(),
                     std::next(column_index.begin(), static_cast<size_t_signed>(row_non_zero[row])));
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Helper Classes
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Iterators for Sparse Matrices
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Stand Alone Sparse Matrix Operators
//----------------------------------------------------------------------------------------------------------------------

double& Matrix_Sparse_Row::operator[](const std::size_t& column) {
  auto col_iter = std::lower_bound(offset.begin(), offset.end(), column);

  //  if(col_iter == offset.end()) {
  //    matrix->insert(row_index, column, 0.0);
  //    *this = (*matrix)[row_index];
  //    col_iter = std::lower_bound(offset.begin(), offset.end(), column); // iterator might have been invalidated.
  //  }
  return row_entry[std::distance(offset.begin(), col_iter)];
}

const double& Matrix_Sparse_Row::operator[](const std::size_t& column) const {
  auto col_iter = std::lower_bound(offset.begin(), offset.end(), column);
  ASSERT_DEBUG(col_iter != offset.end(), "does not exist.");
  return row_entry[std::distance(offset.begin(), col_iter)];
}


Iterator_Matrix_Sparse Matrix_Sparse::begin() {
  return {this, 0};
};

Const_Iterator_Matrix_Sparse Matrix_Sparse::begin() const {
  return Const_Iterator_Matrix_Sparse();
};

Const_Iterator_Matrix_Sparse Matrix_Sparse::cbegin() const {
  return Const_Iterator_Matrix_Sparse();
};


}

