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

namespace Disa {

// ---------------------------------------------------------------------------------------------------------------------
// Adjacency_Graph
// ---------------------------------------------------------------------------------------------------------------------
// Public Constructors and Destructors
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details Initializes an Adjacency_Graph object with the edges in the given initializer list. The constructor finds
 * the vertex with the highest index to determine a suitable reserve for the graph in the provided edge list. Each edge
 * is then inserted into the edge and any excess reserve memory removed.
 */
template<bool _directed>
Adjacency_Graph<_directed>::Adjacency_Graph(std::initializer_list<Edge> edge_graph) {
  const auto iter = std::max_element(edge_graph.begin(), edge_graph.end(),
                                     [](const Edge& edge_0, const Edge& edge_1) {
                                       return order_edge_vertex(&edge_0).second < order_edge_vertex(&edge_1).second;
                                     });
  reserve(std::max(iter->first, iter->second), edge_graph.size());
  FOR_EACH(edge, edge_graph) insert(edge);
  shrink_to_fit();
}

// ---------------------------------------------------------------------------------------------------------------------
// Modifiers
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details If the edge is already in the graph the function returns immediately. If the edge contains a vertex index
 * greater than the current graph size, the size of the graph is increased to accommodate the insertion. Lower bound is
 * then used to insert the upper index of the edge into the lower index adjacency list. The offset values are then
 * increased by one until the upper index is reached. The lower index is then inserted into the upper index adjacency
 * list. Finally the offset values are increase by two till the end of the offset vector.
 */
template<bool _directed>
bool Adjacency_Graph<_directed>::insert(const Edge& edge) {
  ASSERT_DEBUG(edge.first != edge.second, "Edge vertices identical, " + std::to_string(edge.first) + " and "
                                          + std::to_string(edge.second) + ".");

  // Preliminaries check if the edge exists, then determine if we need to resize.
  if(contains(edge)) return false;
  const auto& [i_first_vertex, i_second_vertex] =
   !_directed ? order_edge_vertex(&edge) : std::pair<const std::size_t&, const std::size_t&>({edge.first, edge.second});
  if(std::max(i_first_vertex, i_second_vertex)  >= size_vertex()) resize(std::max(i_first_vertex, i_second_vertex) + 1);

  // Insert lower vertex.
  insert_vertex_adjacent_list(i_first_vertex, i_second_vertex);
  std::for_each(std::next(offset.begin(), static_cast<s_size_t>(i_first_vertex + 1)),
                !_directed ? std::next(offset.begin(), static_cast<s_size_t>(i_second_vertex + 2)) : offset.end(),
                [](std::size_t& off){off++;}); 
  if(_directed) return true;

  // Insert upper vertex (tricky: last for loop is safe -> The highest vertex + 1 (for size) + 1 (for offset) <= end()).
  insert_vertex_adjacent_list(i_second_vertex, i_first_vertex);
  ++(*std::next(offset.begin(), static_cast<s_size_t>(i_second_vertex + 1)));
  std::for_each(std::next(offset.begin(), static_cast<s_size_t>(i_second_vertex + 2)), offset.end(),
                [](std::size_t& off){off += 2;});

  

  return true;
}

/**
 * @details Loops over the data and determines the new offsets and which entries in vertex adjacent list need to be
 * removed, updating the offsets and recording the entries to be deleted. After which both the offset vector and the
 * vertex adjacency list are 'looped' over and all entries flagged for removal are erased. Finally all vertices in the
 * vertex adjacency list are relabeled.
 */
template<bool _directed>
template<class _unary_predicate>
void Adjacency_Graph<_directed>::erase_if(_unary_predicate delete_vertex) {

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
    } else { // All adjacency entries need to be removed, since the vertex is to be removed.
      removed += std::distance(adjacency.first, adjacency.second);
      std::transform(begin_delete, end_delete, begin_delete, [](const auto) { return true; });
    }
  }
  offset.back() -= removed;

  // Erase offset entries for the removed vertices.
  i_vertex = 0;
  offset.erase(std::remove_if(offset.begin(), offset.end(),
                              [&](const auto) { return i_vertex < size_vertex() ? delete_vertex(i_vertex++) : false; }),
               offset.end());

  // Erase removed vertices from the adjacency lists, and then relabel.
  i_vertex = 0;
  vertex_adjacent_list.erase(std::remove_if(vertex_adjacent_list.begin(), vertex_adjacent_list.end(),
                                            [&](const auto) { return adjacency_delete[i_vertex++]; }),
                             vertex_adjacent_list.end());
  std::transform(vertex_adjacent_list.begin(), vertex_adjacent_list.end(), vertex_adjacent_list.begin(),
                 [&](const auto& i_old_vertex) { return new_indexes[i_old_vertex]; });

  ASSERT(offset.back() == vertex_adjacent_list.size(),
         "Total offsets no longer match vertex size while reducing to subgraph, " + std::to_string(offset.back())
         + " vs. " + std::to_string(vertex_adjacent_list.size()) + ".");
};

