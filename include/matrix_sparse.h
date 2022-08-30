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
template<typename _matrix_type> class Matrix_Sparse_Row;
template<typename _matrix_type> struct Iterator_Matrix_Sparse_Row;
template<typename _matrix_type> struct Iterator_Matrix_Sparse_Element;

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
  using iterator = Iterator_Matrix_Sparse_Row<Matrix_Sparse>;
  using const_iterator = Iterator_Matrix_Sparse_Row<const Matrix_Sparse>;
  using iterator_element = Iterator_Matrix_Sparse_Element<Matrix_Sparse>;
  using const_iterator_element = Iterator_Matrix_Sparse_Element<const Matrix_Sparse>;

  //--------------------------------------------------------------------------------------------------------------------
  // Public Member functions
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
  Matrix_Sparse_Row<Matrix_Sparse> operator[](const std::size_t& i_row);

  /**
   * @brief Subscript operator for access to a specified matrix row.
   * @param[in] i_row The row index of the matrix.
   * @return A constant sparse matrix row helper class, providing further operators for column access.
   */
  Matrix_Sparse_Row<const Matrix_Sparse> operator[](const std::size_t& i_row) const;

  //--------------------------------------------------------------------------------------------------------------------
  // Iterators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Returns an iterator to the beginning row of the matrix.
   * @return A row iterator.
   */
  iterator begin() noexcept;

  /**
   * @brief Returns a const iterator to the beginning row of the matrix.
   * @return A constant row iterator.
   */
  [[nodiscard]] const_iterator begin() const noexcept;

  /**
   * @brief Returns a const iterator to the beginning row of the matrix.
   * @return A constant row iterator.
   */
  [[nodiscard]] const_iterator cbegin() const noexcept;

  /**
   * @brief Returns an iterator to the end row (i.e. 1 past the last row) of the matrix.
   * @return A row iterator.
   */
  iterator end() noexcept;

  /**
   * @brief Returns a constant iterator to the end row (i.e. 1 past the last row) of the matrix.
   * @return A row iterator.
   */
  [[nodiscard]] const_iterator end() const noexcept;

  /**
   * @brief Returns a constant iterator to the end row (i.e. 1 past the last row) of the matrix.
   * @return A row iterator.
   */
  [[nodiscard]] const_iterator cend() const noexcept;

  //--------------------------------------------------------------------------------------------------------------------
  // Capacity
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Checks whether the matrix is empty. An empty matrix is considered where the number of rows is 0.
   * @return True if the matrix is empty, else false.
   */
  [[nodiscard]] bool empty() const noexcept { return row_non_zero.empty() || row_non_zero.size() < 2; };

  /**
   * @brief Returns the number of rows in the matrix.
   * @return The number of rows.
   */
  [[nodiscard]] std::size_t size_row() const noexcept {
    return !row_non_zero.empty() ? row_non_zero.size() - 1 : 0;
  };

  /**
   * @brief Returns the number of columns in the matrix.
   * @return The number of columns.
   */
  [[nodiscard]] std::size_t size_column() const noexcept { return column_size; };

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
   * @brief Returns the number of row offsets and non-zeros entries that can be held in currently allocated storage.
   * @return [number of rows offsets, number of non-zeros entries.]
   *
   * Note: The number of row offsets (for CSR matrices) is always one greater than the actual matrix row size.
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
  void clear() noexcept {
    row_non_zero.clear();
    column_index.clear();
    element_value.clear();
    column_size = 0;
  };

  /**
   * @brief Inserts an value with the parsed value at [i_row, i_column], if an value exists nothing occurs.
   * @param[in] i_row The i_row index to insert the value.
   * @param[in] i_column The i_column index to insert the value.
   * @param[in] value The value to insert.
   * @return [iterator to the inserted or existing element, true if insertion took place, else false].
   */
  std::pair<iterator_element, bool> insert(const std::size_t& i_row, const std::size_t& i_column, const double& value);

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
   *
   * Note: Undefined behaviour exists if the element does not exist, i.e. the iterator must be dereferenceable.
   */
  iterator_element erase(const Iterator_Matrix_Sparse_Element<Matrix_Sparse>& iter_element);

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
    std::swap(element_value, other.element_value);
    std::swap(column_size, other.column_size);
  };

  //--------------------------------------------------------------------------------------------------------------------
  // Lookup
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Finds the value iterator to the matrix value at a row and column.
   * @param[in] i_row The row index to the element.
   * @param[in] i_column The column index to the element.
   * @return Element iterator.
   *
   * Note: If i_row >= row_size() end() is returned, if no element is found *(begin + i_row).end() is returned.
   */
  iterator_element find(const std::size_t& i_row, const std::size_t& i_column);

  /**
   * @brief Finds the value iterator to the matrix value at a row and column.
   * @param[in] i_row The row index to the element.
   * @param[in] i_column The column index to the element.
   * @return constant element iterator.
   *
   * Note: If i_row >= row_size() end() is returned, if no element is found *(begin + i_row).end() is returned.
   */
  [[nodiscard]] const_iterator_element find(const std::size_t& i_row, const std::size_t& i_column) const;

  /**
   * @brief Checks if the element at a row and column are non-zero.
   * @param[in] i_row The row index to the element.
   * @param[in] i_column The column index to the element.
   * @return True if non-zero, else false.
   */
  [[nodiscard]] bool contains(const std::size_t& i_row, const std::size_t& i_column) const;

  /**
   * @brief returns an element iterator to the first element not less than the given key
   * @param[in] i_row The row index to the element.
   * @param[in] i_column The column index of the element.
   * @return Iterator pointing to the first element that is not less than key.
   *
   * Note: If i_row >= row_size() end() is returned, if no element is found *(begin + i_row).end() is returned.
   */
  iterator_element lower_bound(const std::size_t& i_row, const std::size_t& i_column);

  /**
   * @brief returns an element iterator to the first element not less than the given key
   * @param[in] i_row The row index to the element.
   * @param[in] i_column The column index of the element.
   * @return Const iterator pointing to the first element that is not less than key.
   *
   * Note: If i_row >= row_size() end() is returned, if no element is found *(begin + i_row).end() is returned.
   */
  [[nodiscard]] const_iterator_element lower_bound(const std::size_t& i_row, const std::size_t& i_column) const;

  //--------------------------------------------------------------------------------------------------------------------
  // Private Members
  //--------------------------------------------------------------------------------------------------------------------

