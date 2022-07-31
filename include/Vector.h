//----------------------------------------------------------------------------------------------------------------------
// MIT License
// Copyright (c) 2022 bevanwsjones
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
//----------------------------------------------------------------------------------------------------------------------
// File Name: Vector.h
// Description:
//----------------------------------------------------------------------------------------------------------------------

//TODO: fill in above
#ifndef DISA_VECTOR_H
#define DISA_VECTOR_H

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <numeric>
#include <vector>

namespace Disa{

template<std::size_t _size>
struct Vector : public std::array<double, _size>
{
  typedef Vector<_size> t_vector;
  const static bool dynamic = false;

  //--------------------------------------------------------------------------------------------------------------------
  // Arithmetic Operators
  //--------------------------------------------------------------------------------------------------------------------

  constexpr t_vector& operator*(const double& scalar)
  {
    for(auto& element: *this) element *= scalar;
    return *this;
  }


};//Vector

template<>
struct Vector<0> : public std::vector<double>
{
  typedef Vector<0> t_vector;
  const static bool dynamic = true;

  Vector<0>(std::initializer_list<double> list)
  {
    resize(list.size());
    auto iter = begin();
    for(const auto& item: list) *iter++ = item;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Arithmetic Operators
  //--------------------------------------------------------------------------------------------------------------------

  inline t_vector& operator*(const double& scalar)
  {
    std::for_each(this->begin(), this->end(), [&](const double& element)
    {return element*scalar;});
    return *this;
  }

};//Vector

}//Disa

#endif //DISA_VECTOR_H
