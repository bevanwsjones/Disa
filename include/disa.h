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
// File Name: disa.h
// Description: Library header, adds all relevant includes for sub-libraries.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef DISA_DISA_H
#define DISA_DISA_H

// ---------------------------------------------------------------------------------------------------------------------
// Core library headers
// ---------------------------------------------------------------------------------------------------------------------

#include "matrix_sparse.h"
#include "matrix_dense.h"
#include "scalar.h"
#include "vector_dense.h"
#include "vector_operators.h"

// ---------------------------------------------------------------------------------------------------------------------
// Graphing library headers
// ---------------------------------------------------------------------------------------------------------------------

#include "adjacency_graph.h"
#include "edge.h"
#include "partition.h"
#include "reorder.h"

// ---------------------------------------------------------------------------------------------------------------------
// Solver library headers
// ---------------------------------------------------------------------------------------------------------------------

#include "solver.h"

#endif //DISA_DISA_H