private:
  std::vector<std::size_t> row_non_zero;    //!< The number of total number preceding non-zero elements in the matrix for each row (sized to size_row() + 1, last value is the total number of non-zeros in the matrix.).
  std::vector<std::size_t> column_index;    //!< The column index for each non-zero value, corresponds to value.
  std::vector<double> element_value;        //!< The each non-zero value value, corresponds to column_index.
  std::size_t column_size {0};              //!< The number of columns of the matrix (used to check validity of operations).

  //--------------------------------------------------------------------------------------------------------------------
  // Private Member Functions
  //--------------------------------------------------------------------------------------------------------------------

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
  friend class Matrix_Sparse_Row<Matrix_Sparse>;
  friend class Matrix_Sparse_Row<const Matrix_Sparse>;
  friend struct Iterator_Matrix_Sparse_Row<Matrix_Sparse>;
  friend struct Iterator_Matrix_Sparse_Row<const Matrix_Sparse>;
  friend struct Iterator_Matrix_Sparse_Element<Matrix_Sparse>;
  friend struct Iterator_Matrix_Sparse_Element<const Matrix_Sparse>;
  friend std::ostream& operator<<(std::ostream& stream, Matrix_Sparse& matrix);
};

//----------------------------------------------------------------------------------------------------------------------
// Helper Classes for Sparse Matrices
//----------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Row Non-Const Helper
//----------------------------------------------------------------------------------------------------------------------

/**
 * todo - make sure matrix type is sparse
 * @tparam _matrix_type
 */
template<typename _matrix_type>
class Matrix_Sparse_Row {
  //static_assert(std::is_same_v<Matrix_Sparse, std::remove_const<_matrix_type> >, "Must be a sparse matrix.");

public:
  // shorthands
  using matrix_type = _matrix_type;
  using iterator = Iterator_Matrix_Sparse_Element<_matrix_type>;
  using const_iterator = Iterator_Matrix_Sparse_Element<const Matrix_Sparse>;

