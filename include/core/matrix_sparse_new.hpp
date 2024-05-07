
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
#include <vector>
#include <span>

namespace Disa {

template<typename _entry_type, typename _index_type>
class Matrix_Sparse;

/**
 * @brief Represents an element in a sparse matrix.
 * 
 * This struct stores the row index, column pointer, and entry pointer of an element in a sparse matrix.
 * It provides member functions to access and modify the value, row index, column index, and entry of the element.
 * It also overloads comparison operators for equality and inequality.
 * 
 * @tparam _entry_type The type of the entry in the matrix element.
 * @tparam _index_type The type of the row and column indices.
 */
template<typename _entry_type, typename _index_type>
struct Matrix_Sparse_Element {

    public:
    using index_type = _index_type;
    using reference_index = _index_type&;
    using const_reference_index = const _index_type&;
    using pointer_index = _index_type*;

    using entry_type = _entry_type;
    using reference_entry = _entry_type&;
    using const_reference_entry = const _entry_type&;
    using pointer_entry = _entry_type*;
    
    using base_type = Matrix_Sparse_Element<entry_type, index_type>;

    /**
     * @brief Default constructor.
     */
    Matrix_Sparse_Element() = default;

    /**
     * @brief Destructor.
     */
    ~Matrix_Sparse_Element() = default;

    /**
     * @brief Constructor that initializes the element with the given row index, column pointer, and entry pointer.
     * 
     * @param row The row index of the element.
     * @param column The column pointer of the element.
     * @param entry The entry pointer of the element.
     */
    Matrix_Sparse_Element(const index_type& row, pointer_index column, pointer_entry entry) : row_index(row), column_ptr(column), entry_ptr(entry) {};

    /**
     * @brief Returns a reference to the value of the element.
     * 
     * @return A reference to the value of the element.
     */
    reference_entry value() { return *entry_ptr; };

    /**
     * @brief Returns the row index of the element.
     * 
     * @return The row index of the element.
     */
    const_reference_index row() const { return row_index; };

    /**
     * @brief Returns the column index of the element.
     * 
     * @return The column index of the element.
     */
    const_reference_index column() const { return *column_ptr; };

    /**
     * @brief Returns the value of the element.
     * 
     * @return The value of the element.
     */
    const_reference_entry value() const { return *entry_ptr; };

    /**
     * @brief Prefix increment operator.
     * 
     * @return A reference to the incremented element.
     */
    base_type& operator++() { ++column_ptr; ++entry_ptr; return *this; };

    /**
     * @brief Postfix increment operator.
     * 
     * @return A copy of the original element before incrementing.
     */
    base_type operator++(int){base_type old = *this; ++column_ptr; ++entry_ptr; return old; };

    /**
     * @brief Prefix decrement operator.
     * 
     * @return A reference to the decremented element.
     */
    base_type& operator--() {--column_ptr; --entry_ptr; return *this; };

    /**
     * @brief Postfix decrement operator.
     * 
     * @return A copy of the original element before decrementing.
     */
    base_type operator--(int) {base_type old = *this; --column_ptr; --entry_ptr; return old;};

    /**
     * @brief Equality comparison operator.
     * 
     * @param that The element to compare with.
     * @return True if the elements (row, column, value) are equal, false otherwise.
     */
    [[nodiscard]] bool operator==(const base_type& that) const {
        return this->row_index == that.row_index && *this->column_ptr == *that.column_ptr 
               && *this->entry_ptr == *that.entry_ptr; 
    };

    /**
     * @brief Inequality comparison operator.
     * 
     * @param that The element to compare with.
     * @return True if the elements are not equal, false otherwise.
     */
    [[nodiscard]] bool operator!=(const base_type& that) const {
        return !(*this == that); 
    };

    private:
    index_type row_index;       /**< The row index of the element. */
    pointer_index column_ptr;   /**< The column pointer of the element. */
    pointer_entry entry_ptr;    /**< The entry pointer of the element. */    
};

template<typename _entry_type, typename _index_type, bool _is_const>
struct Base_Iterator_Matrix_Sparse_Element {
  public:
    using index_type = _index_type;
    using pointer_index = std::conditional_t<_is_const, const _index_type*, _index_type*>;
    
