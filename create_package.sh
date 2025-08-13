#!/bin/bash
# Create complete Diamond Language distribution package

echo "📦 Creating Diamond Language distribution package..."

# Package name
PACKAGE_NAME="diamond-language-complete"
PACKAGE_DIR="$PACKAGE_NAME"
ARCHIVE_NAME="$PACKAGE_NAME.tar.gz"

# Clean previous package
rm -rf "$PACKAGE_DIR" "$ARCHIVE_NAME" 2>/dev/null

# Create package directory
mkdir -p "$PACKAGE_DIR"

echo "📁 Copying source files..."

# Copy all source files
cp *.c *.h "$PACKAGE_DIR/"
cp Makefile "$PACKAGE_DIR/"

# Copy documentation
cp README.md INSTALL.md FEATURES.md QUICKSTART.md LICENSE "$PACKAGE_DIR/"
cp .gitignore "$PACKAGE_DIR/"

# Copy examples
cp -r examples "$PACKAGE_DIR/"

# Copy Blue package manager
cp blue blue_package_manager.py "$PACKAGE_DIR/"

# Copy installers
cp install.sh setup.py "$PACKAGE_DIR/"
cp build_windows.bat "$PACKAGE_DIR/"

# Copy pre-built packages
cp *.deb *.tar.gz "$PACKAGE_DIR/" 2>/dev/null || true

# Make scripts executable
chmod +x "$PACKAGE_DIR/install.sh"
chmod +x "$PACKAGE_DIR/setup.py"
chmod +x "$PACKAGE_DIR/blue"

echo "📝 Creating installation instructions..."

# Create quick install file
cat > "$PACKAGE_DIR/INSTALL_NOW.txt" << 'EOF'
# Diamond Language - Quick Installation

## Automatic Installation (Recommended)

### Linux/macOS:
```bash
./install.sh
```

### Cross-Platform (Python):
```bash
python3 setup.py
```

### Windows:
```bash
python setup.py
```

## What You Get

After installation:
- Diamond Language compiler (dmo)
- Blue package manager (blue)
- Complete examples and documentation
- System-wide installation (optional)

## First Program

Create hello.dmo:
```diamond
use stdlib;
int main() {
    show.txt("Hello, Diamond World!");
    return 0;
}
```

Run it:
```bash
./dmo hello.dmo
```

## Package Manager

```bash
./blue list                # List packages
./blue install audio      # Install packages
./blue info gamedev       # Package info
```

Ready to start programming in Diamond Language!
EOF

echo "🗜️  Creating archive..."

# Create compressed archive
tar -czf "$ARCHIVE_NAME" "$PACKAGE_DIR"

echo "🧹 Cleaning up..."
rm -rf "$PACKAGE_DIR"

echo "✅ Package created: $ARCHIVE_NAME"
echo "📊 Package size: $(du -h "$ARCHIVE_NAME" | cut -f1)"
echo ""
echo "🚀 Usage:"
echo "   tar -xzf $ARCHIVE_NAME"
echo "   cd $PACKAGE_NAME"
echo "   ./install.sh"
echo ""
echo "📤 Ready for distribution!"