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
// File Name: matrix_sparse.cpp
// Description: Contains the definition of methods from the sparse matrix classes for Disa.
// ---------------------------------------------------------------------------------------------------------------------

#include <tuple>
#include "matrix_sparse.hpp"

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse
// ---------------------------------------------------------------------------------------------------------------------
// Constructors/Destructors
// ---------------------------------------------------------------------------------------------------------------------

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
                             std::initializer_list<double> value, std::size_t column)
  : row_non_zero(non_zero), column_index(index), element_value(value), column_size(column) {
  ASSERT(row_non_zero.front() == 0, "First value must be zero, but is " + std::to_string(row_non_zero.front()) + ".");
  ASSERT(row_non_zero.back() == column_index.size(), "Number of non-zeros does not match number of column non zeros");
  ASSERT(std::is_sorted(row_non_zero.begin(), row_non_zero.end()), "Inconsistent non-zeros list, but be ascending.");
  ASSERT(column_index.size() == value.size(),
         "Mis-match in column and value size, " + std::to_string(column_index.size()) + " vs. " +
         std::to_string(value.size()));

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
      new_value[i_non_zero] = element_value[row_non_zero[row] + org_index[i_non_zero]];
      org_index[i_non_zero] = column_index[row_non_zero[row] + org_index[i_non_zero]];
      ASSERT(org_index[i_non_zero] < column_size,
             "Column index, " + std::to_string(org_index[i_non_zero]) + ", in row " + std::to_string(row) +
             " not in range" + range_column() + ".");
    }
    ASSERT(std::adjacent_find(org_index.begin(), org_index.end()) == org_index.end(),
           "Duplicate column index, " + std::to_string(column_size) + "in row " + std::to_string(row) + ".");
    std::swap_ranges(new_value.begin(), new_value.end(),
                     std::next(element_value.begin(), static_cast<s_size_t>(row_non_zero[row])));
    std::swap_ranges(org_index.begin(), org_index.end(),
                     std::next(column_index.begin(), static_cast<s_size_t>(row_non_zero[row])));
  }
}

// ---------------------------------------------------------------------------------------------------------------------
// Element Access
// ---------------------------------------------------------------------------------------------------------------------

/**
 * \details Using lower bound, finds and returns the element value at the requested row and column index. If it does not
 * exist an error is thrown.
 */
[[nodiscard]] double& Matrix_Sparse::at(const std::size_t& i_row, const std::size_t& i_column) {
  ASSERT(i_row < size_row(), "Row index " + std::to_string(i_row) + " not in range " + range_row() + ".");
  ASSERT(i_column < size_column(),
         "Column index " + std::to_string(i_column) + " not in range " + range_column() + ".");
  auto iter = lower_bound(i_row, i_column);
  ASSERT(i_column == iter.i_column(), "Row, column index " + std::to_string(i_row) + ", " + std::to_string(i_column)
                                      + "not in range " + range_column() + ".");

  return *iter;
};

/**
 * \details Using lower bound, finds and returns the element value, as a const, at the requested row and column index.
 * If it does not exist an error is thrown.
 */
[[nodiscard]] const double& Matrix_Sparse::at(const std::size_t& i_row, const std::size_t& i_column) const {
  ASSERT(i_row < size_row(), "Row index " + std::to_string(i_row) + " not in range " + range_row() + ".");
  ASSERT(i_column < size_column(),
         "Column index " + std::to_string(i_column) + " not in range " + range_column() + ".");
  const auto iter = lower_bound(i_row, i_column);
  ASSERT(i_column == iter.i_column(), "Row, column index " + std::to_string(i_row) + ", " + std::to_string(i_column)
                                      + "not in range " + range_column() + ".");
  return *iter;
};

/**
 * \details Constructs and returns a sparse matrix row object for the given row. In debug range checking is performed.
 */
Matrix_Sparse_Row<Matrix_Sparse> Matrix_Sparse::operator[](const std::size_t& i_row) {
  ASSERT_DEBUG(i_row < size_row(), "Row " + std::to_string(i_row) + " not in range " + range_row() + ".");
  return {i_row, this};
}

/**
 * \details Constructs and returns a sparse matrix row object for the given row. In debug range checking is performed.
 */