/**
 * @details Resizes the graph. If the graph is smaller than the new size the offset list is grown out, essentially
 * representing the addition of a disconnected point cloud. If the size is, however, smaller than the current size of
 * the graph the vertices with an index greater or equal to the new size are removed. The 'edges' connecting the removed
 * vertices from the remaining vertices are sought and removed from the graph.
 */
template<bool _directed>
void Adjacency_Graph<_directed>::resize(const std::size_t& size) {

  // Branch based on expansion or contraction of the graph.
  if(size_vertex() < size) {
    offset.resize(size + 1, offset.empty() ? 0 : offset.back());
  }
  else {
    // Resize the containers.
    offset.resize(size + 1);
    vertex_adjacent_list.resize(offset.back());

    // Loop over remaining vertices, looking for edges which no longer exist.
    std::size_t offset_carry = 0;
    FOR(i_vertex, size_vertex()) {
      auto [adjacency_begin, adjacency_end] = vertex_adjacency_iter(i_vertex);
      const auto erase_iter = std::lower_bound(adjacency_begin, adjacency_end, size);
      if(erase_iter != adjacency_end) {
        const auto offset_current = std::distance(erase_iter, adjacency_end);
        vertex_adjacent_list.erase(erase_iter, adjacency_end);
        offset[i_vertex + 1] -= offset_current;
        offset_carry += offset_current;
      }
      offset[i_vertex + 2] -= offset_carry;
    }
  }
}

// -------------------------------------------------------------------------------------------------------------------
// Lookup
// -------------------------------------------------------------------------------------------------------------------

/**
 * @details First ensures that the vertex exists in the graph, then using std::ranges looks for the second vertex of the
 * edge in the adjacency graph of the first.
 */
template<bool _directed>
[[nodiscard]] bool Adjacency_Graph<_directed>::contains(const Edge& edge) const {
  ASSERT_DEBUG(edge.first != edge.second, "Edge vertices identical, " + std::to_string(edge.first) + " and "
                                          + std::to_string(edge.second) + ".");

  if(empty()) return false;
  if(std::max(edge.first, edge.second) >= size_vertex()) return false;
  auto iter = std::lower_bound((*this)[edge.first].begin(), (*this)[edge.first].end(), edge.second);
  return iter != (*this)[edge.first].end() && *iter == edge.second;
}

// ---------------------------------------------------------------------------------------------------------------------
// Graph Operators
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details To begin new memory is allocated for a new graph. First the offsets for the new graph of determined by
 * computing and storing the offset for each old vertex and storing it in the new vertex position, and then by summing
 * the preceding offsets to obtain the new offset for each vertex. The new vertex lists are populated by coping and
 * renumbering the old vertex list's data to the new positions, using both the permutation vector and the newly computed
 * offsets. Each must also be resorted in an ascending fashion. Finally, the new graph is swapped with the current and
 * returned.
 */
