// ---------------------------------------------------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------------------------------------------------
//  File Name: matrix_sparse.h
//  Description: todo
// ---------------------------------------------------------------------------------------------------------------------

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
class Matrix_Sparse_Row_Const;
struct Iterator_Matrix_Sparse_Row;
struct Iterator_Matrix_Sparse_Row_Const;
struct Iterator_Matrix_Sparse_Element;
struct Iterator_Matrix_Sparse_Element_Const;

//----------------------------------------------------------------------------------------------------------------------
// Matrix Sparse
//----------------------------------------------------------------------------------------------------------------------

/**
 * @class Matrix_Sparse
 * @brief todo
 *
 * todo
 * Design Philosophy:
 *
 * Element accessors and Iterators act like a matrix dense and support random look-up and insertion. A helper class use
 * used to provide 'peel off' the outer or row indexing and then provide access to the 'column'.
 *
 * Modifiers and Lookups functions act like a std::map where the i_row and i_column effectively are the 'key' and
 * return iterators to the actual value entries - i.e. access to the 0.0 entries is essentially a 'null space'.
 *
 *
 */
class Matrix_Sparse {

public:
  // shorthands
  using iterator = Iterator_Matrix_Sparse_Row;
  using iterator_element = Iterator_Matrix_Sparse_Element;
  using const_iterator = Iterator_Matrix_Sparse_Row_Const;
  using const_iterator_element = Iterator_Matrix_Sparse_Element_Const;

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
   * @brief Initializer list based on the 'raw' data structure of a sparse matrix.
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

  /**
   * @brief Subscript operator for access to a specified matrix row.
   * @param[in] i_row The row index of the matrix.
   * @return A sparse matrix row helper class, providing further operators for column access.
   */
  constexpr Matrix_Sparse_Row operator[](const std::size_t& i_row);

  /**
   * @brief Subscript operator for access to a specified matrix row.
   * @param[in] i_row The row index of the matrix.
   * @return A constant sparse matrix row helper class, providing further operators for column access.
   */
  constexpr Matrix_Sparse_Row_Const operator[](const std::size_t& i_row) const;

  //--------------------------------------------------------------------------------------------------------------------
  // Iterators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Returns an iterator to the beginning row of the matrix.
   * @return A row iterator.
   */
  constexpr iterator begin() noexcept;

  /**
   * @brief Returns a const iterator to the beginning row of the matrix.
   * @return A constant row iterator.
   */
  [[nodiscard]] constexpr const_iterator begin() const noexcept;

  /**
   * @brief Returns a const iterator to the beginning row of the matrix.
   * @return A constant row iterator.
   */
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept;

  /**
   * @brief Returns an iterator to the end row (i.e. 1 past the last row) of the matrix.
   * @return A row iterator.
   */
  constexpr iterator end() noexcept;

  /**
   * @brief Returns a constant iterator to the end row (i.e. 1 past the last row) of the matrix.
   * @return A row iterator.
   */
  [[nodiscard]] constexpr const_iterator end() const noexcept;

