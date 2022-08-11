# Disa

![GitHub](https://img.shields.io/github/license/bevanwsjones/Disa)
[![cpp application](https://img.shields.io/github/workflow/status/bevanwsjones/Disa/CMake/main)](https://github.com/bevanwsjones/Disa/actions/workflows/cmake.yml)
[![Documentation](https://img.shields.io/badge/docs-docsforge-blue)](http://https://disa.docsforge.com/)

General purpose linear algebra library with sparse solving capabilities.

## Install:

- TBD

## Design Goals:

- Easy to use linear algebra objects, static vs. dynamic vs. sparse
    - Interoperability of containers.

## Behaviour Notes:

- Dynamically sized types are promoted to statically sized when interacting with static types.
  
  i.e. `static type` + `dynamic type` -> `static type`
