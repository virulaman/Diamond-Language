#!/bin/bash
# DMO Language Compiler - Debian Package Builder

set -e

PKG_NAME="dmo-compiler"
PKG_VERSION="1.0.0"
PKG_ARCH="amd64"
PKG_DIR="${PKG_NAME}_${PKG_VERSION}_${PKG_ARCH}"

echo "Building DMO Compiler .deb package..."

# Clean up previous builds
rm -rf $PKG_DIR *.deb

# Create package directory structure
mkdir -p $PKG_DIR/DEBIAN
mkdir -p $PKG_DIR/usr/bin
mkdir -p $PKG_DIR/usr/share/dmo/examples
mkdir -p $PKG_DIR/usr/share/doc/dmo-compiler

# Build the compiler
echo "Compiling DMO compiler..."
make clean
make dmo

# Copy binary
cp dmo $PKG_DIR/usr/bin/

# Copy examples
cp examples/*.dmo $PKG_DIR/usr/share/dmo/examples/

# Create control file
cat > $PKG_DIR/DEBIAN/control << EOF
Package: $PKG_NAME
Version: $PKG_VERSION
Architecture: $PKG_ARCH
Maintainer: DMO Development Team <dev@dmo-lang.org>
Depends: libc6 (>= 2.17), libgcc1 (>= 1:3.0)
Section: devel
Priority: optional
Homepage: https://github.com/dmo-lang/dmo-compiler
Description: DMO Programming Language Compiler
 DMO (Dynamic Modular Operations) is a C-implemented programming language
 with C#-like syntax and built-in graphics capabilities. Features include:
 .
  - C#-like syntax with C-like program structure
  - Unique I/O functions using show.txt() syntax
  - Built-in graphics library (dmo_graphs) with SVG output
  - Module system with 'use' statements
  - Complete compiler with lexer, parser, AST, and interpreter
 .
 This package provides the DMO compiler executable and example programs.
EOF

# Create copyright file
cat > $PKG_DIR/usr/share/doc/dmo-compiler/copyright << EOF
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: dmo-compiler
Upstream-Contact: DMO Development Team <dev@dmo-lang.org>
Source: https://github.com/dmo-lang/dmo-compiler

Files: *
Copyright: 2025 DMO Development Team
License: MIT

License: MIT
 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:
 .
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 .
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
EOF

# Create changelog
cat > $PKG_DIR/usr/share/doc/dmo-compiler/changelog.Debian << EOF
dmo-compiler (1.0.0) stable; urgency=low

  * Initial release of DMO Programming Language Compiler
  * Complete C-based compiler implementation
  * Support for C#-like syntax with C structure
  * Built-in graphics library with SVG output
  * Module system with stdlib and dmo_graphs
  * Example programs included

 -- DMO Development Team <dev@dmo-lang.org>  $(date -R)
EOF

# Compress changelog
gzip -9 $PKG_DIR/usr/share/doc/dmo-compiler/changelog.Debian

# Create README
cat > $PKG_DIR/usr/share/doc/dmo-compiler/README << EOF
DMO Programming Language Compiler
=================================

DMO is a modern programming language with C#-like syntax and built-in
graphics capabilities, implemented entirely in C.

Usage:
  dmo program.dmo

Examples are installed in: /usr/share/dmo/examples/

For more information, visit: https://github.com/dmo-lang/dmo-compiler
EOF

# Set permissions
chmod 755 $PKG_DIR/usr/bin/dmo
chmod 644 $PKG_DIR/usr/share/dmo/examples/*
chmod 644 $PKG_DIR/usr/share/doc/dmo-compiler/*

# Build package
echo "Building .deb package..."
dpkg-deb --build $PKG_DIR

echo "Package built successfully: ${PKG_DIR}.deb"
echo ""
echo "To install: sudo dpkg -i ${PKG_DIR}.deb"
echo "To remove: sudo apt remove $PKG_NAME"