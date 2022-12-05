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
//  File Name: matrix_sparse.h
//  Description: Contains the declaration and some definitions for the basic sparse matrix classes for Disa, at present
//               represented as a CSR matrix.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_MATRIX_SPARSE_H
#define DISA_MATRIX_SPARSE_H

#include "macros.h"
#include "scalar.h"
#include "vector_dense.h"

#include <numeric>
#include <vector>

namespace Disa {

// Forward declarations
template<typename _matrix_type>
class Matrix_Sparse_Row;

template<typename _matrix_type>
struct Iterator_Matrix_Sparse_Row;
template<typename _matrix_type>
struct Iterator_Matrix_Sparse_Element;

//----------------------------------------------------------------------------------------------------------------------
// Matrix Sparse
//----------------------------------------------------------------------------------------------------------------------

/**
 * @class Matrix_Sparse
 * @brief A efficient data structure which represents a matrix comprised of mostly zero entries. Follows compact storage
 * row (CSR) approach.
 *
 * @details
 * The Matrix_Sparse struct implements a mathematical matrix of nxm real numbers, but only stores memory for non-zero
 * entries.
 *
 * A CSR matrix stores three vectors; the first indicates the number of non-zeros per row (row_non_zero), this is
 * achieved by recording, for each element (row) in the row_non_zero vector the accumulated non-zeros upto that row.
 * With the last element being one greater than n, the number of rows, and recording the total number of non-zeros in
 * the matrix. The next vector stores the column indexes (column_index) for each non-zero matrix element, and for each
 * row the indexes are sorted in an ascending manner. The start of each row's columns in the column index vector is
 * given by the value in row_non_zero for that row. Finally, the last vector contains the values of each non-zero
 * element (element_value) and has a one-to-one correspondence with column_index vector. The last piece of data stored
 * is the number of columns in the matrix, this ensures correctness of mathematical operations.
 *
 * From a design perspective the class has two goals. The first is for the class to have a behaviour similar to that of
 * a dense matrix for sub-script operators and class iterators. This is achieved via a helper class and two iterator
 * classes which allow (range) loops over rows and then columns, further non-const subscript operators are fully able to
 * add non-zero entries to the matrix (just be aware this may invalidate iterators). Finally, the use of iterators
 * (when) accessed sequentially allows for efficient caching, the use of the element_iterator::i_row() and
 * element_iterator::i_column() functions also allow for the iterator to provide use for information about the
 * dereferenceable matrix entry.
 *
 * The second goal is to have have the class have a behaviour similar to a std::map structure, where the 'key' is the
 * row-column index pair for an entry. Thus functions, not normally associated with continuous structures (eg find,
 * contains, lower_bound, etc), are supported by the matrix.
 *
 * @note The user should be aware of the following:
 * 1. Iterators advance over non-zero data - i.e. an iterator will not be dereferenceable to a zero entry (when defined
 *    behaviour is expected).
 * 2. Unlike map or vector erasure functions do not effect the size of the matrix only the number of non-zeroes.
 * 2. Unlike map or vector insertion functions, size is only changed if the row or column index exceeds the matrix row
 *    or column size respectively, the number of non-zeroes will however always change.
 * 3. Non-const sub-script operators behave like a map in terms of overflow, i.e. the matrix is resized to accommodate
 *    entries added past the last row or column.
 * 4. As with all 'end() iterators', dereferencing should be considered undefined behaviour, with particular focus on
 *    a row end() iterator, which may dereference to the first entry on the next row (you have been warned).
 *
 * Future:
 * 1. If further sparse matrices are added to Disa then this class should be renamed to Matrix_Sparse_CSR
 */
class Matrix_Sparse {

public:
  // shorthands
  using matrix = Matrix_Sparse;
  using iterator = Iterator_Matrix_Sparse_Row<matrix>;
  using const_iterator = Iterator_Matrix_Sparse_Row<const matrix>;
  using iterator_element = Iterator_Matrix_Sparse_Element<matrix>;
  using const_iterator_element = Iterator_Matrix_Sparse_Element<const matrix>;

  //--------------------------------------------------------------------------------------------------------------------
  // Public Member Functions
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
                std::initializer_list<Scalar> value, std::size_t column);

  /**
   * @brief Default destructor.
   */
  ~Matrix_Sparse() = default;

  /**
   * @brief Performs a trivial deep copy of the assigned matrix.
   * @param[in] other The other sparse matrix.
   * @return Updated copy of this matrix.
   */
  matrix& operator=(const matrix& other) = default;

  //--------------------------------------------------------------------------------------------------------------------
  // Element Access
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Access specified element with bounds checking.
   * @param[in] i_row The row index of the element.
   * @param[in] i_column The column index of the element.
   * @return Value of the element.
   */
  [[nodiscard]] Scalar& at(const std::size_t& i_row, const std::size_t& i_column);

