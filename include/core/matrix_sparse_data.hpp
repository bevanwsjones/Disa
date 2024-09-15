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

namespace Disa {

template<typename _value_type, typename _index_type = std::size_t>
struct CSR_Data {
  using iterator =
  std::tuple<_index_type, typename std::vector<_index_type>::iterator, typename std::vector<_value_type>::iterator>;
  using const_iterator = std::tuple<const _index_type, typename std::vector<_index_type>::const_iterator,
                                    typename std::vector<_value_type>::const_iterator>;

  std::vector<_index_type> row_offset;
  std::vector<_index_type> column_index;
  std::vector<_value_type> value;
  _index_type columns;
};

template<typename _value_type, typename _index_type>
_index_type size_row(const CSR_Data<_value_type, _index_type>& data) noexcept {
  return !data.row_offset.empty() ? data.row_offset.size() - 1 : 0;
};

template<typename _value_type, typename _index_type>
_index_type size_column(const CSR_Data<_value_type, _index_type>& data) noexcept {
  return data.columns;
};

template<typename _value_type, typename _index_type>
_index_type size_non_zero(const CSR_Data<_value_type, _index_type>& data) noexcept {
  return data.column_index.size();
};

template<typename _value_type, typename _index_type>
std::pair<typename CSR_Data<_value_type, _index_type>::iterator, bool> insert(CSR_Data<_value_type, _index_type>& data,
                                                                              const _index_type& row,
                                                                              const _index_type& column,
                                                                              const _value_type& value) {
  // Resize if we need to.
  using s_index_type = std::make_signed_t<_index_type>;

  if(row >= size_row(data)) resize(data, row + 1, data.columns);
  if(column >= size_column(data)) resize(data, size_row(data), data.columns + 1);
  auto iter_insert = lower_bound(data, row, column);
  if(std::get<0>(iter_insert) == row && *std::get<1>(iter_insert) == column) return {iter_insert, false};

  for(auto non_zeros = std::next(data.row_offset.begin(), static_cast<s_index_type>(row + 1));
      non_zeros < data.row_offset.end(); ++(*non_zeros++))
    ;
  auto iter_column = data.column_index.insert(std::get<1>(iter_insert), column);
  auto iter_value = data.value.insert(std::get<2>(iter_insert), value);
  return {std::make_tuple(row, iter_column, iter_value), true};
}

template<typename _value_type, typename _index_type>
void resize(CSR_Data<_value_type, _index_type>& data, const _index_type& row, const _index_type& column) {

  using s_index_type = std::make_signed_t<_index_type>;

  // resize rows first
  if(row < size_row(data)) {
    data.column_index.resize(data.row_offset[row]);
    data.value.resize(data.row_offset[row]);
  }
  data.row_offset.resize(row + 1, data.row_offset.empty() ? 0 : data.row_offset.back());

  // resize columns
  // Note: Since we may have to do column erases over multiple rows we do it directly here rather than use erase().
  if(column < size_column(data)) {
    _index_type offset_loss = 0;
    FOR(row, size_row(data)) {
      const auto& iter_column_end =
      data.column_index.begin() + static_cast<s_index_type>(data.row_offset[row + 1] - offset_loss);
      auto iter_column = std::upper_bound(data.column_index.begin() + static_cast<s_index_type>(data.row_offset[row]),
                                          iter_column_end, column - 1);
      if(iter_column != iter_column_end) {
        const s_index_type& start_distance = std::distance(data.column_index.begin(), iter_column);
        const s_index_type& end_distance = std::distance(data.column_index.begin(), iter_column_end);
        data.column_index.erase(iter_column, iter_column_end);
        data.value.erase(data.value.begin() + start_distance, data.value.begin() + end_distance);
        offset_loss += end_distance - start_distance;
      }
      data.row_offset[row + 1] -= offset_loss;
    }
  }
  data.columns = column;
}

template<typename _value_type, typename _index_type>
CSR_Data<_value_type, _index_type>::iterator lower_bound(CSR_Data<_value_type, _index_type>& data,
                                                         const _index_type& row, const _index_type& column) {
  if(row < size_row(data)) {
    const auto& iter_start = data.column_index.begin() + data.row_offset[row];
    const auto& iter_end = data.column_index.begin() + data.row_offset[row + 1];
    const auto& iter_lower = std::lower_bound(iter_start, iter_end, column);
    return std::make_tuple(iter_lower != iter_end ? row : (row + 1), iter_lower,
                           data.value.begin() + std::distance(data.column_index.begin(), iter_lower));
  } else return std::make_tuple(size_row(data), data.column_index.end(), data.value.end());
}

template<typename _value_type, typename _index_type>
CSR_Data<_value_type, _index_type>::const_iterator lower_bound(const CSR_Data<_value_type, _index_type>& data,
                                                               const _index_type& row, const _index_type& column) {
  if(row < size_row(data)) {
    const auto& iter_start = data.column_index.begin() + data.row_offset[row];
    const auto& iter_end = data.column_index.begin() + data.row_offset[row + 1];
    const auto& iter_lower = std::lower_bound(iter_start, iter_end, column);
    return std::make_tuple(iter_lower != iter_end ? row : (row + 1), iter_lower,
                           data.value.begin() + std::distance(data.column_index.begin(), iter_lower));
  } else return std::make_tuple(size_row(data), data.column_index.end(), data.value.end());
}

}  // namespace Disa

#endif  // DISA_MATRIX_SPARSE_DATA_H