Matrix_Sparse_Row<const Matrix_Sparse> Matrix_Sparse::operator[](const std::size_t& i_row) const {
  ASSERT_DEBUG(i_row < size_row(), "Row " + std::to_string(i_row) + " not in range " + range_row() + ".");
  return {i_row, this};
}

// ---------------------------------------------------------------------------------------------------------------------
// Element Access
// ---------------------------------------------------------------------------------------------------------------------

/**
 * \details Constructs and returns a sparse matrix row iterator for the first row.
 */
Matrix_Sparse::iterator Matrix_Sparse::begin() noexcept {
  return {0, this};
}

/**
 * \details Constructs and returns a constant sparse matrix row iterator for the first row.
 */
Matrix_Sparse::const_iterator Matrix_Sparse::begin() const noexcept {
  return {0, this};
}

/**
 * \details Constructs and returns a constant sparse matrix row iterator for the first row.
 */
Matrix_Sparse::const_iterator Matrix_Sparse::cbegin() const noexcept {
  return begin();
}

/**
 * \details Constructs and returns a sparse matrix row iterator for 'a row' past the last row.
 */
Matrix_Sparse::iterator Matrix_Sparse::end() noexcept {
  return {size_row(), this};
}

/**
 * \details Constructs and returns a constant sparse matrix row iterator for 'a row' past the last row.
 */
Matrix_Sparse::const_iterator Matrix_Sparse::end() const noexcept {
  return {size_row(), this};
}

/**
 * \details Constructs and returns a constant sparse matrix row iterator for 'a row' past the last row.
 */
Matrix_Sparse::const_iterator Matrix_Sparse::cend() const noexcept {
  return end();
}

// ---------------------------------------------------------------------------------------------------------------------
// Modifiers
// ---------------------------------------------------------------------------------------------------------------------

/**
 * \details If the value to be inserted has a row or column index greater than the current size of the matrix, the
 * matrix is resized to accommodate the insertion. Lower bound is then used to find the iterator to the position where
 * the new value is to be inserted. If there is a value already present at the position the function 'aborts' reporting
 * this as false in the second value of the pair. Otherwise the row non-zero vector us updated indicating the new
 * non-zero in the row by propagating the offset incrementation to the remaining parts of the vector. The column index
 * and element value vectors are appropriately updated.
 */
std::pair<Matrix_Sparse::iterator_element, bool>
Matrix_Sparse::insert(const std::size_t& i_row, const std::size_t& i_column, const double& value) {

  // Resize if we need to.
  if(i_row >= size_row()) resize(i_row + 1, column_size);
  if(i_column >= size_column()) resize(size_row(), i_column + 1);
  const auto iter_insert = lower_bound(i_row, i_column);
  if(!(iter_insert == (*this)[i_row].end() || iter_insert.i_column() != i_column)) return {iter_insert, false};

  for(auto non_zeros = std::next(row_non_zero.begin(), static_cast<s_size_t>(iter_insert.i_row() + 1));
      non_zeros < row_non_zero.end(); ++(*non_zeros++));
  const auto distance = std::distance(&*element_value.cbegin(), &*iter_insert);
  return {{this, i_row, &*column_index.insert(column_index.begin() + distance, i_column),
           &*element_value.insert(element_value.begin() + distance, value)}, true};
}

/**
 * \details First the function will attempt to insert with the parsed value. If this fails the value will be assigned to
 * the existing matrix element.
 */
std::pair<Matrix_Sparse::iterator_element, bool>
Matrix_Sparse::insert_or_assign(const std::size_t& i_row, const std::size_t& i_column, const double& value) {
  auto iter_inserted = insert(i_row, i_column, value);
  if(!iter_inserted.second) *iter_inserted.first = value;
  return iter_inserted;
}

/**
 * \details Erases a element from the matrix, if the element does not exist and error is thrown in debug. The number of
 * row non-zeros at the row is decremented for the 'next row' and propagated to the rest of the row non-zero vector. The
 * value and column index vectors have the associated matrix entry data erased (reducing their size by 1).
 */