    using entry_type = _entry_type;
    using pointer_entry = std::conditional_t<_is_const, const _entry_type*, _entry_type*>;  
   
    using value_type = Matrix_Sparse_Element<entry_type, index_type>;
    using reference = std::conditional_t<_is_const, const Matrix_Sparse_Element<entry_type, index_type>&, Matrix_Sparse_Element<entry_type, index_type>&>;
    using pointer = std::conditional_t<_is_const, const Matrix_Sparse_Element<entry_type, index_type>*, Matrix_Sparse_Element<entry_type, index_type>*>;

    using iterator_type = Base_Iterator_Matrix_Sparse_Element<entry_type, index_type, _is_const>;

    Base_Iterator_Matrix_Sparse_Element() = default;
    ~Base_Iterator_Matrix_Sparse_Element() = default;
    Base_Iterator_Matrix_Sparse_Element(value_type element_value) : value(element_value) {};
    Base_Iterator_Matrix_Sparse_Element(const index_type& row, index_type* column, entry_type* entry) : value(row, column, entry) {};

    reference operator*(){ return value; };
    pointer operator->() { return &value; };

    iterator_type& operator++() { ++value; return *this; };
    iterator_type operator++(int){iterator_type old = *this; ++value; return old; };
    iterator_type& operator--() {--value; return *this; };
    iterator_type operator--(int) {iterator_type old = *this; --value; return old; };

    [[nodiscard]] bool operator==(const iterator_type& that) const {
        return this->value == that.value; 
    };

    [[nodiscard]] bool operator!=(const iterator_type& that) const {
        return this->value != that.value; 
    };

  private:
    value_type value;
};

template<typename _entry_type, typename _index_type>
using Iterator_Matrix_Sparse_Element = Base_Iterator_Matrix_Sparse_Element<_entry_type, _index_type, false>;
template<typename _entry_type, typename _index_type>
using Const_Iterator_Matrix_Sparse_Element = Base_Iterator_Matrix_Sparse_Element<_entry_type, _index_type, true>;

// row

template<typename _entry_type, typename _index_type>
struct Matrix_Sparse_Row {
    public:
    using index_type = _index_type;
    using reference_index = _index_type&;
    using pointer_index = _index_type*;

    using entry_type = _entry_type;
    using reference_entry = _entry_type&;
    using const_reference_entry = const _entry_type&;
    using pointer_entry = _entry_type*;
    using const_pointer_entry = const _entry_type*;
  
    using iterator = Iterator_Matrix_Sparse_Element<_entry_type, _index_type>;
    using const_iterator = Const_Iterator_Matrix_Sparse_Element<_entry_type, _index_type>;

    using base_type = Matrix_Sparse_Row<_entry_type, _index_type>;

    Matrix_Sparse_Row() = default;
    ~Matrix_Sparse_Row() = default;
    Matrix_Sparse_Row(Matrix_Sparse<entry_type, index_type>* matrix, const index_type& row, std::span<index_type> column, std::span<entry_type> entries) : matrix_ptr(matrix), row_index(row), column_index(column), entry(entries) {};

    index_type row() const { return row_index; };
    
    // Non-const versions
    iterator begin() { return iterator(row_index, &(*column_index.begin()), &(*entry.begin())); }
    iterator end() { return iterator(row_index, &(*column_index.end()), &(*entry.end())); }

    // Const versions
    const_iterator begin() const { return const_iterator(row_index, &(*column_index.begin()), &(*entry.begin())); }
    const_iterator end() const { return const_iterator(row_index, &(*column_index.end()), &(*entry.end())); }

    // cbegin and cend versions
    const_iterator cbegin() const { return const_iterator(row_index, &(*column_index.begin()), &(*entry.begin())); }
    const_iterator cend() const { return const_iterator(row_index, &(*column_index.end()), &(*entry.end())); }

