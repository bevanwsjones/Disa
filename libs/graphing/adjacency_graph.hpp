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
//  File Name: adjacency_graph.hpp
//  Description: Contains the template definitions of the Adjacency_Graph Class.
// ---------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
// Modifiers
//--------------------------------------------------------------------------------------------------------------------

/**
 * @details
 */
template<class _unary_predicate>
void Adjacency_Graph::erase_if(_unary_predicate delete_vertex) {

  std::size_t removed = 0;
  std::size_t i_vertex = 0;
  std::vector<bool> adjacency_delete(vertex_adjacent_list.size(), false);
  std::vector<std::size_t> new_indexes(vertex_adjacent_list.size(), std::numeric_limits<std::size_t>::max());

  // Loop over the data, and determine the new offsets and which entries in vertex_adjacent_list need to be removed.
  FOR(i_vertex_old, size_vertex()) {
    const auto& adjacency = vertex_adjacency_iter(i_vertex_old);
    const auto& begin_delete = adjacency_delete.begin() + std::distance(vertex_adjacent_list.begin(), adjacency.first);
    const auto& end_delete = adjacency_delete.begin() + std::distance(vertex_adjacent_list.begin(), adjacency.second);

    offset[i_vertex_old] -= removed;

    // Only remove vertices which will be taken out, and record new indexes.
    if(!delete_vertex(i_vertex_old)) {
      new_indexes[i_vertex_old] = i_vertex++;
      removed += std::count_if(adjacency.first, adjacency.second, delete_vertex);
      std::transform(adjacency.first, adjacency.second, begin_delete, delete_vertex);
    }
    else { // All adjacency entries need to be removed, since the vertex is to be removed.
      removed += std::distance(adjacency.first, adjacency.second);
      std::transform(begin_delete, end_delete, begin_delete, [](const auto){return true;});
    }
  }
  offset.back() -= removed;

  // Erase offset entries for the removed vertices.
  i_vertex = 0;
  offset.erase(std::remove_if(offset.begin(), offset.end(),
                              [&](const auto){return i_vertex < size_vertex() ? delete_vertex(i_vertex++) : false;}),
               offset.end());

  // Erase removed vertices from the adjacency lists, and then relabel.
  i_vertex = 0;
  vertex_adjacent_list.erase(std::remove_if(vertex_adjacent_list.begin(), vertex_adjacent_list.end(),
                                            [&](const auto){return adjacency_delete[i_vertex++];}),
                             vertex_adjacent_list.end());
  std::transform(vertex_adjacent_list.begin(), vertex_adjacent_list.end(), vertex_adjacent_list.begin(),
                 [&](const auto& i_old_vertex){return new_indexes[i_old_vertex];});

  ASSERT(offset.back() == vertex_adjacent_list.size(),
         "Total offsets no longer match vertex size while reducing to subgraph, " + std::to_string(offset.back())
         + " vs. " + std::to_string(vertex_adjacent_list.size()) + ".");
};
