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

#ifndef DISA_MATRIX_SPARSE_NEW_H
#define DISA_MATRIX_SPARSE_NEW_H

#include <numeric>
#include <span>
#include <vector>

#include "matrix_sparse_data.hpp"
#include "scalar.hpp"

namespace Disa {

template<typename _entry_type, typename _index_type>
class Matrix_Sparse;

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Element and Iterators
// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Element
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @struct Matrix_Sparse_Element
 * @brief Represents an element in a sparse (CSR) matrix.  
 * @tparam _value_type The type of the value in the matrix element.
 * @tparam _index_type The type of the row and column indices.
 * 
 * @details
 * The class represents a sparse matrix element for the CSR matrix. It provides read-write access to the element value,
 * however the column value is read only, since updating it may cause the CSR ordering to be invalidated. Some 
 * additional operators are provided for comparison and assignment.
 */
template<typename _value_type, typename _index_type>
class Matrix_Sparse_Element {

 public:
  using index_type = const _index_type;
  using const_pointer_index = const _index_type*;
  using const_reference_index = const _index_type&;

  using value_type = _value_type;
  using pointer_value = _value_type*;
  using reference_value = _value_type&;
  using const_pointer_value = const _value_type*;
  using const_reference_value = const _value_type&;

  using base_type = Matrix_Sparse_Element<value_type, _index_type>;

  /**
   * @brief Default constructor.
   */
  constexpr Matrix_Sparse_Element() noexcept = default;

  /**
   * @brief Destructor.
   */
  ~Matrix_Sparse_Element() noexcept = default;

  /**
   * @brief Constructor that initializes the element with the given column pointer and value pointer.
   * 
   * @param column The column pointer of the element.
   * @param value The value pointer of the element.
   */
  constexpr Matrix_Sparse_Element(const_pointer_index column, pointer_value value) noexcept
      : ptr_column(column), ptr_value(value) {}

  /**
   * @brief Copy constructor (deleted).
   * @note Cannot copy an element since it would invalidate the CSR ordering.
   */
  Matrix_Sparse_Element(const Matrix_Sparse_Element& other) = delete;

  /**
   * @brief Returns a reference to the value of the element.
   * 
   * @return A reference to the value of the element.
   */
  [[nodiscard]] constexpr reference_value value() noexcept { return *ptr_value; }

  /**
   * @brief Returns the column index of the element.
   * 
   * @return The column index of the element.
   */
  [[nodiscard]] constexpr const_reference_index i_column() const noexcept { return *ptr_column; }

  /**
   * @brief Returns a const reference to the value of the element.
   * 
   * @return A const reference to the value of the element.
   */
  [[nodiscard]] constexpr const_reference_value value() const noexcept { return *ptr_value; }

  /**
   * @brief Equality comparison operator.
   * 
   * @param that The element to compare with.
   * @return True if the elements (column, value) are equal, false otherwise.
   */
  [[nodiscard]] constexpr bool operator==(const base_type& that) const noexcept {
    return ptr_column == that.ptr_column && ptr_value == that.ptr_value;
  }

  /**
   * @brief Inequality comparison operator.
   * 
   * @param that The element to compare with.
   * @return True if the elements are not equal, false otherwise.
   */
  [[nodiscard]] constexpr bool operator!=(const base_type& that) const noexcept { return !(*this == that); }

  /**
   * @brief Copy assignment operator (deleted).
   * @note Cannot copy assign an element since it would invalidate the CSR ordering.
   */
  Matrix_Sparse_Element& operator=(const Matrix_Sparse_Element& that) = delete;

