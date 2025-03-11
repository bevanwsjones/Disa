# Disa

![GitHub](https://img.shields.io/github/license/bevanwsjones/Disa)
[![CMake](https://github.com/bevanwsjones/Disa/actions/workflows/cmake.yml/badge.svg)](https://github.com/bevanwsjones/Disa/actions/workflows/cmake.yml)

General purpose linear algebra library with sparse solving capabilities, which will ultimately be 'rolled' into a CFD solver (currently not public). 

The full cloud-based project Fyn-Tech can be found [here](https://github.com/fyn-tech), where the current development focus can be found.

## Design Goals:

- Easy to use linear algebra objects, static vs. dynamic vs. sparse
- Containers should feel natural to use, almost as an extension of the `C++ stl`.

## Behaviour Notes:

- Dynamically sized types are promoted to statically sized when interacting with static types.
  
  i.e. `static type` + `dynamic type` -> `static type`
