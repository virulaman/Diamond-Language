#!/bin/bash
# DMO Language Compiler - Universal Linux Package Builder

set -e

echo "DMO Compiler - Universal Linux Package Builder"
echo "=============================================="

# Function to detect Linux distribution
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        echo $ID
    elif type lsb_release >/dev/null 2>&1; then
        lsb_release -si | tr '[:upper:]' '[:lower:]'
    elif [ -f /etc/lsb-release ]; then
        . /etc/lsb-release
        echo $DISTRIB_ID | tr '[:upper:]' '[:lower:]'
    else
        echo "unknown"
    fi
}

# Make build scripts executable
chmod +x build_deb.sh build_rpm.sh

DISTRO=$(detect_distro)
echo "Detected distribution: $DISTRO"
echo ""

case $DISTRO in
    ubuntu|debian|mint|pop)
        echo "Building .deb package for Debian-based system..."
        ./build_deb.sh
        ;;
    fedora|rhel|centos|rocky|almalinux)
        echo "Building .rpm package for Red Hat-based system..."
        ./build_rpm.sh
        ;;
    arch|manjaro)
        echo "For Arch-based systems, you can build from source:"
        echo "1. git clone <this-repo>"
        echo "2. cd dmo-compiler"
        echo "3. make clean && make dmo"
        echo "4. sudo cp dmo /usr/local/bin/"
        ;;
    *)
        echo "Unknown distribution. Building both .deb and .rpm packages..."
        echo ""
        echo "Building .deb package..."
        if command -v dpkg-deb >/dev/null 2>&1; then
            ./build_deb.sh
        else
            echo "dpkg-deb not found, skipping .deb build"
        fi

        echo ""
        echo "Building .rpm package..."
        if command -v rpmbuild >/dev/null 2>&1 || command -v dnf >/dev/null 2>&1 || command -v yum >/dev/null 2>&1; then
            ./build_rpm.sh
        else
            echo "RPM build tools not found, skipping .rpm build"
        fi
        ;;
esac

echo ""
echo "=============================================="
echo "Build completed!"
echo ""
echo "Generated files:"
ls -la *.deb *.rpm 2>/dev/null || echo "No packages generated (check build logs above)"
echo ""
echo "Usage after installation:"
echo "  dmo program.dmo"
echo ""
echo "Example programs will be installed in:"
echo "  /usr/share/dmo/examples/ (system-wide)"
echo "=============================================="