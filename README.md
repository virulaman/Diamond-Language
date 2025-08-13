# Diamond Programming Language

## Overview

This project is a fully functional C-implemented programming language called Diamond. The language features C#-like syntax with C-like program structure, unique I/O functions, graphics capabilities, and a module system. The entire compiler is implemented in C and includes lexer, parser, AST generator, and interpreter components.

## User Preferences

Preferred communication style: Simple, everyday language.

## Language Features

### Core Language Features
- **C#-like syntax**: Familiar syntax for developers coming from C# backgrounds
- **C-like structure**: Traditional C program structure with main() function entry point
- **Unique I/O system**: `show.txt("")` for printing text and variables
- **Graphics library**: Built-in graphics system with `dmo.gr.*` namespace for graphics operations
- **Module system**: `use <module>` syntax for importing modules (stdlib, dmo_graphs, request)
- **Data types**: Support for int, string, char with proper type checking
- **Math operations**: Full arithmetic support (+, -, *, /, %) with C-style precedence
- **OS integration**: System command execution with `system()`, `cls`, `clear` functions
- **HTTP capabilities**: Request module with `request.get()` and `request.post()` functions

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
- **math_demo.dmo**: Demonstrates all math operations including modulo and OS commands
- **request_demo.dmo**: Shows HTTP request capabilities with external APIs
- **advanced_demo.dmo**: Comprehensive demo of all language features

## Recent Changes
- **2025-08-13**: Fixed deployment configuration for production readiness:
  - **Health Endpoints**: Added `/health`, `/healthz`, `/ready`, and `/status` endpoints for deployment monitoring
  - **Production Entry Points**: Created `main.py`, `wsgi.py`, and `Procfile` for deployment
  - **Gunicorn Configuration**: Added `gunicorn.conf.py` for production WSGI server with proper logging
  - **Environment Variables**: Support for PORT and FLASK_ENV configuration
  - **Startup Scripts**: Created `start.sh` for flexible production/development startup
  - **Deployment Config**: Added `deploy.json` with proper health check configuration
  - **Response Formatting**: Fixed health endpoints to return proper JSON responses
- **2025-08-13**: Enhanced Diamond Language website with complete security and admin features:
  - **Documentation System**: Complete guide to Diamond Language and all Blue packages
  - **Secure Package Upload**: Strict .c, .h, .md file validation prevents script injection
  - **Admin Panel**: Password-protected admin access (password: diamond123)
  - **Public Extensions Browser**: Community package viewing with search functionality
  - **Domain Research**: Found cheapest options - diamond-lang.net for $0.70/year at IONOS
- **2025-08-13**: Successfully implemented complete "Blue" package manager system:
  - **Package Installation**: `blue install <module>` command for module management
  - **Extension Uploads**: `blue upload <file.c>` for community extensions
  - **Module Registry**: API backend with searchable module database
  - **6 Surprise Modules**: audio, gamedev, crypto, network, ai, database modules
  - **Complete Package Ecosystem**: Install, search, list, info, upload functionality
- **2025-08-13**: Advanced Diamond language features completed:
  - **Math Module**: sin(), cos(), tan(), sigmoid(), sqrt(), pow() functions
  - **Graphics S
