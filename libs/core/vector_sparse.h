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
// File Name: vector_sparse.h
// Description: Contains the declaration and definition of mathematical vector sparse vector. 
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_VECTOR_SPARSE_H
#define DISA_VECTOR_SPARSE_H

#include <vector>

#include <map>

#include "index.h"
#include "macros.h"

namespace Disa {

// iterator
//   typedef _Tp  non_zero_type;
//   typedef _Tp& reference;
//   typedef _Tp* pointer;

//   typedef bidirectional_iterator_tag iterator_category;
//   typedef ptrdiff_t			 difference_type;

//   typedef _Rb_tree_iterator<_Tp>		_Self;
//   typedef _Rb_tree_node_base::_Base_ptr	_Base_ptr;
//   typedef _Rb_tree_node<_Tp>*		_Link_type;


//const iterator 
//   typedef _Tp	 non_zero_type;
//   typedef const _Tp& reference;
//   typedef const _Tp* pointer;

//   typedef _Rb_tree_iterator<_Tp> iterator;

//   typedef bidirectional_iterator_tag iterator_category;
//   typedef ptrdiff_t			 difference_type;

//   typedef _Rb_tree_const_iterator<_Tp>		_Self;
//   typedef _Rb_tree_node_base::_Const_Base_ptr	_Base_ptr;
//   typedef const _Rb_tree_node<_Tp>*			_Link_type;

template<typename _type, typename _index>
struct Iterator_Base {

    public:
    using index_type = _index;
    using reference_index = _index&;
    using pointer_index = _index*;

    using non_zero_type = _type;
    using reference_non_zero = _type&;
    using pointer_non_zero = _type*;
    
    using base_type = Iterator_Base<_type, _index>;

    Iterator_Base() = default;
    ~Iterator_Base() = default;
    Iterator_Base(pointer_non_zero non_zero, pointer_index index) : non_zero_ptr(non_zero), index_ptr(index) { };
    Iterator_Base(base_type&& other);

    non_zero_type& value() { return *non_zero_ptr };
    index_type& index() {return *index_ptr; };

    const non_zero_type& value() const { return *non_zero_ptr };
    const index_type& index() const {return *index_ptr; };

    base_type& operator++() { ++index_ptr; ++non_zero_ptr; return this;  };
    base_type operator++(int){base_type old = *this; ++index_ptr; ++non_zero_ptr; return old; };
    base_type& operator--(){ --index_ptr; --non_zero_ptr; return this;  };;
    base_type operator--(int) {base_type old = *this; --index_ptr; --non_zero_ptr; return old;};

    bool operator==(const base_type that){
        return this->index_ptr == that.index_ptr && this->non_zero_ptr == that.non_zero_ptr
    };

    private:
    pointer_index index_ptr;
    pointer_non_zero non_zero_ptr;
};

template<typename _type,typename _index>
class Iterator_Vector_Sparse {

    public:

    using index_type = _index;
    using pointer_index = _index*;

    using non_zero_type = _type;
    using pointer_non_zero = _type*;

    using value_type = Iterator_Base<_type, _index>;
    using reference = Iterator_Base<_type, _index>&;
    using pointer = Iterator_Base<_type, _index>*;

    using iterator_type = Iterator_Vector_Sparse<_type, _index>;

    Iterator_Vector_Sparse(){};
    ~Iterator_Vector_Sparse(){};
    Iterator_Vector_Sparse(pointer_non_zero non_zero, pointer_index index) : value(value_type(non_zero, index)) { };

    reference operator*();
    pointer operator->();
    iterator_type& operator++();
    iterator_type operator++(int);
    iterator_type& operator--();
    iterator_type operator--(int);
    bool operator==(const iterator_type& that) {
        return this->value == that.value;
    };

    private:
    value_type value;
};

template<typename _type,typename _index>
class Const_Iterator_Vector_Sparse {

    public:

    using index_type = _index;
    using pointer_index = _index*;

    using non_zero_type = _type;
    using pointer_non_zero = _type*;

    using value_type = Iterator_Base<_type, _index>;
    using reference = const Iterator_Base<_type, _index>&;
    using pointer = const Iterator_Base<_type, _index>*;

    using iterator_type = Const_Iterator_Vector_Sparse<_type, _index>;

    Const_Iterator_Vector_Sparse() = default;
    ~Iterator_Vector_Sparse() = default;
    Const_Iterator_Vector_Sparse(pointer_non_zero non_zero, pointer_index index) : value(value_type(non_zero, index)) { };

    reference operator*();
    pointer operator->();
    iterator_type& operator++();
    iterator_type operator++(int);
    iterator_type& operator--();
    iterator_type operator--(int);
    bool operator==(const iterator_type& that) {
        return this->value == that.value;
    };

    private:
    value_type value;
};

template<typename _type,typename _index>
class Vector_Sparse {

    public:
    using index_type = _index;
    using non_zero_type = _type;
    using vector_type = Vector_Sparse<non_zero_type, index_type>;
    using interator = Iterator_Vector_Sparse<non_zero_type, index_type>;
    using const_iterator = Const_Iterator_Vector_Sparse<non_zero_type, index_type>;;
    
    Vector_Sparse() = default;
    ~Vector_Sparse() = default;
    Vector_Sparse(vector_type&& other);

    // Element Access
    [[nodiscard]] non_zero_type at(index_type i_element);
    [[nodiscard]] const non_zero_type& at(index_type i_element) const;
    [[nodiscard]] non_zero_type operator[](index_type i_element);
    [[nodiscard]] const non_zero_type& operator[](index_type i_element) const;
    [[nodiscard]] std::pair<index_type*, non_zero_type*> data();
    
    // iteratiors 

    // capacity
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] index_type size() const noexcept;
    void reserve(index_type size_non_zero) noexcept;
    [[nodiscard]] index_type capacity() noexcept;
    void shrink_to_fit() noexcept;

    // modifiers
    void clear() noexcept;
    std::pair<interator, bool> insert(index_type index, non_zero_type value);
    std::pair<interator, bool> insert_or_assign(index_type index, non_zero_type value);
    iterator erase(iterator iter);
    void resize(index_type size);
    void swap(vector_type other);

    //lookup
    interator find(index_type index);
    [[nodiscard]] const_iterator find(index_type index) const;
    [[nodiscard]] bool contains(index_type index) const;
    iterator lower_bound(index_type index);
    [[nodiscard]] const_iterator lower_bound(index_type index) const; 

    // mathmatical assignment operators
    vector_type& operator*=(non_zero_type scalar);
    vector_type& operator/=(non_zero_type scalar);
    vector_type& operator+=(const vector_type& other);
    vector_type& operator-=(const vector_type& other);
    
    private:

    std::vector<index_type> i_element;
    std::vector<non_zero_type> values;

};

int main(){
    Index x;
    Scalar y;
    Index* px = &x;
    Scalar* py = &y;
    Iterator_Vector_Sparse<Scalar, Index> iter(py, px);
    Const_Iterator_Vector_Sparse<Scalar, Index> const_iter(py, px);
    iter->value() = 0.0;
    iter->index() = 0;

    const_iter->index();
    const_iter->value();
}

}

#endif //DISA_VECTOR_SPARSE_H