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

// Forward declarations
class Matrix_Sparse;

class Matrix_Sparse_Row;

struct Matrix_Sparse_Entry;
struct Matrix_Sparse_Entry_Const;
struct Iterator_Matrix_Sparse;
struct Const_Iterator_Matrix_Sparse;
struct Const_Matrix_Sparse_Entry;

//----------------------------------------------------------------------------------------------------------------------
// Matrix Sparse
//----------------------------------------------------------------------------------------------------------------------

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

  /**
   * @brief Initaliser list based on the 'raw' data strucutre of a sparse matrix.
   * @param[in] non_zero The non-zero offsets per row, must be sized to number of rows + 1, and the first index should be 0.
   * @param[in] index The column index of the non-zero value in each row, size must correspond to value, need not be be sorted.
   * @param[in] value The value at each non-zero position in the matrix, corresponds to index.
   * @param[in] column The absolute number of columns per row.
   */
  Matrix_Sparse(std::initializer_list<std::size_t> non_zero, std::initializer_list<std::size_t> index,
                std::initializer_list<double> value, std::size_t column);

  /**
   * @brief Default destructor.
   */
  ~Matrix_Sparse() = default;

  //--------------------------------------------------------------------------------------------------------------------
  // Member functions
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Performs a trivial deep copy of the assigned matrix.
   * @param[in] other The other sparse matrix.
   * @return Updated copy of this matrix.
   */
  Matrix_Sparse& operator=(const Matrix_Sparse& other) = default;

  //--------------------------------------------------------------------------------------------------------------------
  // Element Access
  //--------------------------------------------------------------------------------------------------------------------

  Iterator_Matrix_Sparse operator[](const std::size_t row) {
    const auto [iter_col_offset, iter_entry] = begin_column(row);
    const std::size_t row_size = row_non_zero[row + 1] - row_non_zero[row];
    return {row, column_size, std::span(iter_col_offset, row_size), std::span(iter_entry, row_size), this};
  };

  Const_Iterator_Matrix_Sparse operator[](const std::size_t row) {
    const auto [iter_col_offset, iter_entry] = begin_column(row);
    const std::size_t row_size = row_non_zero[row + 1] - row_non_zero[row];
    return {row, column_size, std::span(iter_col_offset, row_size), std::span(iter_entry, row_size), this};
  };
  //
  //  std::tuple<> front();
  //  std::tuple<> back();
  //  std::tuple<> at();
  //  std::tuple<> date();

  //--------------------------------------------------------------------------------------------------------------------
  // Iterators
  //--------------------------------------------------------------------------------------------------------------------

  Iterator_Matrix_Sparse begin();

  Const_Iterator_Matrix_Sparse begin() const;

  Const_Iterator_Matrix_Sparse cbegin() const;

  Iterator_Matrix_Sparse end();

  Const_Iterator_Matrix_Sparse end() const;

  Const_Iterator_Matrix_Sparse cend() const;

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

  friend struct Iterator_Matrix_Sparse;

  [[nodiscard]] std::string row_column(std::size_t i_row, std::size_t i_column) const {
    return std::to_string(i_row) + ", " + std::to_string(i_column);
  };

  [[nodiscard]] std::string range_row() const { return "[0, " + std::to_string(row_non_zero.size() - 1) + ")"; };

  [[nodiscard]] std::string range_column() const { return "[0, " + std::to_string(column_size) + ")"; };

  [[nodiscard]] std::string range_row_column() const { return "[" + range_row() + ", " + range_column() + "]"; };


};

//----------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Helper Classes
//----------------------------------------------------------------------------------------------------------------------

class Matrix_Sparse_Row {

public:
  Matrix_Sparse_Row(std::size_t i_row, std::span<std::size_t> col_offset, std::span<double> entry,
                    Matrix_Sparse* sparse_matrix)
    : row_index(i_row), offset(col_offset), row_entry(entry), matrix(sparse_matrix) {};

  double& operator[](const std::size_t& i_column);

  const double& operator[](const std::size_t& i_column) const;

private:

  std::span<std::size_t> offset;
  std::span<double> row_entry;
  Matrix_Sparse* matrix;
  std::size_t row_index;
};

struct Matrix_Sparse_Entry {
  const std::size_t& row;
  const std::size_t& column;
  double& value;
};

struct Matrix_Sparse_Entry_Const {
  const std::size_t& row;
  const std::size_t& column;
  const double& value;
};

//----------------------------------------------------------------------------------------------------------------------
// Iterators for Sparse Matrices
//----------------------------------------------------------------------------------------------------------------------

struct Iterator_Matrix_Sparse_Row { //: public std::iterator <std::bi_directional_iterator_tag> {

  Iterator_Matrix_Sparse_Row(std::size_t& row, std::size_t* column_index_ptr, double* entry_ptr)
    : row_index(row), column_index(column_index_ptr), entry(entry_ptr) {};

  Iterator_Matrix_Sparse_Row& operator++() {
    column_index++;
    entry++;
    return *this;
  }