  /**
   * @brief Returns a constant iterator to the end row (i.e. 1 past the last row) of the matrix.
   * @return A row iterator.
   */
  [[nodiscard]] constexpr const_iterator cend() const noexcept;

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
    element_value.reserve(non_zero);
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
    element_value.shrink_to_fit();
  };

  //--------------------------------------------------------------------------------------------------------------------
  // Modifiers
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Clears the contents of the matrix, sets the column size to zero.
   */
  void clear() noexcept;

  /**
   * @brief Inserts an value with the parsed value at [i_row, i_column], if an value exists nothing occurs.
   * @param[in] i_row The i_row index to insert the value.
   * @param[in] i_column The i_column index to insert the value.
   * @param[in] value The value to insert.
   * @return [iterator to the inserted or existing element, true if insertion took place, else false].
   */
  std::pair<iterator_element, bool> insert(const std::size_t& i_row, const std::size_t& i_column, const double& value);

  /**
   * todo
   */
  std::pair<iterator_element, bool> insert(const iterator_element& iter_insert, const std::size_t& i_column,
                                           const double& value);

  /**
   * @brief Inserts or updates an value with the parsed value at [i_row, i_column].
   * @param[in] i_row The i_row index to insert the value.
   * @param[in] i_column The i_column index to insert the value.
   * @param[in] value The value to insert.
   * @return [iterator to the inserted or existing element, true if insertion took place, else false].
   */
  std::pair<iterator_element, bool>
  insert_or_assign(const std::size_t& i_row, const std::size_t& i_column, const double& value);

  /**
   * @brief Erases an value at the specified row and column.
   * @param[in] iter_element The iterator to the value to delete.
   * @return The iterator to the value after the deleted value.
   */
  iterator_element erase(const Iterator_Matrix_Sparse_Element& iter_element);

  /**
   * @brief Changes the number of rows and columns of the matrix.
   * @param[in] row Number of rows to resized the matrix to.
   * @param[in] column Number of columns to resized the matrix to.
   *
   * Note: Does not effect number of non-zeros.
   */
  constexpr void resize(const std::size_t& row, const std::size_t& column) noexcept;

  /**
   * @brief Swaps the contents of the matrix with the parsed matrix
   * @param[in,out] other The other matrix, this matrix will obtain the other's value and visa versa.
   */
  constexpr void swap(Matrix_Sparse& other) noexcept;

  //--------------------------------------------------------------------------------------------------------------------
  // Lookup
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Finds the value iterator to the matrix value at a row and column.
   * @param[in] i_row The row index to the element.
   * @param[in] i_column The column index to the element.
   * @return Element iterator.
   *
   * Note:
   * 1. If the row index is not in [0, row_size()) the iterator will point to the end()->end().
   * 2. If the value at i_row, i_column is zero, the iterator will return (begin() + i_row)->end().
   */
  iterator_element find(const std::size_t& i_row, const std::size_t& i_column);

  /**
   * @brief Finds the value iterator to the matrix value at a row and column.
   * @param[in] i_row The row index to the element.
   * @param[in] i_column The column index to the element.
   * @return constant element iterator.
   *
   * Note:
   * 1. If the row index is not in [0, row_size()) the iterator will point to the end()->end().
   * 2. If the value at i_row, i_column is zero, the iterator will return (begin() + i_row)->end().
   */
  [[nodiscard]] const_iterator_element find(const std::size_t& i_row, const std::size_t& i_column) const;

  /**
   * @brief Checks if the element at a row and column are non-zero.
   * @param[in] i_row The row index to the element.
   * @param[in] i_column The column index to the element.
   * @return True if non-zero, else false.
   */
  [[nodiscard]] bool contains(const std::size_t& i_row, const std::size_t& i_column) const;

private:
  std::vector<std::size_t> row_non_zero;    //!< The number of total number preceding non-zero elements in the matrix for each row (sized to size_row() + 1, last value is the total number of non-zeros in the matrix.).
  std::vector<std::size_t> column_index;    //!< The column index for each non-zero value, corresponds to value.
  std::vector<double> element_value;        //!< The each non-zero value value, corresponds to column_index.
  std::size_t column_size {0};              //!< The number of columns of the matrix (used to check validity of operations).

  /**
   * @brief Converts and formates a row and colum index to string.
   * @param[in] i_row The row index.
   * @param[in] i_column The column index.
   * @return Formatted string.
   */
  [[nodiscard]] static std::string row_column(std::size_t i_row, std::size_t i_column) {
    return std::to_string(i_row) + ", " + std::to_string(i_column);
  };

  /**
   * @brief Creates a string formatted for the range of the matrix rows.
   * @return Formatted string.
   */
  [[nodiscard]] std::string range_row() const { return "[0, " + std::to_string(row_non_zero.size() - 1) + ")"; };

  /**
   * @brief Creates a string formatted for the range of the matrix columns.
   * @return Formatted string.
   */
  [[nodiscard]] std::string range_column() const { return "[0, " + std::to_string(column_size) + ")"; };

  /**
   * @brief Creates a string formatted for the range of both rows and columns.
   * @return Formatted string.
   */
  [[nodiscard]] std::string range_row_column() const { return "[" + range_row() + ", " + range_column() + "]"; };

  // Friend types and functions.
  friend class Matrix_Sparse_Row;
  friend class Matrix_Sparse_Row_Const;
  friend struct Iterator_Matrix_Sparse_Row;
  friend struct Iterator_Matrix_Sparse_Row_Const;
  friend std::ostream& operator<<(std::ostream& stream, Matrix_Sparse& matrix);
};

//----------------------------------------------------------------------------------------------------------------------
// Helper Classes for Sparse Matrices
//----------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Row Non-Const Helper
//----------------------------------------------------------------------------------------------------------------------