template<bool _directed>
Adjacency_Graph<_directed> Adjacency_Graph<_directed>::reorder(const std::vector<std::size_t>& permutation) {

  ASSERT_DEBUG(permutation.size() == size_vertex(), "Incorrect sizes, " + std::to_string(permutation.size()) + " vs. "
                                                    + std::to_string(size_vertex()) + ".");
  ASSERT_DEBUG(std::accumulate(permutation.begin(), permutation.end(), 0.0) == (size_vertex() - 1)*size_vertex()/2.0,
               "Checksum for parsed re_ordering failed, are the elements unique?");

  // Allocate memory.
  Adjacency_Graph graph;
  graph.offset.resize(offset.size());
  graph.vertex_adjacent_list.resize(vertex_adjacent_list.size());

  // Set up the offsets for the new graph.
  if(!empty()) {
    graph.offset[0] = 0;
    FOR(i_old, size_vertex()) graph.offset[permutation[i_old] + 1] = offset[i_old + 1] - offset[i_old];
    FOR(i_new, size_vertex()) graph.offset[i_new + 1] += graph.offset[i_new];
  }

  // Populate the new graph using the reorder mapping.
  FOR(i_old, size_vertex()) {
    const std::size_t& i_new = permutation[i_old];
    auto adjacency_iter = vertex_adjacency_iter(i_old);
    std::transform(adjacency_iter.first, adjacency_iter.second,
                   std::next(graph.vertex_adjacent_list.begin(), static_cast<s_size_t>(graph.offset[i_new])),
                   [permutation](const std::size_t& i_old){return permutation[i_old];});
    adjacency_iter = graph.vertex_adjacency_iter(i_new);
    std::sort(adjacency_iter.first, adjacency_iter.second);
  }

  // swap class data and return.
  this->swap(graph);
  return graph;
}

// ---------------------------------------------------------------------------------------------------------------------
// Helper Functions
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @details This function inserts a new vertex into the adjacency list of the specified vertex in the graph. If the
 * inserted vertex is already present in the adjacency list, this function does nothing.
 */
template<bool _directed>
void Adjacency_Graph<_directed>::insert_vertex_adjacent_list(std::size_t vertex, std::size_t insert_vertex) {
  const auto& adjacency = vertex_adjacency_iter(vertex);
  const auto& insert_iter = std::lower_bound(adjacency.first, adjacency.second, insert_vertex);
  const auto& distance = std::distance(vertex_adjacent_list.begin(), insert_iter);
  vertex_adjacent_list.insert(vertex_adjacent_list.begin() + distance, insert_vertex);
}

//----------------------------------------------------------------------------------------------------------------------
// Operator Overloading
//----------------------------------------------------------------------------------------------------------------------
// Stream Operators
//----------------------------------------------------------------------------------------------------------------------

/**
 * @details Outputs the subgraph data to 'screen', each vertex is printed, followed by its adjacent vertices. If the
 * vertex adjacency is empty then a period is placed in the row.
 *
 * The output format looks like:
 * 1, 3
 * 0
 * .
 * 0, 1
 */
template<bool _directed>
std::ostream& operator<<(std::ostream& ostream, const Adjacency_Graph<_directed>& graph) {
  FOR(i_vertex, graph.size_vertex()) {
    if(i_vertex != 0) ostream<<"\n";
    FOR_EACH(adjacent_vertex, graph[i_vertex])
      ostream<<adjacent_vertex<<(adjacent_vertex != graph[i_vertex].back() ? ", " : "");
    if(graph[i_vertex].empty()) ostream<<".";
  }
  return ostream;
}

}

//----------------------------------------------------------------------------------------------------------------------
// STL specialisation
//----------------------------------------------------------------------------------------------------------------------

namespace std {

/**
 * @details Specialization of the std::hash template for the Disa::Adjacency_Graph class. It calculates a hash value for
 * the adjacency graph by XORing the hash values of its size, number of edges, and sizes of its first and last vertices.
 * If the graph is empty, the hash value is set to zero.
 */
template<bool _directed>
std::size_t hash<Disa::Adjacency_Graph<_directed> >::operator()(const Disa::Adjacency_Graph<_directed>& graph) const noexcept {
  if(graph.empty()) return 0; // All empty graphs are considered identical.
  std::size_t hashValue = 0;
  hashValue ^= std::hash<std::size_t> {}(graph.size_vertex());
  hashValue ^= std::hash<std::size_t> {}(graph.size_edge());
  hashValue ^= std::hash<std::size_t> {}(graph.front().size());
  hashValue ^= std::hash<std::size_t> {}(graph.back().size());
  return hashValue;
}

}