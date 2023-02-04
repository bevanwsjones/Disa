# Disa

![GitHub](https://img.shields.io/github/license/bevanwsjones/Disa)
[![CMake](https://github.com/bevanwsjones/Disa/actions/workflows/cmake.yml/badge.svg)](https://github.com/bevanwsjones/Disa/actions/workflows/cmake.yml)

General purpose linear algebra library with sparse solving capabilities.

## Install:

- TBD

## Design Goals:

- Easy to use linear algebra objects, static vs. dynamic vs. sparse
  - Containers should feel natural to use, almost as an extension of the `C++ stl`.

## Behaviour Notes:

- Dynamically sized types are promoted to statically sized when interacting with static types.
  
  i.e. `static type` + `dynamic type` -> `static type`