class Matrix_Sparse_Row {

public:
  // shorthands
  using iterator = Iterator_Matrix_Sparse_Element;
  using const_iterator = Iterator_Matrix_Sparse_Element_Const;

  constexpr Matrix_Sparse_Row(const Matrix_Sparse_Row& other) = default;

  constexpr Matrix_Sparse_Row(std::size_t i_row, Matrix_Sparse* sparse_matrix) noexcept
  : matrix(sparse_matrix), row_index(i_row) {};

  iterator begin();

  iterator end();

  [[nodiscard]] const_iterator begin() const;

  [[nodiscard]] const_iterator end() const;

  double& operator[](const std::size_t& i_column);

  const double& operator[](const std::size_t& i_column) const;

private:
  Matrix_Sparse* const matrix;
  std::size_t row_index;

  friend class Matrix_Sparse_Row_Const;
  friend struct Iterator_Matrix_Sparse_Row;
  friend struct Iterator_Matrix_Sparse_Row_Const;
};

//----------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Row Const Helper
//----------------------------------------------------------------------------------------------------------------------

class Matrix_Sparse_Row_Const {

public:
  using iterator = Iterator_Matrix_Sparse_Element_Const;

  constexpr Matrix_Sparse_Row_Const(std::size_t i_row, const Matrix_Sparse* const sparse_matrix) noexcept
    : matrix(sparse_matrix), row_index(i_row) {
    ASSERT_DEBUG(sparse_matrix != nullptr, "Constructing with null pointer.");
  };

  explicit constexpr Matrix_Sparse_Row_Const(const Matrix_Sparse_Row& other) noexcept
    : matrix(other.matrix), row_index(other.row_index) {};

  [[nodiscard]] iterator begin() const;

  [[nodiscard]] iterator end() const;

  const double& operator[](const std::size_t& i_column) const;

private:
  const Matrix_Sparse* const matrix;
  std::size_t row_index;

  friend struct Iterator_Matrix_Sparse_Row;
  friend struct Iterator_Matrix_Sparse_Row_Const;
};

//----------------------------------------------------------------------------------------------------------------------
// Iterators for Sparse Matrices
//----------------------------------------------------------------------------------------------------------------------
// Iterators for Sparse Matrix Rows Non-Const
//----------------------------------------------------------------------------------------------------------------------

struct Iterator_Matrix_Sparse_Row : public std::iterator<std::bidirectional_iterator_tag, Matrix_Sparse_Row> {

  using const_value_type = Matrix_Sparse_Row_Const;
  using const_reference = Matrix_Sparse_Row_Const&;

  explicit Iterator_Matrix_Sparse_Row(const Matrix_Sparse_Row& row) : matrix_row(row) {};

  constexpr Iterator_Matrix_Sparse_Row(std::size_t i_row, Matrix_Sparse* sparse_matrix)
    : matrix_row(Matrix_Sparse_Row(i_row, sparse_matrix)) {};

  constexpr Iterator_Matrix_Sparse_Row& operator++() {
    matrix_row.row_index++;
    return *this;
  }

  Iterator_Matrix_Sparse_Row operator++(int) {
    Iterator_Matrix_Sparse_Row iter = *this;
    ++matrix_row.row_index;
    return iter;
  }

  Iterator_Matrix_Sparse_Row operator+(difference_type offest) {
    matrix_row.row_index += offest;
    return *this;
  }

  constexpr Iterator_Matrix_Sparse_Row& operator--() {
    matrix_row.row_index--;
    return *this;
  }

  Iterator_Matrix_Sparse_Row operator--(int) {
    Iterator_Matrix_Sparse_Row iter = *this;
    --matrix_row.row_index;
    return iter;
  }

  Iterator_Matrix_Sparse_Row operator-(difference_type offest) {
    matrix_row.row_index -= offest;
    return *this;
  }

  constexpr pointer operator->() { return &matrix_row; }

  reference operator*() { return matrix_row; }

  const_reference operator*() const { return const_reference(matrix_row); }

  value_type operator[](const difference_type& i_advance) { return *std::next(*this, i_advance); }

  const_reference operator[](const difference_type& i_advance) const {
    return const_reference(*std::next(*this, i_advance));
  }

  bool operator==(const Iterator_Matrix_Sparse_Row& other) const {
    return matrix_row.matrix == other.matrix_row.matrix && matrix_row.row_index == other.matrix_row.row_index;
  }

