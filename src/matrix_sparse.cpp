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

  Iterator_Matrix_Sparse_Row iter = this->begin();

  iter->begin();
}

//--------------------------------------------------------------------------------------------------------------------
// Element Access
//--------------------------------------------------------------------------------------------------------------------

constexpr Matrix_Sparse_Row Matrix_Sparse::operator[](const std::size_t& i_row) {
  ASSERT_DEBUG(i_row < size_row(), "Row " + std::to_string(i_row) + " not in range " + range_row() + ".");
  return {i_row, this};
}

constexpr Matrix_Sparse_Row_Const Matrix_Sparse::operator[](const std::size_t& i_row) const {
  ASSERT_DEBUG(i_row < size_row(), "Row " + std::to_string(i_row) + " not in range " + range_row() + ".");
  return {i_row, this};
}

//--------------------------------------------------------------------------------------------------------------------
// Element Access
//--------------------------------------------------------------------------------------------------------------------

constexpr Matrix_Sparse::iterator Matrix_Sparse::begin() noexcept {
  return {0, this};
}

constexpr Matrix_Sparse::const_iterator Matrix_Sparse::begin() const noexcept {
  return {0, this};
}

constexpr Matrix_Sparse::const_iterator Matrix_Sparse::cbegin() const noexcept {
  return {0, this};
}

constexpr Matrix_Sparse::iterator Matrix_Sparse::end() noexcept {
  return {size_row() - 1, this};
}

constexpr Matrix_Sparse::const_iterator Matrix_Sparse::end() const noexcept {
  return {size_row() - 1, this};
}

constexpr Matrix_Sparse::const_iterator Matrix_Sparse::cend() const noexcept {
  return {size_row() - 1, this};
}

//--------------------------------------------------------------------------------------------------------------------
// Modifiers
//--------------------------------------------------------------------------------------------------------------------

void Matrix_Sparse::clear() noexcept {
  row_non_zero.clear();
  column_index.clear();
  element_value.clear();
  column_size = 0;
};

std::pair<Matrix_Sparse::iterator_element, bool>
Matrix_Sparse::insert(const std::size_t& i_row, const std::size_t& i_column, const double& value) {

  ASSERT_DEBUG(i_row < size_row(), "Row " + std::to_string(i_row) + " not in range " + range_row() + ".");
  ASSERT_DEBUG(i_column < size_column(),
               "Column " + std::to_string(i_column) + " not in range " + range_column() + ".");
  auto iter_insert = find(i_row, i_column);
  return (iter_insert != end()->end() || iter_insert != (begin() + static_cast<s_size_t>(i_row))->end())
         ? std::make_pair(Matrix_Sparse::iterator_element(iter_insert), false) : insert(iter_insert, i_column, value);
}

std::pair<Matrix_Sparse::iterator_element, bool>
Matrix_Sparse::insert(const Matrix_Sparse::iterator_element& iter_insert, const std::size_t& i_column,
                      const double& value) {
  ASSERT_DEBUG(i_column != iter_insert.i_column(), "Cannot insert into the same column.");
  for(auto non_zeros = std::next(row_non_zero.begin(), static_cast<s_size_t>(iter_insert.i_row() + 1));
      non_zeros < row_non_zero.end(); ++(*non_zeros++));
  const auto distance = std::distance(&*element_value.cbegin(), &*iter_insert);
  auto iter_entry = element_value.insert(element_value.begin() + distance, value);
  auto iter_column = column_index.insert(column_index.begin() + distance, i_column);
  return {{this, iter_insert.i_row(), &*iter_column, &*iter_entry}, true};
};

std::pair<Matrix_Sparse::iterator_element, bool>
Matrix_Sparse::insert_or_assign(const std::size_t& i_row, const std::size_t& i_column, const double& value) {

  ASSERT_DEBUG(i_row < size_row(), "Row " + std::to_string(i_row) + " not in range " + range_row() + ".");
  ASSERT_DEBUG(i_column < size_column(),
               "Column " + std::to_string(i_column) + " not in range " + range_column() + ".");

  auto iter_inserted = insert(i_row, i_column, value);
  if(!iter_inserted.second) *iter_inserted.first = value;
  return iter_inserted;
}

Matrix_Sparse::iterator_element Matrix_Sparse::erase(const Iterator_Matrix_Sparse_Element& iter_element) {

  ASSERT_DEBUG(contains(iter_element.i_row(), iter_element.i_column()),
               "Entry [" + row_column(iter_element.i_row(), iter_element.i_column()) + "] does not exist.");

  for(auto non_zeros = std::next(row_non_zero.begin(), static_cast<s_size_t>(iter_element.i_row() + 1));
      non_zeros < row_non_zero.end(); --(*non_zeros++));
  const auto distance = std::distance(&*element_value.cbegin(), &*iter_element);
  return {this, iter_element.i_row(), &*(column_index.erase(column_index.begin() + distance)),
          &*(element_value.erase(element_value.begin() + distance))};
};

