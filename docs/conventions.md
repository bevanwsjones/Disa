# Conventions

Items in _italics_ are still tbd/under consideration.

## Introduction

Disa follows, as much as possible,
the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).
If there is a need to deviate from this then this should be added to the documentation of the relevant section in
code.

## General Conventions:

- One can refer to core guidelines in using `CG-x`, where `x` indicates the rule being referred to.
- Layout: `K&R layout`
- Variable names: `snake_case`
- Classes and defined types names: `Pascal_Snake_Case`
- Macros: `SCREAMING_SNAKE_CASE`
- Line length: 120 characters, however, occasional inline (documentation) commenting may exceed this.
- Tabs: 2 white space characters

## File Layout Conventions

### hpp Files
- Template definitions must be placed inside a `.hpp` file, unless inlined.
- `.hpp` files must only be included in their corresponding `.h` file.
- `.hpp` files should be included outside all `namespace`s.
- `using namespace` may not be used globally withing an `.hpp` file.
- Do not place header guards inside `.hpp` files.
- Do not include any files inside an `.hpp` file.

## Technical Conventions:

- For (vertex/edge) indices `std::numeric_limits<std::size_t>::max()` will be used as the default 'no data' option, 
  since -1 is not available for unsigned types. Rational for `max` is -1 would overflow/roll over to `max`.

## Naming

In order to keep the project ordered and consistent some conventions regarding naming have been adopted.

| #   | Rule                       | Example                          | Rational                                                                             | 
|-----|----------------------------|----------------------------------|--------------------------------------------------------------------------------------|
| 1.  | General-to-specific names  | Matrix_Sparse, Matrix_Dense      | Helps keep related concepts close together when  alphabetically sorted.              |
| 2.  | Avoid short hand names*    | `row` rather than `r`            | Helps readability.                                                                   |
| 3.  | Do not used plural names   | `vector` rather than `vectors`   | Can create bugs when similar objects are only have an 's' different in there names.  |
| 4.  | Prefix templates with `_`  | `template<std::size_t _size>`    | While his is a slight deviation of CG-NL.5, it  allows name reuse for class members. |
| 5.  | Prefix indexes with `i_`   | `i_row`                          | Helps readability.                                                                   |

\* subject to know/accepted short hands below:

| Abbreviation | Expanded      |
|--------------|---------------| 
| abs          | absolute      |
| config       | configuration |
| coef         | coefficient   |
| i_           | index         |
| iter         | iteration     |
| min          | minimum       |
| max          | maximum       |

## Documentation and Commenting

All source code should be properly documented in a manner consistent with [doxygen](https://github.com/doxygen/doxygen)
.  
Note that `Disa` uses `@` style for indicating the type of comment, e.g. `@brief Comment`. 

Use blocking to separate out sections of code, especially in large files and classes. Below is an example of a block
comment:

```cpp
// --------------------------------------------------------------------------------------------------------------------
// Statically Sized Dense Vector Class
// ---------------------------------------------------------------------------------------------------------------------
```

## Do's and don't

- As much as possible avoid macros, and preprocessor directives. Its well documented that the standard is trying to
  slowly remove them.
- Macros that act as functions should be writen in such a way that the `;` is still required at the end of a statement.