 private:
  const_pointer_index ptr_column{nullptr}; /**< The column of the CSR element. */
  pointer_value ptr_value{nullptr};        /**< The value of the CSR element. */
};

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Element Iterators
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @struct Base_Iterator_Matrix_Sparse_Element
 * @brief Base iterator for Matrix_Sparse_Element.
 * @tparam _entry_type The type of the entry in the matrix element.
 * @tparam _index_type The type of the indices.
 * @tparam _is_const Whether this is a const iterator or not.
 * 
 * This structure provides a bidirectional iterator for Matrix_Sparse_Element. It can be used to iterate over the 
 * elements of a sparse matrix in CSR format, typically row-wise although not exclusively.
 */
template<typename _entry_type, typename _index_type, bool _is_const>
struct Base_Iterator_Matrix_Sparse_Element {
 public:
  // Iterator traits
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = Matrix_Sparse_Element<_entry_type, _index_type>;
  using const_value_type = Matrix_Sparse_Element<const _entry_type, const _index_type>;
  using pointer = std::conditional_t<_is_const, const_value_type*, value_type*>;
  using reference = std::conditional_t<_is_const, const_value_type&, value_type&>;

  // type definitions
  using index_type = _index_type;
  using entry_type = _entry_type;
  using pointer_index = std::conditional_t<_is_const, const _index_type*, _index_type*>;
  using pointer_entry = std::conditional_t<_is_const, const _entry_type*, _entry_type*>;
  using iterator_type = Base_Iterator_Matrix_Sparse_Element<entry_type, index_type, _is_const>;

  /**
   * @brief Default constructor.
   */
  constexpr Base_Iterator_Matrix_Sparse_Element() noexcept = default;

  /**
   * @brief Destructor.
   */
  ~Base_Iterator_Matrix_Sparse_Element() noexcept = default;

  /**
   * @brief Constructor that initializes the iterator with column and entry pointers.
   * 
   * @param column Pointer to the column index.
   * @param entry Pointer to the entry value.
   */
  constexpr Base_Iterator_Matrix_Sparse_Element(pointer_index column, pointer_entry entry) noexcept
      : ptr_column(column), ptr_entry(entry) {}

  /**
   * @brief Dereference operator.
   * 
   * @return A Matrix_Sparse_Element object.
   */
  [[nodiscard]] constexpr auto operator*() const noexcept {
    if constexpr(_is_const) return const_value_type(ptr_column, ptr_entry);
    else return value_type(ptr_column, ptr_entry);
  }

  /**
   * @brief Pre-increment operator.
   * 
   * @return Reference to the incremented iterator.
   */
  constexpr iterator_type& operator++() noexcept {
    ++ptr_column;
    ++ptr_entry;
    return *this;
  }

  /**
   * @brief Post-increment operator.
   * 
   * @return Copy of the iterator before increment.
   */
  constexpr iterator_type operator++(int) noexcept {
    iterator_type old = *this;
    ++(*this);
    return old;
  }

  /**
   * @brief Pre-decrement operator.
   * 
   * @return Reference to the decremented iterator.
   */
  constexpr iterator_type& operator--() noexcept {
    --ptr_column;
    --ptr_entry;
    return *this;
  }

  /**
   * @brief Post-decrement operator.
   * 
   * @return Copy of the iterator before decrement.
   */
  constexpr iterator_type operator--(int) noexcept {
    iterator_type old = *this;
    --(*this);
    return old;
  }

  /**
   * @brief Equality comparison operator.
   * 
   * @param that The iterator to compare with.
   * @return True if the iterators are equal, false otherwise.
   */
  [[nodiscard]] constexpr bool operator==(const iterator_type& that) const noexcept {
    return ptr_column == that.ptr_column && ptr_entry == that.ptr_entry;
  }

  /**
   * @brief Inequality comparison operator.
   * 
   * @param that The iterator to compare with.
   * @return True if the iterators are not equal, false otherwise.
   */
  [[nodiscard]] constexpr bool operator!=(const iterator_type& that) const noexcept { return !(*this == that); }

