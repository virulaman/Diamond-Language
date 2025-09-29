#!/bin/bash
# DMO Language Compiler - RPM Package Builder

set -e

PKG_NAME="dmo-compiler"
PKG_VERSION="1.0.0"
PKG_RELEASE="1"
PKG_ARCH="x86_64"

echo "Building DMO Compiler .rpm package..."

# Install rpmbuild if not present
if ! command -v rpmbuild &> /dev/null; then
    echo "Installing rpm-build tools..."
    if command -v dnf &> /dev/null; then
        sudo dnf install -y rpm-build rpmdevtools
    elif command -v yum &> /dev/null; then
        sudo yum install -y rpm-build rpmdevtools
    else
        echo "Error: Package manager not found. Please install rpm-build manually."
        exit 1
    fi
fi

# Setup RPM build environment
rpmdev-setuptree

# Build the compiler
echo "Compiling DMO compiler..."
make clean
make dmo

# Create source tarball
TARBALL="${PKG_NAME}-${PKG_VERSION}.tar.gz"
mkdir -p /tmp/${PKG_NAME}-${PKG_VERSION}
cp -r . /tmp/${PKG_NAME}-${PKG_VERSION}/
cd /tmp
tar --exclude='.git' --exclude='*.o' --exclude='dmo' -czf $TARBALL ${PKG_NAME}-${PKG_VERSION}
cp $TARBALL ~/rpmbuild/SOURCES/
cd - > /dev/null

# Create RPM spec file
cat > ~/rpmbuild/SPECS/${PKG_NAME}.spec << EOF
Name:           ${PKG_NAME}
Version:        ${PKG_VERSION}
Release:        ${PKG_RELEASE}%{?dist}
Summary:        DMO Programming Language Compiler

License:        MIT
URL:            https://github.com/dmo-lang/dmo-compiler
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc make
Requires:       glibc libgcc

%description
DMO (Dynamic Modular Operations) is a C-implemented programming language
with C#-like syntax and built-in graphics capabilities. Features include:

- C#-like syntax with C-like program structure
- Unique I/O functions using show.txt() syntax
- Built-in graphics library (dmo_graphs) with SVG output
- Module system with 'use' statements
- Complete compiler with lexer, parser, AST, and interpreter

This package provides the DMO compiler executable and example programs.

%prep
%autosetup

%build
make clean
make dmo

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_bindir}
mkdir -p %{buildroot}%{_datadir}/dmo/examples
mkdir -p %{buildroot}%{_docdir}/%{name}

# Install binary
install -m 755 dmo %{buildroot}%{_bindir}/

# Install examples
install -m 644 examples/*.dmo %{buildroot}%{_datadir}/dmo/examples/

# Install documentation
install -m 644 replit.md %{buildroot}%{_docdir}/%{name}/README.md

%files
%license
%doc %{_docdir}/%{name}/README.md
%{_bindir}/dmo
%{_datadir}/dmo/examples/*.dmo

%changelog
* $(date "+%a %b %d %Y") DMO Development Team <dev@dmo-lang.org> - ${PKG_VERSION}-${PKG_RELEASE}
- Initial release of DMO Programming Language Compiler
- Complete C-based compiler implementation
- Support for C#-like syntax with C structure
- Built-in graphics library with SVG output
- Module system with stdlib and dmo_graphs
- Example programs included
EOF

# Build RPM package
echo "Building .rpm package..."
rpmbuild -ba ~/rpmbuild/SPECS/${PKG_NAME}.spec

# Copy built packages to current directory
cp ~/rpmbuild/RPMS/${PKG_ARCH}/${PKG_NAME}-${PKG_VERSION}-${PKG_RELEASE}.*.${PKG_ARCH}.rpm .
cp ~/rpmbuild/SRPMS/${PKG_NAME}-${PKG_VERSION}-${PKG_RELEASE}.*.src.rpm .

echo ""
echo "========================================"
echo "RPM packages built successfully!"
echo ""
echo "Binary package: ${PKG_NAME}-${PKG_VERSION}-${PKG_RELEASE}.*.${PKG_ARCH}.rpm"
echo "Source package: ${PKG_NAME}-${PKG_VERSION}-${PKG_RELEASE}.*.src.rpm"
echo ""
echo "Installation commands:"
echo "Fedora/RHEL/CentOS: sudo dnf install ${PKG_NAME}-${PKG_VERSION}-${PKG_RELEASE}.*.${PKG_ARCH}.rpm"
echo "Or: sudo rpm -ivh ${PKG_NAME}-${PKG_VERSION}-${PKG_RELEASE}.*.${PKG_ARCH}.rpm"
echo ""
echo "Removal: sudo dnf remove $PKG_NAME"
echo "Or: sudo rpm -e $PKG_NAME"
echo "========================================"