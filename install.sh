#!/bin/bash
# Diamond Language Installer Script
# Automatically builds and installs the Diamond Language compiler

set -e

echo "🔷 Diamond Language Installer"
echo "=========================================="

# Check if we're in the right directory
if [ ! -f "main.c" ] || [ ! -f "Makefile" ]; then
    echo "❌ Error: Please run this script from the Diamond Language source directory"
    exit 1
fi

# Check for dependencies
echo "🔍 Checking dependencies..."

if ! command -v gcc &> /dev/null; then
    echo "❌ GCC compiler not found. Please install build tools:"
    echo "   Ubuntu/Debian: sudo apt-get install build-essential"
    echo "   CentOS/RHEL: sudo yum groupinstall 'Development Tools'"
    echo "   macOS: xcode-select --install"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo "❌ Make not found. Please install build tools."
    exit 1
fi

echo "✅ Dependencies found"

# Clean previous builds
echo "🧹 Cleaning previous builds..."
make clean 2>/dev/null || true

# Build the compiler
echo "🔨 Building Diamond Language compiler..."
if make all; then
    echo "✅ Build successful!"
else
    echo "❌ Build failed!"
    exit 1
fi

# Test the installation
echo "🧪 Testing installation..."
if [ -f "./dmo" ]; then
    echo "✅ Diamond compiler (dmo) created successfully"
    
    # Test with hello world example
    if [ -f "examples/hello.dmo" ]; then
        echo "🔍 Running test program..."
        if ./dmo examples/hello.dmo >/dev/null 2>&1; then
            echo "✅ Test program executed successfully"
        else
            echo "⚠️  Test program failed, but compiler was built"
        fi
    fi
else
    echo "❌ Compiler binary not found"
    exit 1
fi

# Install system-wide (optional)
echo ""
read -p "📦 Install Diamond Language system-wide? (y/N): " install_global

if [[ $install_global =~ ^[Yy]$ ]]; then
    echo "🔧 Installing system-wide..."
    
    # Create installation directories
    sudo mkdir -p /usr/local/bin
    sudo mkdir -p /usr/local/share/diamond
    sudo mkdir -p /usr/local/share/diamond/examples
    
    # Copy binary
    sudo cp dmo /usr/local/bin/
    sudo chmod +x /usr/local/bin/dmo
    
    # Copy examples
    sudo cp -r examples/* /usr/local/share/diamond/examples/
    
    # Copy Blue package manager
    if [ -f "blue" ]; then
        sudo cp blue /usr/local/bin/
        sudo chmod +x /usr/local/bin/blue
    fi
    
    if [ -f "blue_package_manager.py" ]; then
        sudo cp blue_package_manager.py /usr/local/share/diamond/
    fi
    
    echo "✅ Diamond Language installed to /usr/local/bin/dmo"
    echo "✅ Examples installed to /usr/local/share/diamond/examples/"
    echo "✅ Blue package manager installed to /usr/local/bin/blue"
    
    # Verify installation
    if command -v dmo &> /dev/null; then
        echo "🎉 Installation successful! You can now use 'dmo' from anywhere."
    else
        echo "⚠️  Installation completed but 'dmo' not in PATH. You may need to restart your terminal."
    fi
else
    echo "📍 Diamond Language compiler available as ./dmo in current directory"
    echo "📍 Blue package manager available as ./blue in current directory"
fi

echo ""
echo "🎉 Installation Complete!"
echo "=========================================="
echo "📖 Usage:"
echo "   ./dmo examples/hello.dmo      # Run example program"
echo "   ./dmo myprogram.dmo          # Run your program"
echo "   ./blue list                  # List available packages"
echo "   ./blue install audio        # Install packages"
echo ""
echo "📚 Documentation: README.md"
echo "🌐 Web interface: https://github.com/virulaman/Diamond-Language"
echo ""