  /**
   * @brief Access specified element with bounds checking.
   * @param[in] i_row The row index of the element.
   * @param[in] i_column The column index of the element.
   * @return Value of the element.
   */
  [[nodiscard]] const Scalar& at(const std::size_t& i_row, const std::size_t& i_column) const;

  /**
   * @brief Subscript operator for access to a specified matrix row.
   * @param[in] i_row The row index of the element.
   * @return A sparse matrix row helper class, providing further operators for column access.
   */
  Matrix_Sparse_Row<matrix> operator[](const std::size_t& i_row);

  /**
   * @brief Subscript operator for access to a specified matrix row.
   * @param[in] i_row The row index of the element.
   * @return A constant sparse matrix row helper class, providing further operators for column access.
   */
  Matrix_Sparse_Row<const matrix> operator[](const std::size_t& i_row) const;

  /**
   * @brief Direct access to the underlying array of the sparse matrix.
   * @return tuple [pointer to non zero offset start, pointer to column index start, pointer to Scalar start].
   *
   * @note If empty all pointers will be nullptrs, if the size_non_zero is 0 the element and column index will be
   *       nullptrs.
   */
  std::tuple<std::size_t*, std::size_t*, Scalar*> inline data() noexcept {
    if(!empty() && !size_non_zero()) std::make_tuple(row_non_zero.data(), column_index.data(), element_value.data());
    else if(!size_non_zero()) return std::make_tuple(row_non_zero.data(), nullptr, nullptr);
    else return std::make_tuple(nullptr, nullptr, nullptr);
  }

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
  [[nodiscard]] inline bool empty() const noexcept { return row_non_zero.empty() || row_non_zero.size() < 2; };

  /**
   * @brief Returns the number of rows in the matrix.
   * @return The number of rows.
   */
  [[nodiscard]] inline std::size_t size_row() const noexcept {
    return !row_non_zero.empty() ? row_non_zero.size() - 1 : 0;
  };

  /**
   * @brief Returns the number of columns in the matrix.
   * @return The number of columns.
   */
  [[nodiscard]] inline std::size_t size_column() const noexcept { return column_size; };

  /**
   * @brief Returns the number of non-zeros in the matrix.
   * @return The number of non-zeros.
   */
  [[nodiscard]] inline std::size_t size_non_zero() const noexcept { return column_index.size(); };

  /**
   * @brief Returns the number of rows and columns in the matrix. If rows are 0, columns are 0.
   * @return Pair containing [rows, columns].
   */
  [[nodiscard]] inline std::pair<std::size_t, std::size_t> size() const noexcept {
    return std::make_pair(size_row(), size_column());
  };

  /**
   * @brief Reserves storage for the matrix.
   * @param[in] row The number of rows to reserve for.
   * @param[in] non_zero The number of non-zeros to reserve. Note this is not the column size.
   *
   * @note The number of row offsets which are reserved will be one greater than the row size passed.
   */
  inline void reserve(const std::size_t& row, const std::size_t& non_zero) noexcept {
    row_non_zero.reserve(row + 1);
    column_index.reserve(non_zero);
    element_value.reserve(non_zero);
  };

  /**
   * @brief Returns the number of row offsets and non-zeros entries that can be held in currently allocated storage.
   * @return [number of rows offsets, number of non-zeros entries.]
   *
   * @note The number of row offsets (for CSR matrices) is always one greater than the actual matrix row size.
   */
  [[nodiscard]] inline std::pair<std::size_t, std::size_t> capacity() const noexcept {
    return std::make_pair(row_non_zero.capacity(), column_index.capacity());
  };