constexpr void Matrix_Sparse::resize(const std::size_t& row, const std::size_t& column) noexcept {
  row_non_zero.resize(row + 1, row_non_zero.empty() ? 0 : row_non_zero.back());
  column_size = column;
};

constexpr void Matrix_Sparse::swap(Matrix_Sparse& other) noexcept {
  std::swap(row_non_zero, other.row_non_zero);
  std::swap(column_index, other.column_index);
  std::swap(element_value, other.element_value);
  std::swap(column_size, other.column_size);
}

//--------------------------------------------------------------------------------------------------------------------
// Lookup
//--------------------------------------------------------------------------------------------------------------------

Matrix_Sparse::iterator_element Matrix_Sparse::find(const std::size_t& i_row, const std::size_t& i_column) {
  if(i_row >= size_row()) return {this, size_row(), &*column_index.end(), &*element_value.end()};
  else {
    auto iter_row = begin() + static_cast<s_size_t>(i_row);
    return std::lower_bound(iter_row->begin(), iter_row->end(), i_column);
  }
}

Matrix_Sparse::const_iterator_element Matrix_Sparse::find(const std::size_t& i_row, const std::size_t& i_column) const {
  if(i_row >= size_row()) return {this, size_row(), &*column_index.end(), &*element_value.end()};
  else {
    auto iter_row = begin() + static_cast<s_size_t>(i_row);
    return std::lower_bound(iter_row->begin(), iter_row->end(), i_column);
  }
}

bool Matrix_Sparse::contains(const std::size_t& i_row, const std::size_t& i_column) const {
  auto iter_column_insert = find(i_row, i_column);
  return iter_column_insert != end()->end() || iter_column_insert != (begin() + static_cast<s_size_t>(i_row))->end();
}

//----------------------------------------------------------------------------------------------------------------------
// Helper Classes for Sparse Matrices
//----------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Row Non-Const Helper
//----------------------------------------------------------------------------------------------------------------------

Matrix_Sparse_Row::iterator Matrix_Sparse_Row::begin() {
  const std::size_t& offset = matrix->row_non_zero[row_index];
  return {matrix, row_index, &matrix->column_index[offset], &matrix->element_value[offset]};
}

Matrix_Sparse_Row::iterator Matrix_Sparse_Row::end() {
  const std::size_t& offset = matrix->row_non_zero[row_index + 1];
  return {matrix, row_index, &matrix->column_index[offset], &matrix->element_value[offset]};
}

Matrix_Sparse_Row::const_iterator Matrix_Sparse_Row::begin() const{
  const std::size_t& offset = matrix->row_non_zero[row_index];
  return {matrix, row_index, &matrix->column_index[offset], &matrix->element_value[offset]};
}

Matrix_Sparse_Row::const_iterator Matrix_Sparse_Row::end() const{
  const std::size_t& offset = matrix->row_non_zero[row_index + 1];
  return {matrix, row_index, &matrix->column_index[offset], &matrix->element_value[offset]};
}

double& Matrix_Sparse_Row::operator[](const std::size_t& i_column) {
  auto iter_element = matrix->find(row_index, i_column);
  if(iter_element == end()) iter_element = matrix->insert(iter_element, i_column, 0.0).first;
  return *iter_element;
}

const double& Matrix_Sparse_Row::operator[](const std::size_t& i_column) const {
  ASSERT_DEBUG(matrix->contains(row_index, i_column),
               "Trying to access a zero element at [" + matrix->row_column(row_index, i_column) + "].");
  return *matrix->find(row_index, i_column);
}

//----------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Row Const Helper
//----------------------------------------------------------------------------------------------------------------------

Matrix_Sparse_Row_Const::iterator Matrix_Sparse_Row_Const::begin() const {
  const std::size_t& offset = matrix->row_non_zero[row_index];
  return {matrix, row_index, &matrix->column_index[offset], &matrix->element_value[offset]};

}

Matrix_Sparse_Row_Const::iterator Matrix_Sparse_Row_Const::end() const {
  const std::size_t& offset = matrix->row_non_zero[row_index + 1];
  return {matrix, row_index, &matrix->column_index[offset], &matrix->element_value[offset]};
}

const double& Matrix_Sparse_Row_Const::operator[](const std::size_t& i_column) const {
  ASSERT_DEBUG(matrix->contains(row_index, i_column),
               "Trying to access a zero element at [" + matrix->row_column(row_index, i_column) + "].");
  return *matrix->find(row_index, i_column);
}

//----------------------------------------------------------------------------------------------------------------------
// Stand Alone Sparse Matrix Operators
//----------------------------------------------------------------------------------------------------------------------

}