  template<typename _matrix_type_other, typename std::enable_if_t<std::is_const_v<_matrix_type_other>, bool > = true>
  constexpr explicit Matrix_Sparse_Row(const Matrix_Sparse_Row<const Matrix_Sparse>& other)
    : matrix(other.matrix), row_index(other.row_index) {};

  constexpr explicit Matrix_Sparse_Row(const Matrix_Sparse_Row<Matrix_Sparse>& other)
  : matrix(other.matrix), row_index(other.row_index) {};

  constexpr Matrix_Sparse_Row(std::size_t i_row, matrix_type* sparse_matrix) noexcept
  : matrix(sparse_matrix), row_index(i_row) {};

  iterator begin();

  iterator end();

  [[nodiscard]] const_iterator begin() const;

  [[nodiscard]] const_iterator end() const;

  [[nodiscard]] const_iterator cbegin() const;

  [[nodiscard]] const_iterator cend() const;

  template<typename _matrix = _matrix_type, typename std::enable_if_t<!std::is_const_v<_matrix>, bool > = true >
  double& operator[](const std::size_t& i_column){
    auto iter_element = matrix->lower_bound(row_index, i_column);
    if(iter_element == end() || i_column != iter_element.i_column())
      iter_element = matrix->insert(row_index, i_column, double()).first;
    return *iter_element;
  };

  const double& operator[](const std::size_t& i_column) const{
    ASSERT_DEBUG(matrix->contains(row_index, i_column),
                 "Trying to access a zero element at [" + matrix->row_column(row_index, i_column) + "].");
    return *matrix->find(row_index, i_column);
  };

private:
  _matrix_type* const matrix;
  std::size_t row_index;

  friend class Matrix_Sparse_Row<Matrix_Sparse>;
  friend class Matrix_Sparse_Row<const Matrix_Sparse>;
  friend struct Iterator_Matrix_Sparse_Row<Matrix_Sparse>;
  friend struct Iterator_Matrix_Sparse_Row<const Matrix_Sparse>;
};

//----------------------------------------------------------------------------------------------------------------------
// Iterators for Sparse Matrices
//----------------------------------------------------------------------------------------------------------------------
// Iterators for Sparse Matrix Rows Non-Const
//----------------------------------------------------------------------------------------------------------------------

/**
 * todo - make sure matrix type is sparse
 * @tparam _matrix_type
 */
template<typename _matrix_type>
struct Iterator_Matrix_Sparse_Row {
  //static_assert(std::is_same_v<Matrix_Sparse, std::remove_const<_matrix_type> >, "Must be a sparse matrix.");

  using iterator_category = std::random_access_iterator_tag;
  using difference_type   = std::ptrdiff_t;
  using matrix_type       = _matrix_type;
  using value_type        = Matrix_Sparse_Row<_matrix_type>;
  using pointer           = value_type*;
  using reference         = value_type&;

  explicit Iterator_Matrix_Sparse_Row(const value_type& row) : matrix_row(row) {};

  constexpr Iterator_Matrix_Sparse_Row(std::size_t i_row, matrix_type* sparse_matrix)
  : matrix_row(value_type(i_row, sparse_matrix)) {};

  constexpr Iterator_Matrix_Sparse_Row& operator++() {
    matrix_row.row_index++;
    return *this;
  }

  constexpr Iterator_Matrix_Sparse_Row operator++(int) {
    Iterator_Matrix_Sparse_Row iter = *this;
    ++matrix_row.row_index;
    return iter;
  }

