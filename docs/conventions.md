# Conventions

## General Convention

Disa follows, as much as possible,
the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).
If there is a need to deviate from this then this should be added to the documentation of the relevant section in
code. As suggested by the `Core Guidelines` `snake_case` is used for most variable names and `Pascal_Snake_Case` for
classes and created types, while `SCREAMING_SNAKE_CASE` should be used for macros. General `K&R layout` should be
used and the line length is set to 120 characters. However, occasional inline documentation commenting may exceed
this.

## Naming

## Documentation

## Do's and don't

- As much as possible avoid macros, and preprocessor directives. Its well documented that the standard is trying to
  slowly remove them.
- Macros that act as functions should be writen in such a way that the `;` is still required at the end of a statement.