  bool operator!=(const Iterator_Matrix_Sparse_Row& other) const {
    return matrix_row.matrix == other.matrix_row.matrix && matrix_row.row_index == other.matrix_row.row_index;
  }

private:
  Matrix_Sparse_Row matrix_row;
};

//----------------------------------------------------------------------------------------------------------------------
// Iterators for Sparse Matrix Rows Const
//----------------------------------------------------------------------------------------------------------------------

struct Iterator_Matrix_Sparse_Row_Const
  : public std::iterator<std::bidirectional_iterator_tag, Matrix_Sparse_Row_Const> {

  explicit Iterator_Matrix_Sparse_Row_Const(const Matrix_Sparse_Row_Const row) : matrix_row(row) {};

  constexpr Iterator_Matrix_Sparse_Row_Const(std::size_t i_row, const Matrix_Sparse* const sparse_matrix)
    : matrix_row(Matrix_Sparse_Row_Const(i_row, sparse_matrix)) {};

  constexpr Iterator_Matrix_Sparse_Row_Const& operator++() {
    matrix_row.row_index++;
    return *this;
  }

  Iterator_Matrix_Sparse_Row_Const operator++(int) {
    Iterator_Matrix_Sparse_Row_Const iter = *this;
    ++matrix_row.row_index;
    return iter;
  }

  Iterator_Matrix_Sparse_Row_Const operator+(difference_type offest) {
    matrix_row.row_index += offest;
    return *this;
  }

  constexpr Iterator_Matrix_Sparse_Row_Const& operator--() {
    matrix_row.row_index--;
    return *this;
  }

  Iterator_Matrix_Sparse_Row_Const operator--(int) {
    Iterator_Matrix_Sparse_Row_Const iter = *this;
    --matrix_row.row_index;
    return iter;
  }

  Iterator_Matrix_Sparse_Row_Const operator-(difference_type offest) {
    matrix_row.row_index -= offest;
    return *this;
  }

  constexpr pointer operator->() { return &matrix_row; }

  reference operator*() { return matrix_row; }

  reference operator*() const { return reference(matrix_row); }

  value_type operator[](const difference_type& i_advance) { return *std::next(*this, i_advance); }

  reference operator[](const difference_type& i_advance) const {
    return reference(*std::next(*this, i_advance));
  }

  bool operator==(const Iterator_Matrix_Sparse_Row_Const& other) const {
    return matrix_row.matrix == other.matrix_row.matrix && matrix_row.row_index == other.matrix_row.row_index;
  }

  bool operator!=(const Iterator_Matrix_Sparse_Row_Const& other) const {
    return matrix_row.matrix == other.matrix_row.matrix && matrix_row.row_index == other.matrix_row.row_index;
  }

private:
  Matrix_Sparse_Row_Const matrix_row;
};

//----------------------------------------------------------------------------------------------------------------------
// Iterators for Sparse Matrix Elements Non-Const
//----------------------------------------------------------------------------------------------------------------------

struct Iterator_Matrix_Sparse_Element : public std::iterator<std::random_access_iterator_tag, double> {

  Iterator_Matrix_Sparse_Element(Matrix_Sparse* sparse_matrix, const std::size_t& i_row, std::size_t* i_column_index,
                                 double* value)
    : row_index(i_row), matrix(sparse_matrix), column_index(i_column_index), value(value) {}

  constexpr Iterator_Matrix_Sparse_Element& operator++() {
    ++column_index;
    ++value;
    return *this;
  }

  Iterator_Matrix_Sparse_Element operator++(int) {
    Iterator_Matrix_Sparse_Element iter = *this;
    ++(*this);
    return iter;
  }

  constexpr Iterator_Matrix_Sparse_Element& operator--() {
    --column_index;
    --value;
    return *this;
  }

  Iterator_Matrix_Sparse_Element operator--(int) {
    Iterator_Matrix_Sparse_Element iter = *this;
    --(*this);
    return iter;
  }

  reference operator*() { return *value; }

  const double& operator*() const { return *value; }

  difference_type operator-(const Iterator_Matrix_Sparse_Element other) {
    return column_index - other.column_index;
  }

  Iterator_Matrix_Sparse_Element& operator+(const difference_type& offset) {
    column_index + offset;
    value + offset;
    return *this;
  }

