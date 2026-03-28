# CLAUDE.md

This file provides guidance to AI Agent when working with code in this repository.

## Build Commands

```bash
# Configure (from project root)
cmake -B cmake-build-agent -S .

# Build
cmake --build cmake-build-agent

# Run executable
./cmake-build-debug/mdir/src/mdir
```

## Project Overview

- **Language**: C++20
- **Build System**: CMake 4.0+

## Code Style

### Formatting
- C++20 standard, 4-space indent
- Allman (BSD) brace style: opening `{` always on its own line
- Single-statement `if` / `for` / `while` may omit braces; multi-statement bodies require braces
- Spaces around `*` and `&` in declarations: `int const & ref`, `Cell const * ptr`
- No spaces for unary operators in expressions: `*ptr`, `&var`, `!flag`
- Headers: `.h` (normal), `.hpp` (header-only / template-heavy)

### Naming (`readability-identifier-naming`)
- **camelCase**: functions, variables, parameters, struct/class fields
- **PascalCase**: classes, structs, enums, enum values
- **UPPER_CASE**: `static const`, `static constexpr`, namespace-scope constants
- Private/protected member variables: underscore **prefix** (`_cellWidth`)

### const Placement — East const (`misc-const-correctness`)
- `const` goes after the type it qualifies: `Type const &` not `const Type &`
- Applies to references, pointers, and local/member variables
- `auto * p = expr;` / `auto const & r = expr;` — always qualify `auto` (`readability-qualified-auto`)
- `constexpr` is unaffected (remains before the type)
- Method const stays at the end: `int size() const`

### Modern C++ Idioms
- `nullptr` not `NULL` or `0` (`modernize-use-nullptr`)
- `using` over `typedef` (`modernize-use-using`)
- `override` on virtual overrides (`modernize-use-override`)
- Prefer in-class member initialization (`modernize-use-default-member-init`)
- Range-for with `auto const &` to avoid copies (`performance-for-range-copy`)