Matrix_Sparse::iterator_element
Matrix_Sparse::erase(const Iterator_Matrix_Sparse_Element<Matrix_Sparse>& iter_element) {
  ASSERT_DEBUG(iter_element != end()->end() && iter_element != (*this)[iter_element.i_row()].end() &&
               contains(iter_element.i_row(), iter_element.i_column()),
               "Entry [" + row_column(iter_element.i_row(), iter_element.i_column()) + "] does not exist.");
  for(auto non_zeros = std::next(row_non_zero.begin(), static_cast<s_size_t>(iter_element.i_row() + 1));
      non_zeros < row_non_zero.end(); --(*non_zeros++));
  const auto distance = std::distance(&*element_value.cbegin(), &*iter_element);
  return {this, iter_element.i_row(), &*(column_index.erase(column_index.begin() + distance)),
          &*(element_value.erase(element_value.begin() + distance))};
};

/**
 * \details For general increase in size the row non-zero vector is resized, with the new entries containing the current
 * row non-zero back() value. For column size increase the column_size member is updated. If the rows are to be reduced,
 * however, all the column indexes and element values greater than the row non-zero value at the new row size are
 * erased. Finally, a column reduction requires looping over all rows to determine which column indexes and element
 * values have 'fallen away'. These entries are removed from the vector and the total update of non-zeros is then
 * propagated through the remainder of the row non-zero vector (in an efficient way).
 */
void Matrix_Sparse::resize(const std::size_t& row, const std::size_t& column) {

  // resize rows first
  if(row < size_row()) {
    column_index.resize(row_non_zero[row]);
    element_value.resize(row_non_zero[row]);
  }
  row_non_zero.resize(row + 1, row_non_zero.empty() ? 0 : row_non_zero.back());

  // resize columns
  // Note: Since we may have to do column erases over multiple rows we do it directly here rather than use erase().
  if(column < size_column()) {
    std::size_t offset_loss = 0;
    FOR(i_row, size_row()) {
      const auto& iter_column_end = column_index.begin() + static_cast<s_size_t>(row_non_zero[i_row + 1] - offset_loss);
      auto iter_column = std::upper_bound(column_index.begin() + static_cast<s_size_t>(row_non_zero[i_row]),
                                          iter_column_end, column - 1);
      if(iter_column != iter_column_end) {
        const s_size_t& start_distance = std::distance(column_index.begin(), iter_column);
        const s_size_t& end_distance = std::distance(column_index.begin(), iter_column_end);
        column_index.erase(iter_column, iter_column_end);
        element_value.erase(element_value.begin() + start_distance, element_value.begin() + end_distance);
        offset_loss += end_distance - start_distance;
      }
      row_non_zero[i_row + 1] -= offset_loss;
    }
  }
  column_size = column;
}

// ---------------------------------------------------------------------------------------------------------------------
// Lookup
// ---------------------------------------------------------------------------------------------------------------------

/**
 * \details If the parsed row is in range, the column index is searched using std::find in the given sparse matrix row.
 * An iterator to the found matrix entry is constructed and returned. If the entry is not contained in the sparse
 * matrix, an end() or *(begin + i_row).end() iterator is constructed.
 */
Matrix_Sparse::iterator_element Matrix_Sparse::find(const std::size_t& i_row, const std::size_t& i_column) {
  if(i_row < size_row()) {
    const auto& iter_start = column_index.begin() + static_cast<s_size_t>(row_non_zero[i_row]);
    const auto& iter_end = column_index.begin() + static_cast<s_size_t>(row_non_zero[i_row + 1]);
    auto iter_find = std::find(iter_start, iter_end, i_column);
    return {this, i_row, &*iter_find, &*(element_value.begin() + std::distance(column_index.begin(), iter_find))};
  } else return (*end()).end();
}

/**
 * \details If the parsed row is in range, the column index is searched using std::find in the given sparse matrix row.
 * A const iterator to the found matrix entry is constructed and returned. If the entry is not contained in the sparse
 * matrix, an end() or *(begin + i_row).end() iterator is constructed.
 */
Matrix_Sparse::const_iterator_element Matrix_Sparse::find(const std::size_t& i_row, const std::size_t& i_column) const {
  if(i_row < size_row()) {
    const auto& iter_start = column_index.begin() + static_cast<s_size_t>(row_non_zero[i_row]);
    const auto& iter_end = column_index.begin() + static_cast<s_size_t>(row_non_zero[i_row + 1]);
    auto iter_find = std::find(iter_start, iter_end, i_column);
    return {this, i_row, &*iter_find, &*(element_value.begin() + std::distance(column_index.begin(), iter_find))};
  } else return (*end()).end();
}

/**
 * \details Calls find, if the iterator is referencable true is returned, else false.
 */