  constexpr Iterator_Matrix_Sparse_Row operator+(difference_type offset) {
    matrix_row.row_index += offset;
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

  constexpr Iterator_Matrix_Sparse_Row operator-(difference_type offset) {
    matrix_row.row_index -= offset;
    return *this;
  }

  template<typename _matrix = _matrix_type, typename std::enable_if_t<!std::is_const_v<_matrix>, bool > = true >
  pointer operator->() { return &matrix_row; }

  template<typename _matrix = _matrix_type, typename std::enable_if_t<!std::is_const_v<_matrix>, bool > = true >
  reference operator*() { return matrix_row; }

  Matrix_Sparse_Row<const Matrix_Sparse> operator*() const {
    return Matrix_Sparse_Row<const Matrix_Sparse>(matrix_row);
  }

  template<typename _matrix = _matrix_type, typename std::enable_if_t<!std::is_const_v<_matrix>, bool > = true >
  value_type operator[](const difference_type& i_offset) {
    return value_type(*std::next(*this, i_offset));
  }

  Matrix_Sparse_Row<const Matrix_Sparse> operator[](const difference_type& i_offset) const {
    return Matrix_Sparse_Row<const Matrix_Sparse>(*std::next(*this, i_offset));
  }

  bool operator==(const Iterator_Matrix_Sparse_Row& other) const {
    return matrix_row.matrix == other.matrix_row.matrix && matrix_row.row_index == other.matrix_row.row_index;
  }

  bool operator!=(const Iterator_Matrix_Sparse_Row& other) const {
    return !(*this == other);
  }

  Iterator_Matrix_Sparse_Row& operator+=(difference_type offset){
    matrix_row.row_index += offset;
    return *this;
  };

  Iterator_Matrix_Sparse_Row& operator-=(difference_type offset){
    matrix_row.row_index -= offset;
    return *this;
  };

private:
  value_type matrix_row;
};

//----------------------------------------------------------------------------------------------------------------------
// Iterators for Sparse Matrix Elements
//----------------------------------------------------------------------------------------------------------------------

/**
 * todo - make sure matrix type is sparse
 * @tparam _matrix_type
 */
template<typename _matrix_type>
struct Iterator_Matrix_Sparse_Element {

  using iterator_category = std::random_access_iterator_tag;
  using difference_type   = std::ptrdiff_t;
  using matrix_type       = _matrix_type;
  using value_type        = std::conditional_t<std::is_const_v<_matrix_type>, const double, double>;
  using size_type         = std::conditional_t<std::is_const_v<_matrix_type>, const std::size_t, std::size_t>;
  using pointer           = value_type*;
  using reference         = value_type&;

  Iterator_Matrix_Sparse_Element(matrix_type* sparse_matrix, const std::size_t& i_row, size_type* i_column_index,
                                 pointer value)
                                 : row_index(i_row), matrix(sparse_matrix), column_index(i_column_index), value(value)
                                 {}

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

  template<typename _matrix = _matrix_type, typename std::enable_if_t<!std::is_const_v<_matrix>, bool > = true >
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
    ASSERT_DEBUG(other.matrix == matrix, "Different matrices.");
    if(this == &other) return *this;
    row_index = other.row_index;
    column_index = other.column_index;
    value = other.value;
    return *this;
  };

  template<typename _matrix = _matrix_type, typename std::enable_if_t<!std::is_const_v<_matrix>, bool > = true >
  reference operator[](const difference_type& i_advance) { return *(value + i_advance); }

  const double& operator[](const difference_type& i_advance) const { return *(value + i_advance); }

  bool operator==(const Iterator_Matrix_Sparse_Element& other) const {
    return matrix == other.matrix && row_index == other.row_index && column_index == other.column_index &&
           value == other.value;
  }

  bool operator!=(const Iterator_Matrix_Sparse_Element& other) const {
    return !(*this == other);
  }

  Iterator_Matrix_Sparse_Element& operator+=(difference_type offset){
    this + offset;
    return *this;
  };

  Iterator_Matrix_Sparse_Element& operator-=(difference_type offset){
    this - offset;
    return *this;
  };

  [[nodiscard]] const std::size_t& i_row() const { return row_index; };

  [[nodiscard]] const std::size_t& i_column() const { return *column_index; };

private:
  size_type* column_index;
  pointer value;
  std::size_t row_index;
  matrix_type* matrix;

  friend Iterator_Matrix_Sparse_Element<Matrix_Sparse>;
  friend Iterator_Matrix_Sparse_Element<const Matrix_Sparse>;
};

//----------------------------------------------------------------------------------------------------------------------
// Stand Alone Sparse Matrix Operators
//----------------------------------------------------------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& stream, Matrix_Sparse& matrix) {
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
