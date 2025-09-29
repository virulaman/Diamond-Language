# DMO Programming Language Compiler

DMO (Dynamic Modular Operations) is a C-implemented programming language with C#-like syntax and built-in graphics capabilities.

## Features

- **C#-like syntax** with C-like program structure
- **Unique I/O system** using `show.txt("")` for output
- **Built-in graphics library** (`dmo_graphs`) with SVG output
- **Module system** with `use <module>` syntax
- **Complete compiler** with lexer, parser, AST, and interpreter

## Installation

### Pre-built Packages

#### Windows
1. Download `dmo-compiler-windows.zip`
2. Extract and run `build_windows.bat`
3. Use `dmo.exe program.dmo`

#### Debian/Ubuntu (.deb)
```bash
chmod +x build_deb.sh
./build_deb.sh
sudo dpkg -i dmo-compiler_1.0.0_amd64.deb
```

#### Fedora/RHEL (.rpm)
```bash
chmod +x build_rpm.sh
./build_rpm.sh
sudo dnf install dmo-compiler-1.0.0-1.*.x86_64.rpm
```

#### Universal Linux
```bash
chmod +x build_universal.sh
./build_universal.sh
```

### Build from Source
```bash
make clean
make dmo
```

## Usage

### Basic Program
```dmo
use stdlib;

int main() {
    show.txt("Hello, World!");
    return 0;
}
```

### Graphics Program
```dmo
use dmo_graphs;

int main() {
    dmo.gr.create.window("My Window", 400, 400);
    dmo.gr.create.sqr(50, 50, 100, 100);
    dmo.gr.create.crle(200, 200, 75);
    return 0;
}
```

### Run Programs
```bash
dmo program.dmo
```

## Language Syntax

### I/O Functions
- `show.txt("text")` - Print text
- `show.txt("Value:", variable)` - Print with variables
- `scanf("%d")` - Input (returns string/number)
- `fget("file.txt")` - Read file

### Graphics Functions
- `dmo.gr.create.window(title, width, height)` - Create window
- `dmo.gr.create.sqr(x, y, width, height)` - Draw rectangle
- `dmo.gr.create.crle(x, y, radius)` - Draw circle
- `dmo.gr.create.line(length)` - Draw line
- `dmo.gr.create.crve(radius, param)` - Draw curve

### Modules
- `use stdlib` - Standard library functions
- `use dmo_graphs` - Graphics library

### Data Types
- `int` - Integer numbers
- `string` - Text strings
- `char` - Single characters

## Examples

The compiler includes three example programs:
- `examples/hello.dmo` - Basic I/O demonstration
- `examples/graphics_demo.dmo` - Graphics capabilities
- `examples/modules_demo.dmo` - Complete feature showcase

## VS Code Integration

1. Install the DMO compiler using one of the methods above
2. Create `.dmo` files in VS Code
3. Use the integrated terminal to compile: `dmo program.dmo`
4. Graphics output generates SVG files you can view directly

## Architecture

- **Lexer** (`lexer.c`) - Tokenization
- **Parser** (`parser.c`) - AST generation
- **Interpreter** (`interpreter.c`) - Program execution
- **Modules** (`modules.c`) - Dynamic module loading
- **Graphics** (`dmo_graphs.c`) - SVG generation
- **Standard Library** (`stdlib_funcs.c`) - Built-in functions

## License

MIT License - See build scripts for full license text.

## Development

Built with:
- C99 standard
- GCC compiler
- Make build system
- Cross-platform compatibility