 private:
  pointer_index ptr_column{nullptr};
  pointer_entry ptr_entry{nullptr};
};

/**
 * @typedef Iterator_Matrix_Sparse_Element
 * @brief Non-const iterator for Matrix_Sparse_Element.
 */
template<typename _entry_type, typename _index_type>
using Iterator_Matrix_Sparse_Element = Base_Iterator_Matrix_Sparse_Element<_entry_type, _index_type, false>;

/**
 * @typedef Const_Iterator_Matrix_Sparse_Element
 * @brief Const iterator for Matrix_Sparse_Element.
 */
template<typename _entry_type, typename _index_type>
using Const_Iterator_Matrix_Sparse_Element = Base_Iterator_Matrix_Sparse_Element<_entry_type, _index_type, true>;

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Row and Iterators
// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Row
// ---------------------------------------------------------------------------------------------------------------------

template<typename _value_type, typename _index_type>
struct Matrix_Sparse_Row {
 public:
  using index_type = _index_type;
  using reference_index = _index_type&;
  using pointer_index = _index_type*;
  using const_pointer_index = const _index_type*;

  using value_type = _value_type;
  using reference_value = _value_type&;
  using const_reference_value = const _value_type&;
  using pointer_value = _value_type*;
  using const_pointer_value = const _value_type*;

  using iterator = Iterator_Matrix_Sparse_Element<_value_type, _index_type>;
  using const_iterator = Const_Iterator_Matrix_Sparse_Element<_value_type, _index_type>;
  using index_iterator = std::vector<index_type>::iterator;

  using base_type = Matrix_Sparse_Row<_value_type, _index_type>;
  using csr_data = CSR_Data<typename std::remove_const<value_type>::type, typename std::remove_const<index_type>::type>;

  class Proxy {
  public:
      Proxy(Matrix_Sparse_Row& row, index_type column) : row_(row), i_column(column) {}

      operator const_reference_value() const {
        const auto iter = std::find(row_.i_column.begin(), row_.i_column.end(), i_column);
        // switch to at
#ifdef DISA_DEBUG
        if(iter != row_.i_column.end()) return *(row_.entry.begin() + std::distance(row_.i_column.begin(), iter));
        ERROR_EXIT("Trying to read ");
#else
        return *(row_.entry.begin() + std::distance(row_.i_column.begin(), iter));
#endif
      }

      Proxy& operator=(const_reference_value value) {
          insert_or_assign(i_row(row_.ptr_data, row_), i_column, value);
          std::swap(row_, Matrix_Sparse_Row(ptr_data, ptr_data->row_offset.begin() + i_row));
          return *this;
      }

  private:
      Matrix_Sparse_Row& row_;
      index_type i_column;
  };


  Matrix_Sparse_Row() = default;
  ~Matrix_Sparse_Row() = default;

  Matrix_Sparse_Row(csr_data* matrix, index_iterator row) {}

  Matrix_Sparse_Row(csr_data* matrix, index_iterator row, pointer_index s_column, pointer_index e_column,
                    const_pointer_entry s_entry, const_pointer_entry e_entry)
      : ptr_data(matrix), row_offset(row), i_column(s_column, e_column), entry(s_entry, e_entry) {}

  // TODO - there is something weird with const here (may resolve with CSR Data)
  Matrix_Sparse_Row(const csr_data* matrix, index_iterator row, const_pointer_index s_column,
                    const_pointer_index e_column, const_pointer_entry s_entry, const_pointer_entry e_entry)
      : ptr_data(const_cast<csr_data*>(matrix)),
        row_offset(row),
        i_column(s_column, e_column),
        entry(s_entry, e_entry) {}

  index_type row_offset() const { return *ptr_row_offset; };

  index_type row() const { return i_row(ptr_data, ptr_row_offset); };

  // Non-const versions
  iterator begin() noexcept { return iterator(&(*i_column.begin()), &(*entry.begin())); }

  iterator end() noexcept { return iterator(&(*i_column.end()), &(*entry.end())); }

  // Const versions
  const_iterator begin() const noexcept { return const_iterator(&(*i_column.begin()), &(*entry.begin())); }

  const_iterator end() const noexcept { return const_iterator(&(*i_column.end()), &(*entry.end())); }

  // cbegin and cend versions
  const_iterator cbegin() const noexcept { return const_iterator(&(*i_column.begin()), &(*entry.begin())); }

  const_iterator cend() const noexcept { return const_iterator(&(*i_column.end()), &(*entry.end())); }

