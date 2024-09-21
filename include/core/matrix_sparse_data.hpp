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
//  File Name: matrix_sparse.hpp
//  Description: Contains the declaration and some definitions for the basic sparse matrix classes for Disa, at present
//               represented as a CSR matrix.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_MATRIX_SPARSE_DATA_H
#define DISA_MATRIX_SPARSE_DATA_H

#include <span>
#include <tuple>
#include <vector>

#include "scalar.hpp"

namespace Disa {

/**
 * @brief A structure to represent Compressed Sparse Row (CSR) data format.
 * 
 * This structure is used to store sparse matrices in CSR format, it contains only the data and no operations. The data
 * is stored in three standard vectors: row_offset, column_index, and value. The row_offset vector stores the index of
 * the first non-zero element in each row, the column_index vector stores the column index of each non-zero element, and
 * the value vector stores the value of each non-zero element. The columns member variable stores the number of columns
 * in the matrix. 
 * 
 * The class is not intended for use outside of the Matrix_Sparse class which wraps it.
 * 
 * @tparam _value_type The type of the values stored in the matrix.
 * @tparam _index_type The type of the indices used for rows and columns, defaults to std::size_t.
 */
template<typename _value_type, typename _index_type = std::size_t>
struct CSR_Data {

  using index_type = _index_type;
  using value_type = _value_type;
  using iterator = std::tuple<typename std::vector<index_type>::iterator, typename std::vector<index_type>::iterator,
                              typename std::vector<value_type>::iterator>;
  using const_iterator =
  std::tuple<typename std::vector<index_type>::const_iterator, typename std::vector<index_type>::const_iterator,
             typename std::vector<value_type>::const_iterator>;