bool Matrix_Sparse::contains(const std::size_t& i_row, const std::size_t& i_column) const {
  const auto iter_element = find(i_row, i_column);
  return iter_element.i_row() == i_row && iter_element != (*(begin() + static_cast<s_size_t>(i_row))).end();
}

/**
 * \details If the parsed row is in range, the column index is searched using std::lower_bound in the given sparse
 * matrix row. An iterator is then constructed from the first column index not less than the parsed column index. If the
 * row is greater than the number of rows an end() iterator is constructed.
 */
Matrix_Sparse::iterator_element Matrix_Sparse::lower_bound(const std::size_t& i_row, const std::size_t& i_column) {
  if(i_row < size_row()) {
    const auto& iter_start = column_index.begin() + static_cast<s_size_t>(row_non_zero[i_row]);
    const auto& iter_end = column_index.begin() + static_cast<s_size_t>(row_non_zero[i_row + 1]);
    const auto& iter_lower = std::lower_bound(iter_start, iter_end, i_column);
    return {this, i_row, &*iter_lower, &*(element_value.begin() + std::distance(column_index.begin(), iter_lower))};
  } else return (*end()).end();
}

/**
 * \details If the parsed row is in range, the column index is searched using std::lower_bound in the given sparse
 * matrix row. A const iterator is then constructed from the first column index not less than the parsed column index.
 * If the row is greater than the number of rows an end() iterator is constructed.
 */
Matrix_Sparse::const_iterator_element
Matrix_Sparse::lower_bound(const std::size_t& i_row, const std::size_t& i_column) const {
  if(i_row < size_row()) {
    const auto& iter_start = column_index.begin() + static_cast<s_size_t>(row_non_zero[i_row]);
    const auto& iter_end = column_index.begin() + static_cast<s_size_t>(row_non_zero[i_row + 1]);
    const auto& iter_lower = std::lower_bound(iter_start, iter_end, i_column);
    return {this, i_row, &*iter_lower, &*(element_value.begin() + std::distance(column_index.begin(), iter_lower))};
  } else return (*end()).end();
}

// ---------------------------------------------------------------------------------------------------------------------
// Mathematical Assignment Operators
// ---------------------------------------------------------------------------------------------------------------------

/**
 * \details A very basic SpGEMM algorithm, a simple loop through the rows and columns of each matrix looking for
 * non-zero entries to multiply and construct and return the resulting matrix. In debug the matrix sizes are checked for
 * compatibility.
 */
Matrix_Sparse& Matrix_Sparse::operator*=(const Matrix_Sparse& other) {
  ASSERT_DEBUG(column_size == other.size_row(),
               "Incompatible other dimensions, " + std::to_string(size_row()) + "," + std::to_string(size_column()) +
               " vs. " + std::to_string(other.size_row()) + "," + std::to_string(other.size_column()) + ".");
  Matrix_Sparse copy;
  copy.reserve(size_row(), size_non_zero() + other.size_non_zero());
  copy.resize(size_row(), other.size_column());
  std::swap(*this, copy);
  FOR_EACH_REF(iter_row, copy) {
    FOR_ITER_REF(iter_element, iter_row) {
      FOR_ITER(iter_element_other, *(other.begin() + iter_element.i_column())) {
        const std::size_t& i_row = iter_element.i_row();
        const std::size_t& i_column = iter_element_other.i_column();
        const Scalar& value = (*iter_element)*(*iter_element_other);
        if(!contains(i_row, i_column)) insert(i_row, i_column, value);
        else (*this)[i_row][i_column] += value;
      }
    }
  }
  shrink_to_fit();
  return *this;
}

// ---------------------------------------------------------------------------------------------------------------------
// Private Member Functions
// ---------------------------------------------------------------------------------------------------------------------

/**
 * \details Consolidation of both addition and subtraction operations for sparse matrix arithmetic. Looping over both
 * matrices row and columns, entries which do not exist in this matrix are inserted (either with addition or
 * subtraction) and entries which do exist are modified by either addition or subtracting to/from the existing value. In
 * debug the matrix sizes are checked for compatibility.
 */
