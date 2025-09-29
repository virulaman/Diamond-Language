# DMO Programming Language

## Overview

This project is a fully functional C-implemented programming language called DMO (Dynamic Modular Operations). The language features C#-like syntax with C-like program structure, unique I/O functions, graphics capabilities, and a module system. The entire compiler is implemented in C and includes lexer, parser, AST generator, and interpreter components.

## User Preferences

Preferred communication style: Simple, everyday language.

## Language Features

### Core Language Features
- **C#-like syntax**: Familiar syntax for developers coming from C# backgrounds
- **C-like structure**: Traditional C program structure with main() function entry point
- **Unique I/O system**: `show.txt("")` for printing text and variables
- **Graphics library**: Built-in `dmo_graphs` with `dmo.gr.*` namespace for graphics operations
- **Module system**: `use <module>` syntax for importing modules (stdlib, dmo_graphs)
- **Data types**: Support for int, string, char with proper type checking

### Compiler Architecture
- **Lexer**: Tokenizes source code and handles DMO-specific syntax
- **Parser**: Generates Abstract Syntax Tree (AST) from tokens
- **AST Generator**: Creates structured representation of the program
- **Interpreter**: Executes the AST with proper function calls and module loading
- **Module System**: Handles dynamic loading of stdlib and graphics modules

### Graphics System
- **dmo_graphs library**: Built-in graphics functionality
- **SVG output**: Generates SVG files for graphics rendering
- **Primitive operations**: create.window, create.sqr, create.crle, create.line, create.crve
- **Coordinate system**: Standard X/Y positioning for graphics elements

## Build System

### Compilation
- **Makefile**: Automated build system with clean, compile, and example targets
- **GCC compiler**: Uses C99 standard with proper warning flags
- **Static linking**: All components linked into single executable

### Project Structure
```
├── main.c              # Entry point and file handling
├── lexer.c/.h          # Lexical analysis and tokenization
├── parser.c/.h         # Syntax parsing and AST generation
├── ast.c/.h            # Abstract Syntax Tree definitions
├── interpreter.c/.h    # Program execution engine
├── modules.c/.h        # Module loading system
├── stdlib_funcs.c/.h   # Standard library functions
├── dmo_graphs.c/.h     # Graphics library implementation
├── examples/           # Sample DMO programs
└── Makefile           # Build configuration
```

### Example Programs
- **hello.dmo**: Demonstrates basic I/O with show.txt function
- **graphics_demo.dmo**: Shows graphics capabilities with SVG output
- **modules_demo.dmo**: Complex example with functions, loops, and graphics

## Recent Changes
- **2025-08-13**: Fixed critical issue with main function execution
- **2025-08-13**: Resolved builtin function call routing problems
- **2025-08-13**: Successfully implemented automatic main function invocation
- **2025-08-13**: All example programs now execute correctly with proper output
- **2025-08-13**: Graphics system generates SVG files as expected