  std::vector<index_type> row_offset{};   /**< The row offset vector. */
  std::vector<index_type> column_index{}; /**< The column index vector. */
  std::vector<value_type> value{};        /**< The value vector. */
  index_type columns{0};                  /**< The number of columns in the matrix. */
};

/**
 * @brief Computes the row index from a given iterator in a const CSR data structure.
 *
 * @tparam _value_type The type of the values stored in the CSR data structure.
 * @tparam _index_type The type of the indices used in the CSR data structure.
 * @param data A reference to the CSR_Data object containing the matrix data.
 * @param iter_row An iterator pointing to a specific row in the row offset vector.
 * @return The row index corresponding to the given iterator.
 * 
 * @note assumes iterator is in rage begin() - end().
 */
template<typename _value_type, typename _index_type>
_index_type i_row(const CSR_Data<_value_type, _index_type>& data,
                  const typename std::vector<_index_type>::const_iterator& iter_row) noexcept {
  return static_cast<_index_type>(std::distance(data.row_offset.cbegin(), iter_row));
};

/**
 * @brief Computes the row index from a given iterator in a CSR data structure.
 *
 * @tparam _value_type The type of the values stored in the CSR data structure.
 * @tparam _index_type The type of the indices used in the CSR data structure.
 * @param data A reference to the CSR_Data object containing the matrix data.
 * @param iter_row An iterator pointing to a specific row in the row offset vector.
 * @return The row index corresponding to the given iterator.
 * 
 * @note assumes iterator is in rage begin() - end().
 */
template<typename _value_type, typename _index_type>
_index_type i_row(CSR_Data<_value_type, _index_type>& data,
                  const typename std::vector<_index_type>::iterator& iter_row) noexcept {
  return i_row(static_cast<const CSR_Data<_value_type, _index_type>&>(data), iter_row);
};

/**
 * @brief Returns the number of rows in a CSR matrix.
 *
 * This function calculates the number of rows in the given CSR_Data structure. It checks if the row_offset vector is 
 * not empty and returns its size minus one. If the row_offset vector is empty, it returns 0.
 *
 * @tparam _value_type The type of the values stored in the CSR matrix.
 * @tparam _index_type The type of the indices used in the CSR matrix.
 * @param data The CSR_Data structure containing the matrix data.
 * @return The number of rows in the CSR matrix.
 */
template<typename _value_type, typename _index_type>
_index_type size_row(const CSR_Data<_value_type, _index_type>& data) noexcept {
  return !data.row_offset.empty() ? data.row_offset.size() - 1 : 0;
};

/**
 * @brief Returns the number of columns in the given CSR data structure.
 * 
 * @tparam _value_type The type of values stored in the CSR data structure.
 * @tparam _index_type The type of indices used in the CSR data structure.
 * @param data The CSR data structure containing the matrix data.
 * @return _index_type The number of columns in the CSR data structure.
 */
template<typename _value_type, typename _index_type>
_index_type size_column(const CSR_Data<_value_type, _index_type>& data) noexcept {
  return data.columns;
};

/**
 * @brief Returns the number of non-zero elements in the given CSR data structure.
 *
 * @tparam _value_type The type of the values stored in the CSR data structure.
 * @tparam _index_type The type of the indices used in the CSR data structure.
 * @param data The CSR data structure containing the sparse matrix data.
 * @return The number of non-zero elements in the CSR data structure.
 */
template<typename _value_type, typename _index_type>
_index_type size_non_zero(const CSR_Data<_value_type, _index_type>& data) noexcept {
  return data.column_index.size();
};

/**
 * @brief Resizes the CSR data structure to represent a new specified number of rows and columns.
 *
 * This function adjusts the size of the CSR data structure, which resizing the row offsets, column indices, and values.
 * If the new size is smaller than the current size, the function will truncate the data accordingly. Specifically for 
 * column truncation, the function will loop over each row to remove any columns that exceed the new column size.
 *
 * @tparam _value_type The type of the values stored in the CSR data structure.
 * @tparam _index_type The type of the indices used in the CSR data structure.
 * @tparam _arg_index_type The type of the arguments used for resizing, which must be convertible to _index_type.
 * @param data The CSR data structure to be resized.
 * @param row The new number of rows, must be non-negative.
 * @param column The new number of columns, must be non-negative.
 */
template<typename _value_type, typename _index_type, typename _arg_index_type>
std::enable_if<std::is_convertible_v<_arg_index_type, _index_type>, void>::type resize(
CSR_Data<_value_type, _index_type>& data, const _arg_index_type& row, const _arg_index_type& column) {

  ASSERT_DEBUG(row >= 0, "New row size must be positive.");
  ASSERT_DEBUG(column >= 0, "New columns size must be positive.");

  // Set up alias.
  using s_index_type = std::make_signed_t<_index_type>;
  auto& row_offset = data.row_offset;
  auto& column_index = data.column_index;
  auto& value = data.value;

  // resize rows first
  if(row < size_row(data)) {
    column_index.resize(row_offset[row]);
    value.resize(row_offset[row]);
  }
  row_offset.resize(row + 1, row_offset.empty() ? 0 : row_offset.back());

  // resize columns
  // Note: Since we may have to do column erases over multiple rows we do it directly here rather than use erase().
  if(column < size_column(data)) {
    _index_type offset_loss = 0;
    for(_index_type row = 0; row < size_row(data); ++row) {
      const auto& iter_column_end = column_index.begin() + static_cast<s_index_type>(row_offset[row + 1] - offset_loss);
      auto iter_column =
      std::upper_bound(column_index.begin() + static_cast<s_index_type>(row_offset[row]), iter_column_end, column - 1);
      if(iter_column != iter_column_end) {
        const auto& start_distance = std::distance(column_index.begin(), iter_column);
        const auto& end_distance = std::distance(column_index.begin(), iter_column_end);
        column_index.erase(iter_column, iter_column_end);
        value.erase(value.begin() + start_distance, value.begin() + end_distance);
        offset_loss += end_distance - start_distance;
      }
      row_offset[row + 1] -= offset_loss;
    }
  }
  data.columns = column;
}

/**
 * @brief Inserts a value into a CSR data structure.
 *
 * This function inserts a value into the specified position (row, column) of the CSR_Data structure. If the position 
 * already contains a value, the function returns without inserting. If the row or column index is out of bounds, the
 * function resizes the data structure accordingly (much in the same way an insert into a map would add a new key-value
 * pair).
 *
 * @tparam _value_type The type of the values stored in the matrix.
 * @tparam _index_type The type of the indices used in the CSR_Data structure.
 * @tparam _arg_index_type The type of the row and column indices provided as arguments.
 * @param data The CSR_Data structure representing the sparse matrix.
 * @param row The row non-negative index where the value should be inserted.
 * @param column The column non-negative index where the value should be inserted.
 * @param value The value to be inserted into the matrix.
 * @return A pair consisting of an iterator to the inserted position and a boolean indicating
 *         whether the insertion was successful (true) or if the position already contained a value (false).
 */
template<typename _value_type, typename _index_type, typename _arg_index_type>
std::pair<typename CSR_Data<_value_type, _index_type>::iterator, bool> insert(CSR_Data<_value_type, _index_type>& data,
                                                                              const _arg_index_type& row,
                                                                              const _arg_index_type& column,
                                                                              const _value_type& value) {

  // Check sizes
  ASSERT_DEBUG(row >= 0, "Row index must be positive.");
  ASSERT_DEBUG(column >= 0, "Columns index must be positive.");

  // Resize if we need to.
  if(row >= size_row(data)) resize(data, row + 1, static_cast<_arg_index_type>(size_column(data)));
  if(column >= size_column(data)) resize(data, static_cast<_arg_index_type>(size_row(data)), column + 1);

  // Search for new insert
  const auto iter_row_start = data.row_offset.begin() + row;  // tricky, only create after resizing.
  auto [iter_row, iter_column, iter_value] = lower_bound(data, row, column);

  // Entry exists, return no insert.
  if(iter_row == iter_row_start && *iter_column == column)
    return {std::make_tuple(iter_row, iter_column, iter_value), false};

  // Insert new entry.
  for(auto non_zeros = iter_row_start + 1; non_zeros < data.row_offset.end(); ++(*non_zeros++)) {};  // increment and ++
  iter_column = data.column_index.insert(iter_column, column);
  iter_value = data.value.insert(iter_value, value);
  return {std::make_tuple(iter_row_start, iter_column, iter_value), true};
}

/**
 * @brief Finds the lower bound of a specified element position in a CSR data structure.
 *
 * This function searches for the lower bound of a specified element position in a CSR data structure. It returns an 
 * iterator to the first element that is not less than the specified row and column indices, in must the same way as a 
 * map. If no element is found, the function returns an iterator to the end of the 'row', such that insert maybe used to
 * insert into the row. 
 *
 * @tparam _value_type The type of the values stored in the matrix.
 * @tparam _index_type The type of the indices used in the matrix.
 * @tparam _arg_index_type The type of the argument indices.
 * @param data The CSR matrix data.
 * @param row The row non-negative index to search for.
 * @param column The column non-negative index to search for.
 * @return A CSR Data iterator to the row, column, and value of the found element. If not found, returns end iterators.
 * 
 * @warning A simple comparison with end() for each iterator, in the returned tuple iterator, is not sufficient to check 
 *          if the element was not found. The iterators must be checked on a 'per row' basis.
 */
template<typename _value_type, typename _index_type, typename _arg_index_type>
std::enable_if<std::is_convertible_v<_arg_index_type, _index_type>,
               typename CSR_Data<_value_type, _index_type>::const_iterator>::type
lower_bound(const CSR_Data<_value_type, _index_type>& data, const _arg_index_type& row, const _arg_index_type& column) {
  // Check sizes
  ASSERT_DEBUG(row >= 0, "Row index must be positive.");
  ASSERT_DEBUG(column >= 0, "Columns index must be positive.");

  if(row < size_row(data)) {
    const auto& iter_row = data.row_offset.cbegin() + row;
    const auto& iter_start = data.column_index.cbegin() + data.row_offset[row];
    const auto& iter_end = data.column_index.cbegin() + data.row_offset[row + 1];
    const auto& iter_lower = std::lower_bound(iter_start, iter_end, column);
    return std::make_tuple(iter_lower != iter_end ? iter_row : (iter_row + 1), iter_lower,
                           data.value.cbegin() + std::distance(data.column_index.cbegin(), iter_lower));
  } else return std::make_tuple(data.row_offset.cend(), data.column_index.cend(), data.value.cend());
}

/**
 * @brief Finds the lower bound of a specified element position in a CSR data structure.
 *
 * This function searches for the lower bound of a specified element position in a CSR data structure. It returns an 
 * iterator to the first element that is not less than the specified row and column indices, in must the same way as a 
 * map. If no element is found, the function returns an iterator to the end of the 'row', such that insert maybe used to
 * insert into the row. 
 *
 * @tparam _value_type The type of the values stored in the matrix.
 * @tparam _index_type The type of the indices used in the matrix.
 * @tparam _arg_index_type The type of the argument indices.
 * @param data The CSR matrix data.
 * @param row The row non-negative index to search for.
 * @param column The column non-negative index to search for.
 * @return A CSR Data iterator to the row, column, and value of the found element. If not found, returns end iterators.
 * 
 * @warning A simple comparison with end() for each iterator, in the returned tuple iterator, is not sufficient to check 
 *          if the element was not found. The iterators must be checked on a 'per row' basis.
 */
template<typename _value_type, typename _index_type, typename _arg_index_type>
std::enable_if<std::is_convertible_v<_arg_index_type, _index_type>,
               typename CSR_Data<_value_type, _index_type>::iterator>::type
lower_bound(CSR_Data<_value_type, _index_type>& data, const _arg_index_type& row, const _arg_index_type& column) {

  // Check sizes
  ASSERT_DEBUG(row >= 0, "Row index must be positive.");
  ASSERT_DEBUG(column >= 0, "Columns index must be positive.");

  const auto& const_iter = lower_bound(static_cast<const CSR_Data<_value_type, _index_type>&>(data), row, column);
  return std::make_tuple(data.row_offset.begin() + std::distance(data.row_offset.cbegin(), std::get<0>(const_iter)),
                         data.column_index.begin() + std::distance(data.column_index.cbegin(), std::get<1>(const_iter)),
                         data.value.begin() + std::distance(data.value.cbegin(), std::get<2>(const_iter)));
}

}  // namespace Disa

#endif  // DISA_MATRIX_SPARSE_DATA_H