template<bool _is_add>
Matrix_Sparse& Matrix_Sparse::matrix_arithmetic(const Matrix_Sparse& other) {
  ASSERT_DEBUG(size_row() == other.size_row() && size_column() == other.size_column(),
               "Incompatible other dimensions, " + std::to_string(size_row()) + "," + std::to_string(size_column()) +
               " vs. " + std::to_string(other.size_row()) + "," + std::to_string(other.size_column()) + ".");

  bool insert_state;
  iterator iter_this = begin();
  const_iterator iter_other = other.begin();
  while(iter_other != other.end()) {
    auto iter_element_this = iter_this->begin();
    const_iterator_element iter_element_other = (*iter_other).begin();
    while(iter_element_other != (*iter_other).end())
    {
      if(iter_element_this == iter_this->end() || iter_element_this.i_column() > iter_element_other.i_column()) {
        std::tie(iter_element_this, insert_state) = insert(iter_element_other.i_row(), iter_element_other.i_column(),
                                                           _is_add ? (*iter_element_other) : -(*iter_element_other));
        ++iter_element_this;
        ++iter_element_other;
      }
      else if(iter_element_this.i_column() < iter_element_other.i_column()) {
        ++iter_element_this;
      }
      else {
        *iter_element_this += _is_add ? (*iter_element_other) : -(*iter_element_other);
        ++iter_element_this;
        ++iter_element_other;
      }
    }
    ++iter_this;
    ++iter_other;
  }
  return *this;
};
template Matrix_Sparse& Matrix_Sparse::matrix_arithmetic<true>(const Matrix_Sparse&);
template Matrix_Sparse& Matrix_Sparse::matrix_arithmetic<false>(const Matrix_Sparse&);

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Row Helper
// ---------------------------------------------------------------------------------------------------------------------
// Iterators
// ---------------------------------------------------------------------------------------------------------------------

/**
 * \details Construct a new element iterator using the current row index.
 */
template<typename _matrix_type>
typename Matrix_Sparse_Row<_matrix_type>::iterator Matrix_Sparse_Row<_matrix_type>::begin() {
  const std::size_t& offset = matrix->row_non_zero[row_index];
  return {matrix, row_index, &matrix->column_index[offset], &matrix->element_value[offset]};
}

/**
 * \details Construct a new element iterator, to one past the last column element, using the current row index.
 */
template<typename _matrix_type>
typename Matrix_Sparse_Row<_matrix_type>::iterator Matrix_Sparse_Row<_matrix_type>::end() {
  const std::size_t& offset = matrix->row_non_zero[row_index + 1];
  return {matrix, row_index, &matrix->column_index[offset], &matrix->element_value[offset]};
}

/**
 * \details Construct a new constant element iterator using the current row index.
 */
template<typename _matrix_type>
typename Matrix_Sparse_Row<_matrix_type>::const_iterator Matrix_Sparse_Row<_matrix_type>::begin() const {
  const std::size_t& offset = matrix->row_non_zero[row_index];
  return {matrix, row_index, &matrix->column_index[offset], &matrix->element_value[offset]};
}

/**
 * \details Construct a constant new element iterator, to one past the last column element, using the current row index.
 */
template<typename _matrix_type>
typename Matrix_Sparse_Row<_matrix_type>::const_iterator Matrix_Sparse_Row<_matrix_type>::end() const {
  const std::size_t& offset = matrix->row_non_zero[row_index + 1];
  return {matrix, row_index, &matrix->column_index[offset], &matrix->element_value[offset]};
}

/**
 * \details Construct a new constant element iterator using the current row index.
 */
template<typename _matrix_type>
typename Matrix_Sparse_Row<_matrix_type>::const_iterator Matrix_Sparse_Row<_matrix_type>::cbegin() const {
  return begin();
}

/**
 * \details Construct a new constant element iterator, to one past the last column element, using the current row index.
 */
template<typename _matrix_type>
typename Matrix_Sparse_Row<_matrix_type>::const_iterator Matrix_Sparse_Row<_matrix_type>::cend() const {
  return end();
}

// Compile for const and non-const sparse matrices.
template class Matrix_Sparse_Row<Matrix_Sparse>;
template class Matrix_Sparse_Row<const Matrix_Sparse>;
template struct Iterator_Matrix_Sparse_Row<Matrix_Sparse>;
template struct Iterator_Matrix_Sparse_Row<const Matrix_Sparse>;
template struct Iterator_Matrix_Sparse_Element<Matrix_Sparse>;
template struct Iterator_Matrix_Sparse_Element<const Matrix_Sparse>;

}