    base_type& operator++() { ++row_index; return *this; };
    base_type operator++(int){base_type old = *this; ++row_index; return old; };
    base_type& operator--() {--row_index; return *this; };
    base_type operator--(int) {base_type old = *this; --row_index; return old; };

    reference_entry operator[](const index_type& i_column) { 
      const auto iter = std::find(column_index.begin(), column_index.end(), i_column);
      if(iter != column_index.end()) return *(entry.begin() + std::distance(column_index.begin(), iter));
      else {
        matrix_ptr->insert(row_index, i_column, 0.0);
        *this = (*matrix_ptr)[row_index];
        return (*this)[i_column];
      }
    };

    const_reference_entry operator[](const index_type& i_column) const { 
        const auto iter = std::find(column_index.begin(), column_index.end(), i_column);
        return iter != column_index.end() ? *(entry.begin() + std::distance(column_index.begin(), iter)) : 0; 
    };

    private: 
    Matrix_Sparse<entry_type, index_type>* matrix_ptr; // used for inserts 
    index_type row_index;
    std::span<index_type> column_index;
    std::span<entry_type> entry;
};

template<typename _entry_type, typename _index_type, bool _is_const>
struct Base_Iterator_Matrix_Sparse_Row {
  public:
    using index_type = _index_type;
    using pointer_index = std::conditional_t<_is_const, const _index_type*, _index_type*>;
    
    using entry_type = _entry_type;
    using pointer_entry = std::conditional_t<_is_const, const _entry_type*, _entry_type*>;  
   
    using value_type = Matrix_Sparse_Row<entry_type, index_type>;
    using reference = std::conditional_t<_is_const, const Matrix_Sparse_Row<entry_type, index_type>&, Matrix_Sparse_Row<entry_type, index_type>&>;
    using pointer = std::conditional_t<_is_const, const Matrix_Sparse_Row<entry_type, index_type>*, Matrix_Sparse_Row<entry_type, index_type>*>;

    using iterator_type = Base_Iterator_Matrix_Sparse_Row<entry_type, index_type, _is_const>;

    Base_Iterator_Matrix_Sparse_Row() = default;
    ~Base_Iterator_Matrix_Sparse_Row() = default;
    
    reference operator*(){ return value; };
    pointer operator->() { return &value; };

    iterator_type& operator++() { ++value; return *this; };
    iterator_type operator++(int){iterator_type old = *this; ++value; return old; };
    iterator_type& operator--() {--value; return *this; };
    iterator_type operator--(int) {iterator_type old = *this; --value; return old; };

    [[nodiscard]] bool operator==(const iterator_type& that) const {
        return this->value == that.value; 
    };

    [[nodiscard]] bool operator!=(const iterator_type& that) const {
        return this->value != that.value; 
    };

  private:
    value_type value;
};

template<typename _entry_type, typename _index_type>
using Iterator_Matrix_Sparse_Row = Base_Iterator_Matrix_Sparse_Row<_entry_type, _index_type, false>;
template<typename _entry_type, typename _index_type>
using Const_Iterator_Matrix_Sparse_Row = Base_Iterator_Matrix_Sparse_Row<_entry_type, _index_type, true>;


template<typename _entry_type, typename _index_type>
class Matrix_Sparse {

    public:
    void insert(const _index_type& row, const _index_type& column, const _entry_type& value) {
        
    };

//   public:
//   using index_type = _index_type;
//   using entry_type = _entry_type;
   using row_type = Matrix_Sparse_Row<_entry_type, _index_type>;
//   using matrix_type = Matrix_Sparse<entry_type, index_type>;
//   using interator = Iterator_Matrix_Sparse_Row<entry_type, index_type>;
//   using const_iterator = COnst_Iterator_Matrix_Sparse_Row<entry_type, index_type>;

    row_type operator[](const _index_type& i_row) {
         return row_type(); 
    };

    private:
    std::vector<_index_type> row_offsets;
    std::vector<_index_type> column_index;
    std::vector<_entry_type> entry;

};

}// namespace Disa

#endif // DISA_MATRIX_SPARSE_NEW_H