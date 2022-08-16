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
//  File Name: matrix_sparse.h
//  Description: todo
// ----------------------------------------------------------------------------------------------------------------------
//
#ifndef DISA_MATRIX_SPARSE_H
#define DISA_MATRIX_SPARSE_H

#include "macros.h"

#include <algorithm>
#include <iterator>
#include <list>
#include <numeric>
#include <span>
#include <vector>

namespace Disa {

class Matrix_Sparse;

class Matrix_Sparse_Row {

public:
  Matrix_Sparse_Row(std::size_t row, std::size_t column, std::span<std::size_t> col_offset, std::span<double> entry,
                    Matrix_Sparse* sparse_matrix)
    : row_index(row), column_size(column), row_col_offset(col_offset), row_entry(entry), matrix(sparse_matrix) {};

  double& operator[](const std::size_t& column);

private:

  std::span<std::size_t> row_col_offset;
  std::span<double> row_entry;
  Matrix_Sparse* matrix;
  std::size_t column_size;
  std::size_t row_index;
};

/**
 * @class Matrix_Sparse
 * @brief todo
 */
class Matrix_Sparse {

public:

  //--------------------------------------------------------------------------------------------------------------------
  // Constructors/Destructors
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Default constructor.
   */
  Matrix_Sparse() = default;

  /**
   * @brief Constructs a sparse matrix with a set number of rows and columns.
   * @param[in] row Number of rows to construct.
   * @param[in] column Number of columns to construct.
   */
  Matrix_Sparse(std::size_t row, std::size_t column) : row_non_zero(row + 1, 0), column_size(column) {};

