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

#include <tuple>
#include <vector>

#include "scalar.hpp"

namespace Disa
{

template <typename _value_type, typename _index_type>
struct CSR_Data
{
  using iterator = std::tuple<_index_type, typename std::vector<_index_type>::const_iterator, typename std::vector<_value_type>::const_iterator>;
  using const_iterator = std::tuple<_index_type, typename std::vector<_index_type>::const_iterator, typename std::vector<_value_type>::const_iterator>;

  std::vector<_index_type> row_offset;
  std::vector<_index_type> column_index;
  std::vector<_value_type> value;
  _index_type columns;
};

template <typename _value_type, typename _index_type>
_index_type size_row(const CSR_Data<_value_type, _index_type> &data) noexcept;

template <typename _value_type, typename _index_type>
_index_type size_column(const CSR_Data<_value_type, _index_type> &data) noexcept;

template <typename _value_type, typename _index_type>
_index_type size_non_zero(const CSR_Data<_value_type, _index_type> &data) noexcept;

template <typename _value_type, typename _index_type>
std::pair<typename CSR_Data<_value_type, _index_type>::iterator, bool> insert(CSR_Data<_value_type, _index_type> &data, 
                                                                              const _index_type &row, 
                                                                              const _index_type &column,
                                                                              const _value_type &value);

template <typename _value_type, typename _index_type>
void resize(CSR_Data<_value_type, _index_type> &data, const _index_type &row, const _index_type &column);

template <typename _value_type, typename _index_type>
CSR_Data<_value_type, _index_type>::iterator lower_bound(CSR_Data<_value_type, _index_type>& data, 
                                                         const _index_type& row, const _index_type& column);

template <typename _value_type, typename _index_type>
CSR_Data<_value_type, _index_type>::const_iterator lower_bound(const CSR_Data<_value_type, _index_type>& data, 
                                                               const _index_type& row, const _index_type& column);



} // namespace Disa

#endif // DISA_MATRIX_SPARSE_DATA_H
