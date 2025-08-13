# Installation Guide

## Quick Installation

### Option 1: Pre-built Package (Ubuntu/Debian)
```bash
sudo dpkg -i dmo-compiler_1.0.0_amd64.deb
dmo --version
```

### Option 2: Source Distribution
```bash
tar -xzf dmo-compiler-distribution.tar.gz
cd dmo-compiler/
make clean && make all
sudo make install
```

### Option 3: Build from Source
```bash
git clone https://github.com/virulaman/Diamond-Language.git
cd Diamond-Language/
make clean && make all
```

### Option 4: Windows
```cmd
# Extract files and run
build_windows.bat
```

## Verify Installation

```bash
# Test the compiler
./dmo examples/hello.dmo

# Should output:
# Hello, World!
# Welcome to Diamond programming language
```

## System Requirements

- **Linux**: GCC, Make, LibM
- **Windows**: MinGW or Visual Studio
- **macOS**: Xcode Command Line Tools

## Troubleshooting

### Build Errors
```bash
# Install dependencies on Ubuntu/Debian
sudo apt-get install build-essential

# Install on CentOS/RHEL
sudo yum groupinstall "Development Tools"
```

### Permission Issues
```bash
# Make files executable
chmod +x dmo
chmod +x build_windows.bat
```

## Next Steps

1. Run examples: `make examples`
2. Read documentation: `README.md`
3. Try the web interface
4. Install Blue packages