  /**
   * @brief reduces memory usage by the matrix by freeing unused memory for both rows and non-zeros
   */
  inline void shrink_to_fit() noexcept {
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
  inline void clear() noexcept {
    row_non_zero.clear();
    column_index.clear();
    element_value.clear();
    column_size = 0;
  };

  /**
   * @brief Inserts an value with the parsed value at [i_row, i_column], if an value exists nothing occurs.
   * @param[in] i_row The i_row index to the insert value.
   * @param[in] i_column The i_column index to the insert value.
   * @param[in] value The value to insert.
   * @return [iterator to the inserted or existing element, true if insertion took place, else false].
   */
  std::pair<iterator_element, bool> insert(const std::size_t& i_row, const std::size_t& i_column, const Scalar& value);

  /**
   * @brief Inserts or updates an value with the parsed value at [i_row, i_column].
   * @param[in] i_row The i_row index to the insert value.
   * @param[in] i_column The i_column index to the insert value.
   * @param[in] value The value to insert.
   * @return [iterator to the inserted or existing element, true if insertion took place, else false].
   */
  std::pair<iterator_element, bool>
  insert_or_assign(const std::size_t& i_row, const std::size_t& i_column, const Scalar& value);

  /**
   * @brief Erases an value at the specified row and column.
   * @param[in] iter_element The iterator to the value to delete.
   * @return The iterator to the value after the deleted value.
   *
   * @note Undefined behaviour exists if the element does not exist, i.e. the iterator must be dereferenceable.
   */
  iterator_element erase(const Iterator_Matrix_Sparse_Element<matrix>& iter_element);

  /**
   * @brief Changes the number of rows and columns of the matrix.
   * @param[in] row Number of rows to resized the matrix to.
   * @param[in] column Number of columns to resized the matrix to.
   *
   * @note Does not effect number of non-zeros, unless the new row and column sizes reduce the size of the matrix.
   */
  void resize(const std::size_t& row, const std::size_t& column);

  /**
   * @brief Swaps the contents of the matrix with the parsed matrix
   * @param[in,out] other The other matrix, this matrix will obtain the other's value and visa versa.
   */
  inline void swap(matrix& other) noexcept {
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
   * @note If i_row >= row_size() end() is returned, if no element is found *(begin + i_row).end() is returned.
   */
  iterator_element find(const std::size_t& i_row, const std::size_t& i_column);

  /**
   * @brief Finds the value iterator to the matrix value at a row and column.
   * @param[in] i_row The row index to the element.
   * @param[in] i_column The column index to the element.
   * @return constant element iterator.
   *
   * @note If i_row >= row_size() end() is returned, if no element is found *(begin + i_row).end() is returned.
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
   * @brief Returns an element iterator to the first element not less than the given key.
   * @param[in] i_row The row index to the element.
   * @param[in] i_column The column index of the element.
   * @return Iterator pointing to the first element that is not less than key.
   *
   * @note If i_row >= row_size() end() is returned, if no element is found *(begin + i_row).end() is returned.
   */
  iterator_element lower_bound(const std::size_t& i_row, const std::size_t& i_column);

  /**
   * @brief Returns an element iterator to the first element not less than the given key.
   * @param[in] i_row The row index to the element.
   * @param[in] i_column The column index of the element.
   * @return Const iterator pointing to the first element that is not less than key.
   *
   * @note If i_row >= row_size() end() is returned, if no element is found *(begin + i_row).end() is returned.
   */
  [[nodiscard]] const_iterator_element lower_bound(const std::size_t& i_row, const std::size_t& i_column) const;

  //--------------------------------------------------------------------------------------------------------------------
  // Mathematical Assignment Operators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Multiplies the matrix by a scalar, A' = A*b, where A is the matrix and b is the scalar.
   * @param[in] scalar Scalar value, b, to multiply the matrix by.
   * @return Updated matrix (A').
   */
  inline matrix& operator*=(const Scalar& scalar) {
    FOR_EACH_REF(element, element_value) element *= scalar;
    return *this;
  }

  /**
   * @brief Divides the matrix by a scalar, A' = A/b, where A is the matrix and b is a scalar.
   * @param[in] scalar Scalar value, b, to multiply the vector by.
   * @return Updated matrix (A').
   *
   * @note Division by zero is left to the user to handle.
   */
  inline matrix& operator/=(const Scalar& scalar) {
    FOR_EACH_REF(element, element_value) element /= scalar;
    return *this;
  }

  /**
   * @brief Addition of a second matrix, A' = A + B, where A and B are matrices.
   * @param[in] other The second matrix, B, to add.
   * @return Updated matrix (A').
   */
  inline matrix& operator+=(const matrix& other) { return matrix_arithmetic<true>(other); };

  /**
   * @brief Subtraction by a second matrix, A' = A - B, where A and B are matrices.
   * @param[in] other The second matrix, B, to subtract.
   * @return Updated matrix (A').
   */
  inline matrix& operator-=(const matrix& other) { return matrix_arithmetic<false>(other); };

  /**
   * @brief Multiplies this matrix by another, A' = A*B, where A and B are matrices.
   * @param[in] other The second other, B, to multiply by.
   * @return Updated matrix (A').
   */
  matrix& operator*=(const matrix& other);

  //--------------------------------------------------------------------------------------------------------------------
  // Private Members
  //--------------------------------------------------------------------------------------------------------------------

private:
  std::vector<std::size_t> row_non_zero;    //!< Total non-zero elements in the matrix for each row (size is one greater than number of rows).
  std::vector<std::size_t> column_index;    //!< The column index for each non-zero value, corresponds to value.
  std::vector<Scalar> element_value;        //!< The each non-zero value value, corresponds to column_index.
  std::size_t column_size{0};               //!< The number of columns of the matrix (used to check validity of operations).

  //--------------------------------------------------------------------------------------------------------------------
  // Private Member Functions
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Converts and formats a row and colum index to string.
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
   * @brief Performs addition and subtraction operations between this matrix and another, i.e. A' = A + B or A' = A - B.
   * @tparam _is_add If true, addition is performed else subtraction is performed.
   * @param[in] other The other matrix B.
   * @return Updated matrix (A').
   */
  template<bool _is_add>
  Matrix_Sparse& matrix_arithmetic(const Matrix_Sparse& other);


  // Friend types and functions.
  friend class Matrix_Sparse_Row<matrix>;
  friend class Matrix_Sparse_Row<const matrix>;
  friend struct Iterator_Matrix_Sparse_Row<matrix>;
  friend struct Iterator_Matrix_Sparse_Row<const matrix>;
  friend struct Iterator_Matrix_Sparse_Element<matrix>;
  friend struct Iterator_Matrix_Sparse_Element<const matrix>;
  friend std::ostream& operator<<(std::ostream& stream, matrix& matrix);
};

//----------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Row Helper
//----------------------------------------------------------------------------------------------------------------------

/**
 * @class Matrix_Sparse_Row
 * @brief Helper class, enables column iteration support.
 * @tparam _matrix_type const or non-const sparse matrix.
 */
template<typename _matrix_type>
class Matrix_Sparse_Row {

public:
  // shorthands
  using matrix_type = _matrix_type;
  using iterator = Iterator_Matrix_Sparse_Element<_matrix_type>;
  using const_iterator = Iterator_Matrix_Sparse_Element<const Matrix_Sparse>;

  //--------------------------------------------------------------------------------------------------------------------
  // Public Member Functions
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Copy constructor.
   * @tparam _matrix Only enabled if this helper is const.
   * @param[in] other The other const helper.
   */
  template<typename _matrix, typename std::enable_if_t<std::is_const_v<_matrix>, bool> = true>
  constexpr explicit Matrix_Sparse_Row(const Matrix_Sparse_Row<const Matrix_Sparse>& other)
    : matrix(other.matrix), row_index(other.row_index) {};

  /**
   * @brief Copy/Converter constructor from non-const.
   * @param[in] other The other non-const helper.
   */
  constexpr explicit Matrix_Sparse_Row(const Matrix_Sparse_Row<Matrix_Sparse>& other)
    : matrix(other.matrix), row_index(other.row_index) {};

  /**
   * @brief Raw constructor, directly constructs private data.
   * @param[in] i_row Row index of the matrix the helper is to 'pointing to'.
   * @param[in] sparse_matrix Pointer to the underlying sparse matrix.
   */
  constexpr Matrix_Sparse_Row(std::size_t i_row, matrix_type* sparse_matrix) noexcept
    : matrix(sparse_matrix), row_index(i_row) {};

  //--------------------------------------------------------------------------------------------------------------------
  // Element Access
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Subscript operator to access specified matrix element.
   * @tparam _matrix Only enabled if this helper is non-const.
   * @param[in] i_column The column index of the element.
   * @return The sparse matrix element.
   *
   * Note if the element does not exist it is inserted.
   */
  template<typename _matrix = _matrix_type, typename std::enable_if_t<!std::is_const_v<_matrix>, bool> = true>
  constexpr Scalar& operator[](const std::size_t& i_column) {
    auto iter_element = matrix->lower_bound(row_index, i_column);
    if(iter_element == end() || i_column != iter_element.i_column())
      iter_element = matrix->insert(row_index, i_column, Scalar()).first;
    return *iter_element;
  };

  /**
   * @brief Subscript operator to access specified matrix const element.
   * @param[in] i_column The column index of the element.
   * @return The const sparse matrix element.
   *
   * Note if the element does not exist undefined behaviour occurs (caught in debug).
   */
  constexpr const Scalar& operator[](const std::size_t& i_column) const {
    ASSERT_DEBUG(matrix->contains(row_index, i_column),
                 "Trying to access a zero element at [" + matrix->row_column(row_index, i_column) + "].");
    return *matrix->find(row_index, i_column);
  };

  //--------------------------------------------------------------------------------------------------------------------
  // Iterators
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Returns an iterator to the beginning of the row's elements.
   * @return An Iterator_Matrix_Sparse_Element<Matrix_Sparse> iterator.
   */
  iterator begin();

  /**
   * @brief Returns an iterator to the end of the row's elements.
   * @return An Iterator_Matrix_Sparse_Element<Matrix_Sparse> iterator.
   */
  iterator end();

  /**
   * @brief Returns a const iterator to the beginning of the row's elements.
   * @return An Iterator_Matrix_Sparse_Element<const Matrix_Sparse> iterator.
   */
  [[nodiscard]] const_iterator begin() const;

  /**
   * @brief Returns a const iterator to the end of the row's elements.
   * @return An Iterator_Matrix_Sparse_Element<const Matrix_Sparse> iterator.
   */
  [[nodiscard]] const_iterator end() const;

  /**
   * @brief Returns a const iterator to the beginning of the row's elements.
   * @return An Iterator_Matrix_Sparse_Element<const Matrix_Sparse> iterator.
   */
  [[nodiscard]] const_iterator cbegin() const;

  /**
   * @brief Returns a const iterator to the end of the row's elements.
   * @return An Iterator_Matrix_Sparse_Element<const Matrix_Sparse> iterator.
   */
  [[nodiscard]] const_iterator cend() const;

  //--------------------------------------------------------------------------------------------------------------------
  // Private Members
  //--------------------------------------------------------------------------------------------------------------------

private:
  _matrix_type* const matrix {nullptr};                              //!< Pointer to the associated sparse matrix.
  std::size_t row_index {std::numeric_limits<std::size_t>::max()};   //!< Current row index of the iterator.

  // Friend types.
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
 * @struct Iterator_Matrix_Sparse_Row
 * @brief Iterator to advance over rows of a CSR sparse matrix.
 * @tparam _matrix_type const or non-const sparse matrix.
 */
template<typename _matrix_type>
struct Iterator_Matrix_Sparse_Row {

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using matrix_type = _matrix_type;
  using value_type = Matrix_Sparse_Row<_matrix_type>;
  using pointer = value_type*;
  using reference = value_type&;

  //--------------------------------------------------------------------------------------------------------------------
  // Public Member Functions
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Constructor from helper class.
   * @param[in] row Sparse Matrix Row helper to initialise from.
   */
  explicit constexpr Iterator_Matrix_Sparse_Row(const value_type& row) : matrix_row(row) {};

  /**
   * @brief Raw constructor, directly constructs the Sparse Matrix Row helper private data.
   * @param[in] i_row Row index of the matrix the helper is to 'pointing to'.
   * @param[in] sparse_matrix Pointer to the underlying sparse matrix.
   */
  constexpr Iterator_Matrix_Sparse_Row(std::size_t i_row, matrix_type* sparse_matrix)
    : matrix_row(value_type(i_row, sparse_matrix)) {};

  //--------------------------------------------------------------------------------------------------------------------
  // I/O Iterator Support
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Iterator equal to comparison operator.
   * @param[in] other The other iterator to compare against.
   * @return True if the iterators are equal, else false.
   */
  constexpr bool operator==(const Iterator_Matrix_Sparse_Row& other) const {
    return matrix_row.matrix == other.matrix_row.matrix && matrix_row.row_index == other.matrix_row.row_index;
  }

  /**
   * @brief Iterator not equal to comparison operator.
   * @param[in] other The other iterator to compare against.
   * @return True if the iterators are not equal, else false.
   */
  constexpr bool operator!=(const Iterator_Matrix_Sparse_Row& other) const {
    return !(*this == other);
  }

  /**
   * @brief Iterator indirection member access operator.
   * @tparam _matrix Only enabled if this iterator is for a non-const helper.
   * @return The underlying non-const sparse matrix row helper class.
   */
  template<typename _matrix = _matrix_type, typename std::enable_if_t<!std::is_const_v<_matrix>, bool> = true>
  constexpr reference operator*() { return matrix_row; }

  /**
   * @brief Iterator indirection member access operator.
   * @return The underlying const sparse matrix row helper class.
   */
  constexpr Matrix_Sparse_Row<const Matrix_Sparse> operator*() const {
    return Matrix_Sparse_Row<const Matrix_Sparse>(matrix_row);
  }

  /**
   * @brief Iterator member of pointer member access operator.
   * @tparam _matrix Only enabled if this iterator is for a non-const helper.
   * @return Pointer to the helper class.
   */
  template<typename _matrix = _matrix_type, typename std::enable_if_t<!std::is_const_v<_matrix>, bool> = true>
  constexpr pointer operator->() { return &matrix_row; }

  //--------------------------------------------------------------------------------------------------------------------
  // Forward Iterator Support
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Iterator pre-increment operator.
   * @return Updated iterator, row index advanced by one.
   */
  constexpr Iterator_Matrix_Sparse_Row& operator++() {
    matrix_row.row_index++;
    return *this;
  }

  /**
   * @brief Iterator post-increment operator.
   * @return Iterator 'state' prior to the row index being advanced by one.
   */
  constexpr Iterator_Matrix_Sparse_Row operator++(int) {
    Iterator_Matrix_Sparse_Row iter = *this;
    ++matrix_row.row_index;
    return iter;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // BiDirectional Iterator Support
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Iterator pre-decrement operator.
   * @return Updated iterator, row index reduced by one.
   */
  constexpr Iterator_Matrix_Sparse_Row& operator--() {
    matrix_row.row_index--;
    return *this;
  }

  /**
   * @brief Iterator post-decrement operator.
   * @return Iterator 'state' prior to the row index being reduced by one.
   */
  Iterator_Matrix_Sparse_Row operator--(int) {
    Iterator_Matrix_Sparse_Row iter = *this;
    --matrix_row.row_index;
    return iter;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Random Access Support
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Iterator addition arithmetic operator.
   * @param[in] offset The amount to advance the iterator by.
   * @return Updated iterator, row index advanced by the offset amount.
   */
  constexpr Iterator_Matrix_Sparse_Row operator+(difference_type offset) {
    matrix_row.row_index += offset;
    return *this;
  }

  /**
   * @brief Iterator subtraction arithmetic operator.
   * @param[in] offset The amount to reduce the iterator by.
   * @return Updated iterator, row index reduced by the offset amount.
   */
  constexpr Iterator_Matrix_Sparse_Row operator-(difference_type offset) {
    matrix_row.row_index -= offset;
    return *this;
  }

  /**
   * @brief Iterator addition assignment operator.
   * @param[in] offset The amount to advance the iterator by.
   * @return Updated iterator, row index advanced by the offset amount.
   */
  Iterator_Matrix_Sparse_Row& operator+=(difference_type offset) {
    matrix_row.row_index += offset;
    return *this;
  };

  /**
   * @brief Iterator subtraction assignment operator.
   * @param[in] offset The amount to reduce the iterator by.
   * @return Updated iterator, row index reduced by the offset amount.
   */
  constexpr Iterator_Matrix_Sparse_Row& operator-=(difference_type offset) {
    matrix_row.row_index -= offset;
    return *this;
  };

  /**
   * @brief Iterator subscript operator
   * @tparam _matrix Only enabled if this iterator is for a non-const helper.
   * @param[in] offset The relative amount to advance the iterator by.
   * @return A non-const sparse matrix row helper with a row index advanced by the offset (relative to this iterator).
   */
  template<typename _matrix = _matrix_type, typename std::enable_if_t<!std::is_const_v<_matrix>, bool> = true>
  constexpr value_type operator[](const difference_type& offset) {
    return value_type(*std::next(*this, offset));
  }

  /**
   * @brief Iterator subscript operator
   * @tparam _matrix Only enabled if this iterator is for a non-const helper.
   * @param[in] offset The relative amount to advance the iterator by.
   * @return A const sparse matrix row helper with a row index advanced by the offset (relative to this iterator).
   */
  constexpr Matrix_Sparse_Row<const Matrix_Sparse> operator[](const difference_type& offset) const {
    return Matrix_Sparse_Row<const Matrix_Sparse>(*std::next(*this, offset));
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Private Members
  //--------------------------------------------------------------------------------------------------------------------

private:
  value_type matrix_row;    //!< Contained row_index iterator for iterator operations
};

//----------------------------------------------------------------------------------------------------------------------
// Iterators for Sparse Matrix Elements
//----------------------------------------------------------------------------------------------------------------------

/**
 * @struct Iterator_Matrix_Sparse_Element
 * @brief Iterator to advance over elements of a row of a CSR sparse matrix.
 * @tparam _matrix_type const or non-const sparse matrix.
 */
template<typename _matrix_type>
struct Iterator_Matrix_Sparse_Element {

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using matrix_type = _matrix_type;
  using value_type = std::conditional_t<std::is_const_v<_matrix_type>, const Scalar, Scalar>;
  using size_type = std::conditional_t<std::is_const_v<_matrix_type>, const std::size_t, std::size_t>;
  using pointer = value_type*;
  using reference = value_type&;

  //--------------------------------------------------------------------------------------------------------------------
  // Public Member Functions
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Raw constructor, directly constructs the private data.
   * @param[in] sparse_matrix Pointer to the sparse matrix this iterator is associated with.
   * @param[in] i_row The row index the dereferenceable matrix element is in.
   * @param[in] i_column The column index the dereferenceable matrix element is in.
   * @param[in] value Pointer to the the dereferenceable matrix element.
   */
  constexpr Iterator_Matrix_Sparse_Element(matrix_type* sparse_matrix, const std::size_t& i_row, size_type* i_column,
                                           pointer value)
                                           : row_index(i_row), matrix(sparse_matrix), column_index(i_column),
                                           value(value) {}

  /**
   * @brief Returns the row index for the dereferenceable matrix element.
   * @return Row index of the matrix element.
   *
   * @note Undefined behaviour if the iterator is not dereferenceable.
   */
  [[nodiscard]] constexpr const std::size_t& i_row() const { return row_index; };

  /**
   * @brief Returns the column index for the dereferenceable matrix element.
   * @return Column index of the matrix element.
   *
   * @note Undefined behaviour if the iterator is not dereferenceable.
   */
  [[nodiscard]] constexpr const std::size_t& i_column() const { return *column_index; };

  //--------------------------------------------------------------------------------------------------------------------
  // I/O Iterator Support
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Iterator equal to comparison operator.
   * @param[in] other The other iterator to compare against.
   * @return True if the iterators are equal, else false.
   */
  constexpr bool operator==(const Iterator_Matrix_Sparse_Element& other) const {
    return matrix == other.matrix && row_index == other.row_index && column_index == other.column_index &&
           value == other.value;
  }

  /**
   * @brief Iterator not equal to comparison operator.
   * @param[in] other The other iterator to compare against.
   * @return True if the iterators are not equal, else false.
   */
  constexpr bool operator!=(const Iterator_Matrix_Sparse_Element& other) const {
    return !(*this == other);
  }

  /**
   * @brief Iterator indirection member access operator.
   * @tparam _matrix Only enabled if this iterator is for a non-const matrix.
   * @return The underlying non-const matrix element.
   */
  template<typename _matrix = _matrix_type, typename std::enable_if_t<!std::is_const_v<_matrix>, bool> = true>
  constexpr reference operator*() { return *value; }

  /**
   * @brief Iterator indirection member access operator.
   * @return The underlying const matrix element.
   */
  constexpr const Scalar& operator*() const { return *value; }

  //--------------------------------------------------------------------------------------------------------------------
  // Forward Iterator Support
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Iterator pre-increment operator.
   * @return Updated iterator, column index advanced to the next non-zero element.
   */
  constexpr Iterator_Matrix_Sparse_Element& operator++() {
    ++column_index;
    ++value;
    return *this;
  }

  /**
   * @brief Iterator post-decrement operator.
   * @return Iterator 'state' prior to the column index being advanced to the next non-zero element.
   */
  Iterator_Matrix_Sparse_Element operator++(int) {
    Iterator_Matrix_Sparse_Element iter = *this;
    ++(*this);
    return iter;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // BiDirectional Iterator Support
  //--------------------------------------------------------------------------------------------------------------------


  /**
   * @brief Iterator pre-decrement operator.
   * @return Updated iterator, column index reduced to the previous non-zero element.
   */
  constexpr Iterator_Matrix_Sparse_Element& operator--() {
    --column_index;
    --value;
    return *this;
  }


  /**
   * @brief Iterator post-decrement operator.
   * @return Iterator 'state' prior to the column index being reduced to the previous non-zero element.
   */
  Iterator_Matrix_Sparse_Element operator--(int) {
    Iterator_Matrix_Sparse_Element iter = *this;
    --(*this);
    return iter;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Random Access Support
  //--------------------------------------------------------------------------------------------------------------------

  /**
   * @brief Iterator addition arithmetic operator.
   * @param[in] offset The amount to advance the iterator by.
   * @return Updated iterator, column index of non-zero elements advanced by the offset amount.
   */
  constexpr difference_type operator-(const Iterator_Matrix_Sparse_Element other) {
    return column_index - other.column_index;
  }

  /**
   * @brief Iterator subtraction arithmetic operator.
   * @param[in] offset The amount to reduce the iterator by.
   * @return Updated iterator, column index of non-zero elements reduced by the offset amount.
   */
  constexpr Iterator_Matrix_Sparse_Element& operator+(const difference_type& offset) {
    column_index + offset;
    value + offset;
    return *this;
  }

  /**
   * @brief Iterator addition assignment operator.
   * @param[in] offset The amount to advance the iterator by.
   * @return Updated iterator, column index of non-zero elements advanced by the offset amount.
   */
  constexpr Iterator_Matrix_Sparse_Element& operator+=(difference_type offset) {
    this + offset;
    return *this;
  };

  /**
   * @brief Iterator subtraction assignment operator.
   * @param[in] offset The amount to reduce the iterator by.
   * @return Updated iterator, column index of non-zero elements reduced by the offset amount.
   */
  constexpr Iterator_Matrix_Sparse_Element& operator-=(difference_type offset) {
    this - offset;
    return *this;
  };

  /**
   * @brief Iterator subscript operator
   * @tparam _matrix Only enabled if this iterator is for a non-const spare matrix.
   * @param[in] offset The relative amount to advance the iterator by.
   * @return A non-const sparse matrix element with a non-zero column index advanced by the offset (relative to this iterator).
   */
  template<typename _matrix = _matrix_type, typename std::enable_if_t<!std::is_const_v<_matrix>, bool> = true>
  constexpr reference operator[](const difference_type& i_advance) { return *(value + i_advance); }


  /**
   * @brief Iterator subscript operator
   * @param[in] offset The relative amount to advance the iterator by.
   * @return A const sparse matrix element with a non-zero column index advanced by the offset (relative to this iterator).
   */
  constexpr const Scalar& operator[](const difference_type& i_advance) const { return *(value + i_advance); }

  //--------------------------------------------------------------------------------------------------------------------
  // Private Members
  //--------------------------------------------------------------------------------------------------------------------

private:
  size_type* column_index;    //!< The column index of the element this iterator dereferences to.
  pointer value;              //!< The matrix element this iterator dereferences to.
  std::size_t row_index;      //!< The row index in the sparse matrix this iterator is associated with.
  matrix_type* matrix;        //!< The sparse matrix this iterator is associated with.

  // Friend types.
  friend Iterator_Matrix_Sparse_Element<Matrix_Sparse>;
  friend Iterator_Matrix_Sparse_Element<const Matrix_Sparse>;
};

//----------------------------------------------------------------------------------------------------------------------
// Stand Alone Sparse Matrix Operators
//----------------------------------------------------------------------------------------------------------------------
// Arithmetic Operators
//----------------------------------------------------------------------------------------------------------------------

/**
 * @brief Multiplies a sparse matrix by a scalar, C = b*A, where A, and C are matrices and b is a scalar
 * @param[in] scalar The scalar value, b, to multiply the matrix by.
 * @param[in] matrix The sparse matrix, A, to be multiplied.
 * @return New sparse matrix, C.
 */
inline Matrix_Sparse operator*(const Scalar& scalar, Matrix_Sparse matrix) {
  return matrix *= scalar;
}

/**
 * @brief Divides a sparse matrix by a scalar, C = A/b, where A, and C are sparse matrices and b is a scalar
 * @param[in] matrix The sparse matrix, A, to be divided.
 * @param[in] scalar The scalar value, b, to divide the matrix by.
 * @return New sparse matrix, C.
 */
inline Matrix_Sparse operator/(Matrix_Sparse matrix, const Scalar& scalar) {
  return matrix /= scalar;
}

/**
 * @brief Multiplies a sparse matrix and vector, c = A*b, where A is a sparse matrix and c and b are vectors.
 * @tparam _size The number of column of the A matrix, dynamic/static.
 * @param[in] matrix The sparse matrix, A, to be multiplied.
 * @param[in] vector The vector, b, to multiply the matrix by.
 * @return New vector, c.
 *
 * @note at present for static vectors the matrix must be square.
 */
template<std::size_t _size>
Vector_Dense<_size> operator*(const Matrix_Sparse& matrix, const Vector_Dense<_size>& vector) {
  ASSERT_DEBUG(matrix.size_column() == vector.size(),
               "Incompatible vector-matrix dimensions, " + std::to_string(matrix.size_row()) + "," +
               std::to_string(matrix.size_column()) + " vs. " + std::to_string(vector.size()) + ".");
  ASSERT_DEBUG(!_size || matrix.size_row() == vector.size(), "For static vectors the matrix must be square.");
  return Vector_Dense<_size>([&](const std::size_t i_row, Scalar value = 0) {
    FOR_ITER(iter, *(matrix.begin() + i_row)) value += *iter*vector[iter.i_column()];
    return value; }, matrix.size_row());
}

/**
 * @brief Adds two sparse matrices together, C = A + B, where A, B, and C are sparse matrices
 * @param[in] matrix_0 The first sparse matrix, A, to add.
 * @param[in] matrix_1 The second sparse matrix, B, to add.
 * @return New sparse matrix, C.
 */
inline Matrix_Sparse operator+(Matrix_Sparse matrix_0, const Matrix_Sparse& matrix_1) {
  ASSERT_DEBUG(matrix_0.size() == matrix_1.size(),
               "Incompatible matrix dimensions, " + std::to_string(matrix_0.size_row()) + "," +
               std::to_string(matrix_0.size_column()) + " vs. " + std::to_string(matrix_1.size_row()) + "," +
               std::to_string(matrix_1.size_column()) + ".");
  matrix_0 += matrix_1;
  return matrix_0;
}

/**
 * @brief Subtracts one sparse matrix from another, C = A - B, where A, B, and C are sparse matrices
 * @param[in] matrix_0 The sparse matrix, A, to be subtracted from.
 * @param[in] matrix_1 The sparse matrix, B, to subtract by.
 * @return New sparse matrix, C.
 */
inline Matrix_Sparse operator-(Matrix_Sparse matrix_0, const Matrix_Sparse& matrix_1) {
  matrix_0 -= matrix_1;
  return matrix_0;
}

/**
 * @brief Multiples two sparse matrices together, C = A*B, where A, B, and C are sparse matrices.
 * @param[in] matrix_0 The left sparse matrix, A, to multiply.
 * @param[in] matrix_1 The right sparse matrix, B, to multiply.
 * @return New sparse matrix, C.
 */
inline Matrix_Sparse operator*(Matrix_Sparse matrix_0, const Matrix_Sparse& matrix_1) {
  matrix_0 *= matrix_1;
  return matrix_0;
}

/**
 * @brief Output shift operator - to write to console.
 * @param[in, out] stream Stream object to write to.
 * @param[in] matrix The sparse matrix to write out.
 * @return The inputted stream.
 *
 * todo: properly format - this is just here to help occasional debugging.
 */
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