  Iterator_Matrix_Sparse_Row operator++(int) {
    Iterator_Matrix_Sparse_Row iter = *this;
    ++(*this);
    return iter;
  }

  Iterator_Matrix_Sparse_Row& operator--() {
    column_index--;
    entry--;
    return *this;
  }

  Iterator_Matrix_Sparse_Row operator--(int) {
    Iterator_Matrix_Sparse_Row iter = *this;
    --(*this);
    return iter;
  }

  Matrix_Sparse_Entry operator*() {
    return {row_index, *column_index, *entry};
  }

  Const_Matrix_Sparse_Entry operator*() const {
    return {row_index, *column_index, *entry};
  }

  bool operator==(const Iterator_Matrix_Sparse_Row& other) const {
    return column_index == other.column_index;
  }

  bool operator!=(const Iterator_Matrix_Sparse_Row& other) const {
    return column_index != other.column_index;
  }

private:

  const std::size_t& row_index;
  std::size_t* column_index;
  double* entry;
};

struct Iterator_Matrix_Sparse {

  Iterator_Matrix_Sparse(Matrix_Sparse* matrix_ptr, const std::size_t& i_row)
    : matrix(matrix_ptr), row_index(i_row) {};

  Iterator_Matrix_Sparse& operator++() {
    row_index++;
    return *this;
  }

  Iterator_Matrix_Sparse operator++(int) {
    Iterator_Matrix_Sparse iter = *this;
    ++row_index;
    return iter;
  }

  Iterator_Matrix_Sparse& operator--() {
    row_index--;
    return *this;
  }

  Iterator_Matrix_Sparse operator--(int) {
    Iterator_Matrix_Sparse iter = *this;
    --row_index;
    return iter;
  }

  Iterator_Matrix_Sparse_Row begin() {
    return {row_index, &matrix->column_index[row_index], &matrix->entry[row_index]};
  };

  Iterator_Matrix_Sparse_Row end() {
    return {row_index, &matrix->column_index[row_index + 1], &matrix->entry[row_index + 1]};
  };

  Matrix_Sparse_Row operator*() {
    return {row_index,
            {std::next(matrix->column_index.begin(),
                       static_cast<size_t_signed>(matrix->row_non_zero[row_index])), matrix->size_column()},
            {std::next(matrix->entry.begin(),
                       static_cast<size_t_signed>(matrix->row_non_zero[row_index])), matrix->size_column()},
            matrix};
  }

  Matrix_Sparse_Row operator*() const {
    return {row_index,
            {std::next(matrix->column_index.begin(),
                       static_cast<size_t_signed>(matrix->row_non_zero[row_index])), matrix->size_column()},
            {std::next(matrix->entry.begin(),
                       static_cast<size_t_signed>(matrix->row_non_zero[row_index])), matrix->size_column()},
            matrix};
  }

  double& operator[](const std::size_t& column) {
    const auto& start_iter = matrix->column_index.begin() + static_cast<size_t_signed>(matrix->row_non_zero[row_index]);
    const auto& end_index =
      matrix->column_index.begin() + static_cast<size_t_signed>(matrix->row_non_zero[row_index + 1]);
    auto col_iter = std::lower_bound(start_iter, end_index, column);
    if(col_iter == end_index) {
      matrix->insert(row_index, column, double());
      *this = (*matrix)[row_index];
      col_iter = std::lower_bound(
        matrix->column_index.begin() + static_cast<size_t_signed>(matrix->row_non_zero[row_index]),
        matrix->column_index.begin() + static_cast<size_t_signed>(matrix->row_non_zero[row_index + 1]),
        column); // iterator might have been invalidated.
    }
    return *(matrix->entry.begin() + std::distance(matrix->column_index.begin(), col_iter));
  }

  const double& operator[](const std::size_t& column) const {
    const auto& start_iter = matrix->column_index.begin() + static_cast<size_t_signed>(matrix->row_non_zero[row_index]);
    const auto& end_index =
      matrix->column_index.begin() + static_cast<size_t_signed>(matrix->row_non_zero[row_index + 1]);
    auto col_iter = std::lower_bound(start_iter, end_index, column);
    ASSERT_DEBUG(col_iter != end_index, "does not exist.");
    return *(matrix->entry.begin() + std::distance(matrix->column_index.begin(), col_iter));
  }

  bool operator==(const Iterator_Matrix_Sparse& other) const {
    return matrix == other.matrix && row_index == other.row_index;
  }

  bool operator!=(const Iterator_Matrix_Sparse& other) const {
    return matrix == other.matrix && row_index == other.row_index;
  }

private:
  Matrix_Sparse* matrix;
  std::size_t row_index;
};

struct Const_Iterator_Matrix_Sparse {

};

//----------------------------------------------------------------------------------------------------------------------
// Stand Alone Sparse Matrix Operators
//----------------------------------------------------------------------------------------------------------------------

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

}

#endif //DISA_MATRIX_SPARSE_H