  Iterator_Matrix_Sparse_Element& operator=(const Iterator_Matrix_Sparse_Element& other) {
    ASSERT_DEBUG(other.row_index == row_index, "Row difference in iterator.");
    ASSERT_DEBUG(other.matrix == matrix, "Different matrices.");
    if(this == &other) return *this;
    column_index = other.column_index;
    value = other.value;
    return *this;
  };

  Iterator_Matrix_Sparse_Element& operator+=(const difference_type& offset) {
    this + offset;
    return *this;
  }

  value_type operator[](const difference_type& i_advance) { return *(value + i_advance); }

  const double& operator[](const difference_type& i_advance) const { return *(value + i_advance); }

  bool operator==(const Iterator_Matrix_Sparse_Element& other) const {
    return matrix == other.matrix && row_index == other.row_index && column_index == other.column_index &&
           value == other.value;
  }

  bool operator!=(const Iterator_Matrix_Sparse_Element& other) const {
    return column_index != other.column_index || value != other.value || row_index == other.row_index ||
           matrix == other.matrix;
  }

  [[nodiscard]] const std::size_t& i_row() const { return row_index; };

  [[nodiscard]] const std::size_t& i_column() const { return *column_index; };

private:
  std::size_t* column_index;
  double* value;
  const std::size_t row_index;
  const Matrix_Sparse* matrix;
};

//----------------------------------------------------------------------------------------------------------------------
// Iterators for Sparse Matrix Elements Const
//----------------------------------------------------------------------------------------------------------------------

struct Iterator_Matrix_Sparse_Element_Const : public std::iterator<std::random_access_iterator_tag, double> {

  Iterator_Matrix_Sparse_Element_Const(const Matrix_Sparse* sparse_matrix, const std::size_t& i_row,
                                       const std::size_t* i_column_index, const double* const value)
    : row_index(i_row), matrix(sparse_matrix), column_index(i_column_index), value(value) {}

  constexpr Iterator_Matrix_Sparse_Element_Const& operator++() {
    ++column_index;
    ++value;
    return *this;
  }

  Iterator_Matrix_Sparse_Element_Const operator++(int) {
    Iterator_Matrix_Sparse_Element_Const iter = *this;
    ++(*this);
    return iter;
  }

  constexpr Iterator_Matrix_Sparse_Element_Const& operator--() {
    --column_index;
    --value;
    return *this;
  }

  Iterator_Matrix_Sparse_Element_Const operator--(int) {
    Iterator_Matrix_Sparse_Element_Const iter = *this;
    --(*this);
    return iter;
  }

  const double& operator*() const { return *value; }

  difference_type operator-(const Iterator_Matrix_Sparse_Element_Const other) {
    return column_index - other.column_index;
  }

  Iterator_Matrix_Sparse_Element_Const& operator+(const difference_type& offset) {
    column_index + offset;
    value + offset;
    return *this;
  }

  Iterator_Matrix_Sparse_Element_Const& operator=(const Iterator_Matrix_Sparse_Element_Const& other) {
    ASSERT_DEBUG(other.row_index == row_index, "Row difference in iterator.");
    ASSERT_DEBUG(other.matrix == matrix, "Different matrices.");
    if(this == &other) return *this;
    column_index = other.column_index;
    value = other.value;
    return *this;
  };

  Iterator_Matrix_Sparse_Element_Const& operator+=(const difference_type& offset) {
    this + offset;
    return *this;
  }

  value_type operator[](const difference_type& i_advance) { return *(value + i_advance); }

  const double& operator[](const difference_type& i_advance) const { return *(value + i_advance); }

  bool operator==(const Iterator_Matrix_Sparse_Element_Const& other) const {
    return matrix == other.matrix && row_index == other.row_index && column_index == other.column_index &&
           value == other.value;
  }

  bool operator!=(const Iterator_Matrix_Sparse_Element_Const& other) const {
    return column_index != other.column_index || value != other.value || row_index == other.row_index ||
           matrix == other.matrix;
  }

  [[nodiscard]] const std::size_t& i_row() const { return row_index; };

  [[nodiscard]] const std::size_t& i_column() const { return *column_index; };


private:
  const std::size_t* column_index;
  const double* value;
  const std::size_t row_index;
  const Matrix_Sparse* matrix;
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
  FOR_EACH(data, matrix.element_value) stream<<data<<" ";
  stream<<std::endl;
  return stream;
}

}

#endif //DISA_MATRIX_SPARSE_H
