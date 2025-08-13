#!/usr/bin/env python3
"""
Blue Package Manager for Diamond Language
A complete package management system for Diamond Language modules
"""

import os
import sys
import json
import urllib.request
import urllib.parse
import subprocess
import argparse
from pathlib import Path

# Configuration
API_BASE_URL = "https://diamond-lang.herokuapp.com"  # Update with your deployed URL
LOCAL_PACKAGES_DIR = os.path.expanduser("~/.diamond/packages")
CACHE_FILE = os.path.expanduser("~/.diamond/cache.json")

def ensure_dirs():
    """Ensure required directories exist"""
    os.makedirs(LOCAL_PACKAGES_DIR, exist_ok=True)
    os.makedirs(os.path.dirname(CACHE_FILE), exist_ok=True)

def api_request(endpoint, method="GET", data=None):
    """Make API request to Diamond Language server"""
    url = f"{API_BASE_URL}/api/{endpoint}"
    
    try:
        if method == "GET":
            with urllib.request.urlopen(url) as response:
                return json.loads(response.read().decode())
        elif method == "POST":
            req = urllib.request.Request(url, data=data.encode() if data else None)
            req.add_header('Content-Type', 'application/json')
            with urllib.request.urlopen(req) as response:
                return json.loads(response.read().decode())
    except Exception as e:
        print(f"Error connecting to Diamond Language server: {e}")
        print("Make sure the Diamond Language web service is running")
        return None

def list_packages():
    """List all available packages"""
    print("📦 Available Diamond Language Packages:")
    print("=" * 50)
    
    # Try to get from API
    result = api_request("packages")
    if result and "packages" in result:
        for pkg in result["packages"]:
            print(f"📌 {pkg['name']} v{pkg['version']}")
            print(f"   Author: {pkg['author']}")
            print(f"   Downloads: {pkg['downloads']}")
            print(f"   {pkg['description']}")
            print()
    else:
        # Fallback to built-in packages
        packages = [
            ("audio", "1.0.0", "Audio processing and sound generation"),
            ("gamedev", "1.2.0", "Game development utilities and graphics"),
            ("crypto", "1.0.1", "Cryptography and security functions"),
            ("network", "1.1.0", "Network programming and HTTP utilities"),
            ("ai", "1.0.0", "Artificial intelligence and machine learning"),
            ("database", "1.3.0", "Database connectivity and ORM tools"),
        ]
        
        for name, version, desc in packages:
            print(f"📌 {name} v{version}")
            print(f"   {desc}")
            print()

def install_package(package_name):
    """Install a package"""
    print(f"🔄 Installing {package_name}...")
    
    # Check if package exists via API
    result = api_request(f"packages/{package_name}")
    if not result:
        print(f"❌ Package '{package_name}' not found")
        return False
    
    # Simulate installation by calling install endpoint
    install_result = api_request(f"packages/{package_name}/install", method="POST")
    if install_result:
        print(f"✅ Package '{package_name}' installed successfully!")
        print(f"📖 Description: {result.get('description', 'No description')}")
        
        # Create local package marker
        package_dir = os.path.join(LOCAL_PACKAGES_DIR, package_name)
        os.makedirs(package_dir, exist_ok=True)
        
        # Save package info
        info_file = os.path.join(package_dir, "info.json")
        with open(info_file, "w") as f:
            json.dump(result, f, indent=2)
        
        return True
    else:
        print(f"❌ Failed to install '{package_name}'")
        return False

def search_packages(query):
    """Search for packages"""
    print(f"🔍 Searching for '{query}'...")
    
    result = api_request("packages")
    if result and "packages" in result:
        found = []
        for pkg in result["packages"]:
            if query.lower() in pkg["name"].lower() or query.lower() in pkg["description"].lower():
                found.append(pkg)
        
        if found:
            print(f"Found {len(found)} package(s):")
            for pkg in found:
                print(f"📌 {pkg['name']} v{pkg['version']} - {pkg['description']}")
        else:
            print("No packages found matching your search")
    else:
        print("Unable to search packages - server unavailable")

def package_info(package_name):
    """Get detailed package information"""
    result = api_request(f"packages/{package_name}")
    if result:
        print(f"📦 Package Information: {package_name}")
        print("=" * 50)
        print(f"Name: {result['name']}")
        print(f"Version: {result['version']}")
        print(f"Author: {result['author']}")
        print(f"Description: {result['description']}")
        print(f"Downloads: {result['downloads']}")
        print(f"Created: {result.get('created_at', 'Unknown')}")
    else:
        print(f"❌ Package '{package_name}' not found")

def upload_package(file_path):
    """Upload a package file"""
    if not os.path.exists(file_path):
        print(f"❌ File not found: {file_path}")
        return False
    
    print(f"⬆️ Uploading {file_path}...")
    print("Note: Use the web interface for full folder uploads with .c + .md + .h files")
    print(f"Web interface: {API_BASE_URL}/packages")
    
    # For now, direct users to web interface for uploads
    return True

def list_installed():
    """List locally installed packages"""
    if not os.path.exists(LOCAL_PACKAGES_DIR):
        print("No packages installed")
        return
    
    packages = [d for d in os.listdir(LOCAL_PACKAGES_DIR) if os.path.isdir(os.path.join(LOCAL_PACKAGES_DIR, d))]
    
    if packages:
        print("📦 Installed Packages:")
        print("=" * 30)
        for pkg in packages:
            info_file = os.path.join(LOCAL_PACKAGES_DIR, pkg, "info.json")
            if os.path.exists(info_file):
                with open(info_file) as f:
                    info = json.load(f)
                print(f"✅ {info['name']} v{info['version']}")
            else:
                print(f"✅ {pkg}")
    else:
        print("No packages installed")

def main():
    parser = argparse.ArgumentParser(description="Blue Package Manager for Diamond Language")
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # List command
    subparsers.add_parser("list", help="List all available packages")
    
    # Install command
    install_parser = subparsers.add_parser("install", help="Install a package")
    install_parser.add_argument("package", help="Package name to install")
    
    # Search command
    search_parser = subparsers.add_parser("search", help="Search for packages")
    search_parser.add_argument("query", help="Search query")
    
    # Info command
    info_parser = subparsers.add_parser("info", help="Get package information")
    info_parser.add_argument("package", help="Package name")
    
    # Upload command
    upload_parser = subparsers.add_parser("upload", help="Upload a package")
    upload_parser.add_argument("file", help="File to upload")
    
    # Installed command
    subparsers.add_parser("installed", help="List installed packages")
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return
    
    ensure_dirs()
    
    if args.command == "list":
        list_packages()
    elif args.command == "install":
        install_package(args.package)
    elif args.command == "search":
        search_packages(args.query)
    elif args.command == "info":
        package_info(args.package)
    elif args.command == "upload":
        upload_package(args.file)
    elif args.command == "installed":
        list_installed()

if __name__ == "__main__":
    main()