  Proxy operator[](const index_type& i_column) noexcept {
    return Proxy(*this, i_column);
  };

  const_reference_value operator[](const index_type& i_column) const noexcept {
    const auto iter = std::find(i_column.begin(), i_column.end(), i_column);
    //switch to at 
#ifdef DISA_DEBUG 
    if(iter != i_column.end()) return *(entry.begin() + std::distance(i_column.begin(), iter));
    ERROR_EXIT("Trying to read ");
#else
    return *(entry.begin() + std::distance(i_column.begin(), iter));
#endif
  };
//at
 private:
  csr_data* ptr_data;  // used for inserts
  const_pointer_index ptr_row_offset;
  const std::span<index_type> i_column;
  const std::span<value_type> entry;
};

// ---------------------------------------------------------------------------------------------------------------------
// Matrix Sparse Row Iterators
// ---------------------------------------------------------------------------------------------------------------------

// template<typename _entry_type, typename _index_type, bool _is_const>
// struct Base_Iterator_Matrix_Sparse_Row {
//  public:
//   using index_type = _index_type;
//   using pointer_index = std::conditional_t<_is_const, const _index_type*, _index_type*>;

//   using entry_type = _entry_type;
//   using pointer_entry = std::conditional_t<_is_const, const _entry_type*, _entry_type*>;

//   using value_type = Matrix_Sparse_Row<entry_type, index_type>;
//   using reference = std::conditional_t<_is_const, const Matrix_Sparse_Row<entry_type, index_type>&,
//                                        Matrix_Sparse_Row<entry_type, index_type>&>;
//   using pointer = std::conditional_t<_is_const, const Matrix_Sparse_Row<entry_type, index_type>*,
//                                      Matrix_Sparse_Row<entry_type, index_type>*>;

//   using iterator_type = Base_Iterator_Matrix_Sparse_Row<entry_type, index_type, _is_const>;

//   Base_Iterator_Matrix_Sparse_Row() = default;
//   ~Base_Iterator_Matrix_Sparse_Row() = default;

//   reference operator*() { return value; };

//   pointer operator->() { return &value; };

//   iterator_type& operator++() {
//     ++value;
//     return *this;
//   };

//   iterator_type operator++(int) {
//     iterator_type old = *this;
//     ++value;
//     return old;
//   };

//   iterator_type& operator--() {
//     --value;
//     return *this;
//   };

//   iterator_type operator--(int) {
//     iterator_type old = *this;
//     --value;
//     return old;
//   };

//   [[nodiscard]] bool operator==(const iterator_type& that) const { return this->value == that.value; };

//   [[nodiscard]] bool operator!=(const iterator_type& that) const { return this->value != that.value; };

//  private:
//   value_type value;
// };

// template<typename _entry_type, typename _index_type>
// using Iterator_Matrix_Sparse_Row = Base_Iterator_Matrix_Sparse_Row<_entry_type, _index_type, false>;
// template<typename _entry_type, typename _index_type>
// using Const_Iterator_Matrix_Sparse_Row = Base_Iterator_Matrix_Sparse_Row<_entry_type, _index_type, true>;

// // ---------------------------------------------------------------------------------------------------------------------
// // Matrix Sparse
// // ---------------------------------------------------------------------------------------------------------------------

// template<typename _value_type, typename _index_type>
// class Matrix_Sparse {

//  public:
//   using index_type = _index_type;
//   using reference_index = _index_type&;
//   using const_reference_index = const _index_type&;
//   using pointer_index = _index_type*;

//   using value_type = _value_type;
//   using reference_value = _value_type&;
//   using const_reference_value = const _value_type&;
//   using pointer_value = _value_type*;

//   using element_type = Matrix_Sparse_Element<value_type, index_type>;
//   using element_iterator = Iterator_Matrix_Sparse_Element<value_type, index_type>;
//   using const_element_iterator = Const_Iterator_Matrix_Sparse_Element<value_type, index_type>;

//   using row_type = Matrix_Sparse_Row<value_type, index_type>;
//   using const_row_type = Matrix_Sparse_Row<const value_type, const index_type>;

//   using matrix_type = Matrix_Sparse<value_type, index_type>;

//   // ---------------------------------------------------------------------------------------------------------------------
//   // Constructors/Destructors
//   // ---------------------------------------------------------------------------------------------------------------------

//   /**
//    * @brief Default constructor.
//    */
//   Matrix_Sparse() = default;

//   /**
//    * @brief Constructs a sparse matrix with a set number of rows and columns.
//    * @param[in] row Number of rows to construct.
//    * @param[in] column Number of columns to construct.
//    */
//   Matrix_Sparse(index_type row, index_type column) : row_offset(row + 1, 0), column_size(column) {}

//   /**
//    * @brief Initializer list based on the 'raw' data structure of a sparse matrix.
//    * @param[in] non_zero The non-zero offsets per row, must be sized to number of rows + 1, and the first index should be 0.
//    * @param[in] index The column index of the non-zero value in each row, size must correspond to value, need not be be sorted.
//    * @param[in] value The value at each non-zero position in the matrix, corresponds to index.
//    * @param[in] column The absolute number of columns per row.
//    */
//   Matrix_Sparse(std::initializer_list<index_type> non_zero, std::initializer_list<index_type> index,
//                 std::initializer_list<value_type> value, index_type column)
//       : row_offset(non_zero), column_index(index), element_value(value), column_size(column) {
//     ASSERT(row_offset.front() == 0, "First value must be zero, but is " + std::to_string(row_offset.front()) + ".");
//     ASSERT(row_offset.back() == column_index.size(), "Number of non-zeros does not match number of column non zeros");
//     ASSERT(std::is_sorted(row_offset.begin(), row_offset.end()), "Inconsistent non-zeros list, but be ascending.");
//     ASSERT(column_index.size() == value.size(), "Mis-match in column and value size, " +
//                                                 std::to_string(column_index.size()) + " vs. " +
//                                                 std::to_string(value.size()));

//     std::vector<value_type> new_value;
//     std::vector<index_type> org_index;
//     FOR(row, row_offset.size() - 1) {
//       const index_type size_non_zero = row_offset[row + 1] - row_offset[row];
//       org_index.resize(size_non_zero);
//       new_value.resize(size_non_zero);
//       std::iota(org_index.begin(), org_index.end(), 0);
//       std::sort(org_index.begin(), org_index.end(), [&](const index_type index_0, const index_type index_1) {
//         return column_index[row_offset[row] + index_0] < column_index[row_offset[row] + index_1];
//       });

//       FOR(i_non_zero, size_non_zero) {
//         new_value[i_non_zero] = element_value[row_offset[row] + org_index[i_non_zero]];
//         org_index[i_non_zero] = column_index[row_offset[row] + org_index[i_non_zero]];
//         ASSERT(org_index[i_non_zero] < column_size, "Column index, " + std::to_string(org_index[i_non_zero]) +
//                                                     ", in row " + std::to_string(row) + " not in range" +
//                                                     range_column() + ".");
//       }
//       ASSERT(std::adjacent_find(org_index.begin(), org_index.end()) == org_index.end(),
//              "Duplicate column index, " + std::to_string(column_size) + "in row " + std::to_string(row) + ".");
//       std::swap_ranges(new_value.begin(), new_value.end(),
//                        std::next(element_value.begin(), static_cast<s_size_t>(row_offset[row])));
//       std::swap_ranges(org_index.begin(), org_index.end(),
//                        std::next(column_index.begin(), static_cast<s_size_t>(row_offset[row])));
//     }
//   }

//   // -------------------------------------------------------------------------------------------------------------------
//   // Element Access
//   // -------------------------------------------------------------------------------------------------------------------

//   /**
//    * @brief Subscript operator for access to a specified matrix row.
//    * @param[in] row The row index of the element.
//    * @return A sparse matrix row helper class, providing further operators for column access.
//    */
//   row_type operator[](const_reference_index row) {
//     ASSERT_DEBUG(row < size_row(), "Row " + std::to_string(row) + " not in range " + range_row() + ".");
//     return {this, row, std::span<index_type>(&column_index[row_offset[row]], &column_index[row_offset[row + 1]]),
//             std::span<value_type>(&element_value[row_offset[row]], &element_value[row_offset[row + 1]])};
//   };

//   Matrix_Sparse_Row<const value_type, const index_type> operator[](const_reference_index row) const {
//     ASSERT_DEBUG(row < size_row(), "Row " + std::to_string(row) + " not in range " + range_row() + ".");
//     return Matrix_Sparse_Row<const value_type, const index_type>(
//     this, row, &column_index[row_offset[row]], &column_index[row_offset[row + 1]], &element_value[row_offset[row]],
//     &element_value[row_offset[row + 1]]);
//   };

//   /**
//    * @brief Direct access to the underlying array of the sparse matrix.
//    * @return tuple [pointer to non zero offset start, pointer to column index start, pointer to Scalar start].
//    *
//    * @note If empty all pointers will be nullptrs, if the size_non_zero is 0 the element and column index will be
//    *       nullptrs.
//    *
//    * todo: Add test for function.
//    */
//   std::tuple<pointer_index, pointer_index, pointer_value> inline data() noexcept {
//     if(!empty() && !size_non_zero()) std::make_tuple(row_offset.data(), column_index.data(), element_value.data());
//     else if(!size_non_zero()) return std::make_tuple(row_offset.data(), nullptr, nullptr);
//     else return std::make_tuple(nullptr, nullptr, nullptr);
//   }

//   // -------------------------------------------------------------------------------------------------------------------
//   // Capacity
//   // -------------------------------------------------------------------------------------------------------------------

//   /**
//    * @brief Checks whether the matrix is empty. An empty matrix is considered where the number of rows is 0.
//    * @return True if the matrix is empty, else false.
//    */
//   [[nodiscard]] inline bool empty() const noexcept { return row_offset.empty() || row_offset.size() < 2; };

//   /**
//    * @brief Returns the number of rows in the matrix.
//    * @return The number of rows.
//    */
//   [[nodiscard]] inline index_type size_row() const noexcept {
//     return !row_offset.empty() ? static_cast<index_type>(row_offset.size() - 1) : 0;
//   };

//   /**
//    * @brief Returns the number of columns in the matrix.
//    * @return The number of columns.
//    */
//   [[nodiscard]] inline index_type size_column() const noexcept { return column_size; };

//   /**
//    * @brief Returns the number of non-zeros in the matrix.
//    * @return The number of non-zeros.
//    */
//   [[nodiscard]] inline index_type size_non_zero() const noexcept {
//     return static_cast<index_type>(column_index.size());
//   };

//   /**
//    * @brief Returns the number of rows and columns in the matrix. If rows are 0, columns are 0.
//    * @return Pair containing [rows, columns].
//    */
//   [[nodiscard]] inline std::pair<index_type, index_type> size() const noexcept {
//     return std::make_pair(size_row(), size_column());
//   };

//   /**
//    * @brief Reserves storage for the matrix.
//    * @param[in] row The number of rows to reserve for.
//    * @param[in] non_zero The number of non-zeros to reserve. Note this is not the column size.
//    *
//    * @note The number of row offsets which are reserved will be one greater than the row size passed.
//    */
//   inline void reserve(const index_type& row, const index_type& non_zero) noexcept {
//     row_offset.reserve(row + 1);
//     column_index.reserve(non_zero);
//     element_value.reserve(non_zero);
//   };

//   /**
//    * @brief Returns the number of row offsets and non-zeros entries that can be held in currently allocated storage.
//    * @return [number of rows offsets, number of non-zeros entries.]
//    *
//    * @note The number of row offsets (for CSR matrices) is always one greater than the actual matrix row size.
//    */
//   [[nodiscard]] inline std::pair<index_type, index_type> capacity() const noexcept {
//     return std::make_pair(row_offset.capacity(), row_offset.capacity());
//   };

//   /**
//    * @brief reduces memory usage by the matrix by freeing unused memory for both rows and non-zeros
//    */
//   inline void shrink_to_fit() noexcept {
//     row_offset.shrink_to_fit();
//     column_index.shrink_to_fit();
//     element_value.shrink_to_fit();
//   };

//   // -------------------------------------------------------------------------------------------------------------------
//   // Modifiers
//   // -------------------------------------------------------------------------------------------------------------------

//   /**
//    * @brief Clears the contents of the matrix, sets the column size to zero.
//    */
//   inline void clear() noexcept {
//     row_offset.clear();
//     column_index.clear();
//     element_value.clear();
//     column_size = 0;
//   };

//   // std::pair<element_iterator, bool>
//   void insert(const_reference_index row, const_reference_index column, const_reference_value value) {
//     // // Resize if we need to.
//     // if(row >= size_row()) resize(row + 1, column_size);
//     // if(column >= size_column()) resize(size_row(), column + 1);
//     // const auto iter_insert = lower_bound(row, column);
//     // if(!(iter_insert == (*this)[row].end() || iter_insert.i_column() != column)) return {iter_insert, false};

//     // for(auto non_zeros = std::next(row_offset.begin(), static_cast<s_size_t>(iter_insert.i_row() + 1));
//     //     non_zeros < row_offset.end(); ++(*non_zeros++));
//     // const auto distance = std::distance(&*element_value.cbegin(), &*iter_insert);
//     // return {element_iterator(row, &*column_index.insert(column_index.begin() + distance, column),
//     //                          &*element_value.insert(element_value.begin() + distance, value)), true};
//   }

//   /**
//    * @brief Changes the number of rows and columns of the matrix.
//    * @param[in] row Number of rows to resized the matrix to.
//    * @param[in] column Number of columns to resized the matrix to.
//    *
//    * @note Does not effect number of non-zeros, unless the new row and column sizes reduce the size of the matrix.
//    */
//   void resize(const_reference_index row, const_reference_index column) {

//     using s_index_type = std::make_signed_t<index_type>;

//     // resize rows first
//     if(row < size_row()) {
//       column_index.resize(row_offset[row]);
//       element_value.resize(row_offset[row]);
//     }
//     row_offset.resize(row + 1, row_offset.empty() ? 0 : row_offset.back());

//     // resize columns
//     // Note: Since we may have to do column erases over multiple rows we do it directly here rather than use erase().
//     if(column < size_column()) {
//       index_type offset_loss = 0;
//       FOR(i_row, size_row()) {
//         const auto& iter_column_end =
//         column_index.begin() + static_cast<s_index_type>(row_offset[i_row + 1] - offset_loss);
//         auto iter_column = std::upper_bound(column_index.begin() + static_cast<s_index_type>(row_offset[i_row]),
//                                             iter_column_end, column - 1);
//         if(iter_column != iter_column_end) {
//           const s_index_type& start_distance = std::distance(column_index.begin(), iter_column);
//           const s_index_type& end_distance = std::distance(column_index.begin(), iter_column_end);
//           column_index.erase(iter_column, iter_column_end);
//           element_value.erase(element_value.begin() + start_distance, element_value.begin() + end_distance);
//           offset_loss += end_distance - start_distance;
//         }
//         row_offset[i_row + 1] -= offset_loss;
//       }
//     }
//     column_size = column;
//   }

//   /**
//    * @brief Swaps the contents of the matrix with the parsed matrix
//    * @param[in,out] other The other matrix, this matrix will obtain the other's value and visa versa.
//    */
//   inline void swap(matrix_type& other) noexcept {
//     std::swap(row_offset, other.row_offset);
//     std::swap(column_index, other.column_index);
//     std::swap(element_value, other.element_value);
//     std::swap(column_size, other.column_size);
//   };

//   // -------------------------------------------------------------------------------------------------------------------
//   // Lookup
//   // -------------------------------------------------------------------------------------------------------------------

//   // /**
//   //  * @brief Finds the value iterator to the matrix value at a row and column.
//   //  * @param[in] i_row The row index to the element.
//   //  * @param[in] i_column The column index to the element.
//   //  * @return Element iterator.
//   //  *
//   //  * @note If i_row >= row_size() end() is returned, if no element is found *(begin + i_row).end() is returned.
//   //  */
//   // iterator_element find(const std::size_t& i_row, const std::size_t& i_column);

//   // /**
//   //  * @brief Finds the value iterator to the matrix value at a row and column.
//   //  * @param[in] i_row The row index to the element.
//   //  * @param[in] i_column The column index to the element.
//   //  * @return constant element iterator.
//   //  *
//   //  * @note If i_row >= row_size() end() is returned, if no element is found *(begin + i_row).end() is returned.
//   //  */
//   // [[nodiscard]] const_iterator_element find(const std::size_t& i_row, const std::size_t& i_column) const;

//   // /**
//   //  * @brief Checks if the element at a row and column are non-zero.
//   //  * @param[in] i_row The row index to the element.
//   //  * @param[in] i_column The column index to the element.
//   //  * @return True if non-zero, else false.
//   //  */
//   // [[nodiscard]] bool contains(const std::size_t& i_row, const std::size_t& i_column) const;

//   // /**
//   //  * @brief Returns an element iterator to the first element not less than the given key.
//   //  * @param[in] i_row The row index to the element.
//   //  * @param[in] i_column The column index of the element.
//   //  * @return Iterator pointing to the first element that is not less than key.
//   //  *
//   //  * @note If i_row >= row_size() end() is returned, if no element is found *(begin + i_row).end() is returned.
//   //  */
//   // iterator_element lower_bound(const std::size_t& i_row, const std::size_t& i_column);

//   // /**
//   //  * @brief Returns an element iterator to the first element not less than the given key.
//   //  * @param[in] i_row The row index to the element.
//   //  * @param[in] i_column The column index of the element.
//   //  * @return Const iterator pointing to the first element that is not less than key.
//   //  *
//   //  * @note If i_row >= row_size() end() is returned, if no element is found *(begin + i_row).end() is returned.
//   //  */
//   // [[nodiscard]] const_iterator_element lower_bound(const std::size_t& i_row, const std::size_t& i_column) const;

//  private:
//   std::vector<index_type>
//   row_offset;  //!< Offset to non-zero elements in the matrix for each row (size is one greater than number of rows).
//   std::vector<index_type> column_index;   //!< The column index for each non-zero value, corresponds to value.
//   std::vector<value_type> element_value;  //!< The each non-zero value value, corresponds to column_index.
//   index_type column_size{0};  //!< The number of columns of the matrix (used to check validity of operations).

//   // -------------------------------------------------------------------------------------------------------------------
//   // Private Member Functions
//   // -------------------------------------------------------------------------------------------------------------------

//   /**
//    * @brief Converts and formats a row and colum index to string.
//    * @param[in] i_row The row index.
//    * @param[in] i_column The column index.
//    * @return Formatted string.
//    */
//   [[nodiscard]] static std::string row_column(std::size_t i_row, std::size_t i_column) {
//     return std::to_string(i_row) + ", " + std::to_string(i_column);
//   };

//   /**
//    * @brief Creates a string formatted for the range of the matrix rows.
//    * @return Formatted string.
//    */
//   [[nodiscard]] std::string range_row() const { return "[0, " + std::to_string(row_offset.size() - 1) + ")"; };

//   /**
//    * @brief Creates a string formatted for the range of the matrix columns.
//    * @return Formatted string.
//    */
//   [[nodiscard]] std::string range_column() const { return "[0, " + std::to_string(column_size) + ")"; };

//   /**
//    * @brief Performs addition and subtraction operations between this matrix and another, i.e. A' = A + B or A' = A - B.
//    * @tparam _is_add If true, addition is performed else subtraction is performed.
//    * @param[in] other The other matrix B.
//    * @return Updated matrix (A').
//    */
//   template<bool _is_add>
//   Matrix_Sparse& matrix_arithmetic(const Matrix_Sparse& other);
// };

}  // namespace Disa

#endif  // DISA_MATRIX_SPARSE_NEW_H