  Matrix_Sparse(std::initializer_list<std::size_t> non_zero, std::initializer_list<std::size_t> index,
                std::initializer_list<double> value, std::size_t column)
    : row_non_zero(non_zero), column_index(index), entry(value), column_size(column) {
    ASSERT(row_non_zero.back() == column_index.size(), "Number of non-zeros does not match number of column non zeros");
    ASSERT(column_index.size() == entry.size(),
           "Mis-match in column and entry size, " + std::to_string(column_index.size()) + " vs. " +
           std::to_string(entry.size()));
    ASSERT(row_non_zero.front() == 0,
           "First non-zero value must be zero, but is " + std::to_string(row_non_zero.front()) + ".");
    ASSERT(std::is_sorted(row_non_zero.begin(), row_non_zero.end()),
           "Number of non-zeros does not match number of column non zeros");

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
               "Row " + std::to_string(row) + " contains a column index of " + std::to_string(column_size) +
               " which is not in range [" + std::to_string(row_non_zero.size()) + ", " + std::to_string(column_size) +
               ").");
      }
      ASSERT(std::adjacent_find(org_index.begin(), org_index.end()) == org_index.end(),
             "Row " + std::to_string(row) + " contains a duplicate column index of " + std::to_string(column_size) +
             ".");
      std::swap_ranges(new_value.begin(), new_value.end(),
                       std::next(entry.begin(), static_cast<size_t_signed>(row_non_zero[row])));
      std::swap_ranges(org_index.begin(), org_index.end(),
                       std::next(column_index.begin(), static_cast<size_t_signed>(row_non_zero[row])));
    }
  };

  /**
   * @brief Default destructor.
   */
  ~Matrix_Sparse() = default;

  //--------------------------------------------------------------------------------------------------------------------
  // Element Access
  //--------------------------------------------------------------------------------------------------------------------

  Matrix_Sparse_Row operator[](const std::size_t row) {
    const auto [iter_col_offset, iter_entry] = begin_column(row);
    const std::size_t row_size = row_non_zero[row + 1] - row_non_zero[row];
    return {row, column_size, std::span(iter_col_offset, row_size), std::span(iter_entry, row_size), this};
  };

  //--------------------------------------------------------------------------------------------------------------------
  // Iterators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Returns a pair of iterators to the beginning of the column index and entries for a row.
   * @param[in] row The row index to the for which the iterators are desired.
   * @return [iterator_column_index, iterator_entry_index]
   */
  [[nodiscard]] std::pair<std::vector<std::size_t>::iterator, std::vector<double>::iterator>
  begin_column(const std::size_t row) {
    ASSERT_DEBUG(row < size_row(), "problems todo");
    return std::make_pair(std::next(column_index.begin(), static_cast<size_t_signed>(row_non_zero[row])),
                          std::next(entry.begin(), static_cast<size_t_signed>(row_non_zero[row])));
  }

  /**
   * @brief Returns a pair of iterators to the end of the column index and entries for a row.
   * @param[in] row The row index to the for which the iterators are desired.
   * @return [iterator_column_index, iterator_entry_index]
   */
  [[nodiscard]] std::pair<std::vector<std::size_t>::iterator, std::vector<double>::iterator>
  end_column(const std::size_t row) {
    ASSERT_DEBUG(row < size_row(), "problems todo");
    return std::make_pair(std::next(column_index.begin(), static_cast<size_t_signed>(row_non_zero[row + 1])),
                          std::next(entry.begin(), static_cast<size_t_signed>(row_non_zero[row + 1])));
  }

  /**
   * @brief Returns a pair of constant iterators to the beginning of the column index and entries for the row.
   * @param[in] row The row index to the for which the iterators are desired.
   * @return [const_iterator_column_index, const_iterator_entry_index]
   */
  [[nodiscard]] std::pair<std::vector<std::size_t>::const_iterator, std::vector<double>::const_iterator>
  cbegin_column(const std::size_t row) const {
    ASSERT_DEBUG(row < size_row(), "problems todo");
    return std::make_pair(
      std::next(column_index.cbegin(), static_cast<size_t_signed>(row_non_zero[row])),
      std::next(entry.cbegin(), static_cast<size_t_signed>(row_non_zero[row])));
  }

  /**
   * @brief Returns a pair of constant iterators to the end of the column index and entries for a row.
   * @param[in] row The row index to the for which the iterators are desired.
   * @return [const_iterator_column_index, const_iterator_entry_index]
   */
  [[nodiscard]] std::pair<std::vector<std::size_t>::const_iterator, std::vector<double>::const_iterator>
  cend_column(const std::size_t row) const {
    ASSERT_DEBUG(row < size_row(), "problems todo");
    return std::make_pair(std::next(column_index.cbegin(), static_cast<size_t_signed>(row_non_zero[row + 1])),
                          std::next(entry.cbegin(), static_cast<size_t_signed>(row_non_zero[row + 1])));
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Capacity
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Checks whether the matrix is empty.
   * @return True if the matrix is empty, else false.
   */
  [[nodiscard]] bool empty() const noexcept { return row_non_zero.empty(); };

  /**
   * @brief Returns the number of rows in the matrix.
   * @return The number of rows.
   */
  [[nodiscard]] std::size_t size_row() const noexcept { return !row_non_zero.empty() ? row_non_zero.size() - 1 : 0; };

  /**
   * @brief Returns the number of columns in the matrix.
   * @return The number of columns.
   */
  [[nodiscard]] constexpr std::size_t size_column() const noexcept { return column_size; };

  /**
   * @brief Returns the number of non-zeros in the matrix.
   * @return The number of non-zeros.
   */
  [[nodiscard]] std::size_t size_non_zero() const noexcept { return column_index.size(); };

  /**
   * @brief Returns the number of rows and columns in the matrix. If rows are 0, columns are 0.
   * @return Pair containing [rows, columns].
   */
  [[nodiscard]] std::pair<std::size_t, std::size_t> size() const noexcept {
    return std::make_pair(size_row(), size_column());
  };

  /**
   * @brief Reserves storage for the matrix.
   * @param[in] row The number of rows to reserve.
   * @param[in] non_zero The number of non-zeros to reserve. Note this is not the column size.
   */
  void reserve(const std::size_t& row, const std::size_t& non_zero) noexcept {
    row_non_zero.reserve(row);
    column_index.reserve(non_zero);
    entry.reserve(non_zero);
  };

  /**
   * @brief Returns the number of rows and non-zeros that can be held in currently allocated storage.
   * @return [number of rows which can be held, number of non-zeros which can be held.]
   */
  [[nodiscard]] std::pair<std::size_t, std::size_t> capacity() const noexcept {
    return std::make_pair(row_non_zero.capacity(), column_index.capacity());
  };

  /**
   * @brief reduces memory usage by the matrix by freeing unused memory for both rows and non-zeros
   */
  void shrink_to_fit() noexcept {
    row_non_zero.shrink_to_fit();
    column_index.shrink_to_fit();
    entry.shrink_to_fit();
  };

  //--------------------------------------------------------------------------------------------------------------------
  // Modifiers
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Clears the contents of the matrix, sets the column size to zero.
   */
  void clear() noexcept {
    row_non_zero.clear();
    column_index.clear();
    entry.clear();
    column_size = 0;
  };

  /**
   * @brief Inserts an element with the parsed value into the specified row and column.
   * @param[in] row The row index to insert the value.
   * @param[in] column The column index to insert the value.
   * @param[in] value The value to insert.
   *
   * Note: if there is an existing non-zero in the row and column specified an error will be thrown.
   */
  void insert(const std::size_t& row, const std::size_t& column, const double& value) {
    ASSERT_DEBUG(row < size_row(),
                 "Row " + std::to_string(row) + " not in range [0, " + std::to_string(size_row()) + ").");
    ASSERT_DEBUG(column < size_column(),
                 "Column " + std::to_string(column) + " not in range [0, " + std::to_string(size_column()) + ").");
    const auto [iter_column_start, iter_entry_start] = cbegin_column(row);
    const auto [iter_column_end, iter_entry_end] = cend_column(row);
    auto iter_column_insert = std::lower_bound(iter_column_start, iter_column_end, column);
    ASSERT_DEBUG(iter_column_insert == iter_column_end || column != *iter_column_insert,
                 "Entry [" + std::to_string(row) + ", " + std::to_string(column) +
                 "] does not exist in the matrix (i.e. is zero). ");
    for(auto non_zeros = std::next(row_non_zero.begin(), static_cast<size_t_signed>(row + 1));
        non_zeros < row_non_zero.end(); ++(*non_zeros++));
    entry.insert(entry.begin() + std::distance(column_index.cbegin(), iter_column_insert), value);
    column_index.insert(iter_column_insert, column);
  };

  /**
   * @brief Erases an element at the specified row and column.
   * @param[in] row The row index for the entry to erase.
   * @param[in] column The column index for the row entry to erase.
   */
  void erase(const std::size_t& row, const std::size_t& column) {
    ASSERT_DEBUG(row < size_row(),
                 "Row " + std::to_string(row) + " not in range [0, " + std::to_string(size_row()) + ").");
    ASSERT_DEBUG(column < size_column(),
                 "Column " + std::to_string(column) + " not in range [0, " + std::to_string(size_column()) + ").");
    const auto [iter_column_start, iter_entry_start] = cbegin_column(row);
    const auto [iter_column_end, iter_entry_end] = cend_column(row);
    auto iter_column_insert = std::lower_bound(iter_column_start, iter_column_end, column);
    ASSERT_DEBUG(iter_column_insert != iter_column_end,
                 "Entry [" + std::to_string(row) + ", " + std::to_string(column) +
                 "] does not exist in the matrix (i.e. is zero). ");
    for(auto non_zeros = std::next(row_non_zero.begin(), static_cast<size_t_signed>(row + 1));
        non_zeros < row_non_zero.end(); --(*non_zeros++));
    entry.erase(entry.begin() + std::distance(column_index.cbegin(), iter_column_insert));
    column_index.erase(iter_column_insert);
  };

  /**
   * @brief Changes the number of rows and columns of the matrix.
   * @param[in] row Number of rows to resized the matrix to.
   * @param[in] column Number of columns to resized the matrix to.
   *
   * Note: Does not effect number of non-zeros.
   */
  void resize(const std::size_t& row, const std::size_t& column) noexcept {
    row_non_zero.resize(row + 1, row_non_zero.empty() ? 0 : row_non_zero.back());
    column_size = column;
  };

  /**
   * @brief Swaps the contents of the matrix with the parsed matrix
   * @param[in,out] other The other matrix, this matrix will obtain the other's value and visa versa.
   */
  void swap(Matrix_Sparse& other) noexcept {
    std::swap(row_non_zero, other.row_non_zero);
    std::swap(column_index, other.column_index);
    std::swap(entry, other.entry);
    std::swap(column_size, other.column_size);
  }

private:

  std::vector<std::size_t> row_non_zero;
  std::vector<std::size_t> column_index;
  std::vector<double> entry;
  std::size_t column_size {0};

  friend std::ostream& operator<<(std::ostream& stream, Matrix_Sparse& matrix);


};

std::ostream& operator<<(std::ostream& stream, Matrix_Sparse& matrix) {
  stream<<std::endl;
  FOR_EACH(row, matrix.row_non_zero) stream<<row<<" ";
  stream<<std::endl;
  FOR_EACH(col, matrix.column_index) stream<<col<<" ";
  stream<<std::endl;
  FOR_EACH(data, matrix.entry) stream<<data<<" ";
  stream<<std::endl;
  return stream;
}

double& Matrix_Sparse_Row::operator[](const std::size_t& column) {
  auto col_iter = std::lower_bound(row_col_offset.begin(), row_col_offset.end(), column);

  if(col_iter == row_col_offset.end()) {
    matrix->insert(row_index, column, 0.0);
    *this = (*matrix)[row_index];
    col_iter = std::lower_bound(row_col_offset.begin(), row_col_offset.end(),
                                column); // iterator might have been invalidated.
  }
  return row_entry[std::distance(row_col_offset.begin(), col_iter)];
}

}

#endif //DISA_MATRIX_SPARSE_H
